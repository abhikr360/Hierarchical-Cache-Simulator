/*********** Author: Mainak Chaudhuri **************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// A list of integers
// Used to maintain the list of access timestamps for a particular block
typedef struct integerList_s {
   unsigned long long id;
   struct integerList_s *next;
} IntegerListEntry;

// This hash table is used by the optimal policy for maintaining the next access stamp
// Each hash table entry has a block address and a list of access timestamps to that block
typedef struct hashTableEntry_s {
   unsigned long long block_addr;     // Block address
   IntegerListEntry *ilhead;    // Head of the access timestamp list
   IntegerListEntry *tail;      // Tail of the access timestamp list
   IntegerListEntry *currentPtr;// Pointer to the current position in the access list during simulation 
   struct hashTableEntry_s *next;
} HashTableEntry;

typedef struct {
   unsigned long long tag;
   unsigned long long lru;
   HashTableEntry *htPtr;       // A pointer to the corresponding hash table entry
                                // Each block address gets a unique hash table entry
} CacheTag;

#define SIZE 4194304
#define INVALID_TAG 0xfffffffffffffffULL

#define LLC_NUMSET 8192 /* 8 MB LLC: 8 X 1024 */
#define LLC_ASSOC 16
typedef unsigned long long ull;


CacheTag** create_cache (int numset, int assoc) {
/* Creates a cache with (number of sets = numset) and (associativity = assoc)
*/
   int i, j;
   CacheTag ** cache = (CacheTag**)malloc(numset*sizeof(CacheTag*));
   assert(cache != NULL);
   for (i=0; i<numset; i++) {
      cache[i] = (CacheTag*)malloc(assoc*sizeof(CacheTag));
      assert(cache[i] != NULL);
      for ( j=0; j<assoc; j++) {
         cache[i][j].tag = INVALID_TAG;
      }
   }
   return cache;
}


HashTableEntry* create_hash_table(int size) {
/* create a hash table of (size=size) */
   int j;
   HashTableEntry* ht;
   ht = (HashTableEntry*)malloc(SIZE*sizeof(HashTableEntry));
   assert(ht != NULL);
   for (j=0; j<SIZE; j++) {
      ht[j].ilhead = NULL;
      ht[j].tail = NULL;
   }
   return ht;
}


int main (int argc, char **argv)
{
   int  i, LLCsetid, maxindex, tid;
   unsigned long long block_addr, max, *uniqueId;
   char output_name[256], input_name[256];
   FILE *fp_in;
   // FILE *fp_out;
   int llcway;
   HashTableEntry *ht, *prev, *ptr;
   int hash_index;
   CacheTag** LLCcache;
   int block_type;
   ull pc;

   if (argc != 2) {
      printf("Need two arguments: input file. Aborting...\n");
      exit (1);
   }

   LLCcache = (CacheTag**)create_cache(LLC_NUMSET, LLC_ASSOC);
   ht = (HashTableEntry*)create_hash_table(SIZE);

   /* The following array is used to find the sequence number for an access to a set;
   This sequence number acts as the timestamp for the access */
   uniqueId = (unsigned long long*)malloc(LLC_NUMSET*sizeof(unsigned long long));
   assert(uniqueId != NULL);
   for (i=0; i<LLC_NUMSET; i++) {
      uniqueId[i] = 0;
   }

   /* Build the hash table of accesses */
   sprintf(input_name, "%s", argv[1]);
   // sprintf(output_name, "%s", argv[2]);
   fp_in = fopen(input_name, "r");
   // fp_out = fopen(output_name, "w");
   assert(fp_in != NULL);
   // assert(fp_out != NULL);

   while (!feof(fp_in)) {
      fscanf(fp_in, "%llu %d %llu %d", &pc, &tid, &block_addr, &block_type);

      // printf("%llu %d %llu %d\n", pc, tid, block_addr, block_type);
      hash_index = block_addr  % SIZE;
      LLCsetid = block_addr % LLC_NUMSET;

      if (ht[hash_index].ilhead == NULL) {
         ht[hash_index].block_addr = block_addr;
         ht[hash_index].ilhead = (IntegerListEntry*)malloc(sizeof(IntegerListEntry));
         assert(ht[hash_index].ilhead != NULL);
         ht[hash_index].tail = ht[hash_index].ilhead;
         ht[hash_index].ilhead->id = uniqueId[LLCsetid];
         ht[hash_index].ilhead->next = NULL;
         ht[hash_index].currentPtr = ht[hash_index].ilhead;  // Initialize to point to the beginning of the list
         ht[hash_index].next = NULL;
      }
      else {
         prev = NULL;
         ptr = &ht[hash_index];
         while (ptr != NULL) {
            if (ptr->block_addr == block_addr) {
               assert(ptr->ilhead != NULL);
               assert(ptr->tail->next == NULL);
               ptr->tail->next = (IntegerListEntry*)malloc(sizeof(IntegerListEntry));
               assert(ptr->tail->next != NULL);
               ptr->tail = ptr->tail->next;
               ptr->tail->id = uniqueId[LLCsetid];
               ptr->tail->next = NULL;
               break;
            }
            prev = ptr;
            ptr = ptr->next;
         }
         if (ptr == NULL) {
            assert(prev->next == NULL);
            ptr = (HashTableEntry*)malloc(sizeof(HashTableEntry));
            assert(ptr != NULL);
            ptr->block_addr = block_addr;
            ptr->ilhead = (IntegerListEntry*)malloc(sizeof(IntegerListEntry));
            assert(ptr->ilhead != NULL);
            ptr->tail = ptr->ilhead;
            ptr->tail->id = uniqueId[LLCsetid];
            ptr->tail->next = NULL;
            ptr->next = NULL;
            ptr->currentPtr = ptr->ilhead;
            prev->next = ptr;
         }
      }
      uniqueId[LLCsetid]++;
   }
   fclose(fp_in);
   printf("Done reading file!\n");



   printf("Access list prepared.\nStarting simulation...\n"); fflush(stdout);

   // Simulate
   // sprintf(input_name, "%s", argv[1]);

   unsigned long long num_misses=0;

   fp_in = fopen(input_name, "r");
   assert(fp_in != NULL);

   while (!feof(fp_in)) {
      fscanf(fp_in, "%llu %d %llu %d", &pc, &tid, &block_addr, &block_type);

      hash_index = block_addr % SIZE;
      LLCsetid = block_addr % LLC_NUMSET;

      /* LLC cache lookup */
      for (llcway=0; llcway<LLC_ASSOC; llcway++) {
         if (LLCcache[LLCsetid][llcway].tag == block_addr) {
            /* LLC cache hit; Update access list */
            assert(LLCcache[LLCsetid][llcway].htPtr != NULL);
            assert(LLCcache[LLCsetid][llcway].htPtr->block_addr == block_addr);
            assert(LLCcache[LLCsetid][llcway].htPtr->currentPtr != NULL);
            LLCcache[LLCsetid][llcway].htPtr->currentPtr = LLCcache[LLCsetid][llcway].htPtr->currentPtr->next;
            break;
         }
      }
      if (llcway==LLC_ASSOC) {
         /* LLC cache miss */
         num_misses++;
         /* Access list pointer needs to be advanced; 
         Search the entry in hash table */
         ptr = &ht[hash_index];
         while (ptr != NULL) {
            if (ptr->block_addr == block_addr) break;
            ptr = ptr->next;
         }
         assert(ptr != NULL);
         assert(ptr->currentPtr != NULL);
         ptr->currentPtr = ptr->currentPtr->next; // Advance to point to the next access

         /* find victim block and replace it with current block */

         /* check if there is invalid way */
         for (llcway=0; llcway<LLC_ASSOC; llcway++) {
            if (LLCcache[LLCsetid][llcway].tag == INVALID_TAG) break;
         }

         if (llcway==LLC_ASSOC) {
            /* no invalid way; find MIN */ 
            // printf("Min\n");
            max = 0;
            for (llcway=0; llcway<LLC_ASSOC; llcway++) {
               if ((LLCcache[llcway][llcway].htPtr == NULL) || (LLCcache[LLCsetid][llcway].htPtr->currentPtr == NULL)) {
               /* No future access (eternally dead) */
                  maxindex = llcway;
                  break;
               }
               if (LLCcache[LLCsetid][llcway].htPtr->currentPtr->id >= max) {
                  max = LLCcache[LLCsetid][llcway].htPtr->currentPtr->id;
                  maxindex = llcway;
               }
            }
            llcway = maxindex;
         }
         assert (llcway < LLC_ASSOC);
         
         LLCcache[LLCsetid][llcway].tag = block_addr;
         LLCcache[LLCsetid][llcway].htPtr = ptr;    // Set up the hash table pointer
      }
      
   }
   fclose(fp_in);

   printf("Done reading file!\n");

   /* Sanity check terminal state
   All access lists must have been exhausted */
   for (i=0; i<SIZE; i++) {
      if (ht[i].ilhead != NULL) {
         ptr = &ht[i];
         while (ptr != NULL) {
            assert(ptr->currentPtr == NULL);
            ptr = ptr->next;
         }
      }
   }

   // fclose(fp_out);

   unsigned long long tot=0;
   for (i=0; i<LLC_NUMSET; i++) {
      tot+=uniqueId[i];
   }
   printf("Miss rate %s: %lf\n", input_name,(num_misses*1.0)/tot);
   return 0;
}
