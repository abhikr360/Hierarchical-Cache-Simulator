/*********** Author: Abhishek Kumar **************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <math.h>

#define SIZE 4194304
#define INVALID_TAG 0xfffffffffffffffULL
#define LLC_NUMSET 8192 /* 8 MB LLC: 8 X 1024 */
#define LLC_ASSOC 16


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


typedef unsigned long long ull;

//Structure to hold the counts used in probabilistic eviction scheme
typedef struct countTableEntry_s{
   ull pagenum;
   ull count[50];
   struct countTableEntry_s* next;
} countTableEntry_s;



//Structure to hold cache entry
typedef struct {
   ull tag;
   ull lru;
   ull hit_count;
   countTableEntry_s* countTableEntry;
    HashTableEntry *htPtr;
} CacheTag;


const double EPSILON = 0.001;

CacheTag** create_cache (int numset, int assoc) {
   /* Creates a cache with (number of sets = numset) and (associativity = assoc)*/
   int i, j;
   CacheTag ** cache = (CacheTag**)malloc(numset*sizeof(CacheTag*));
   assert(cache != NULL);
   for (i=0; i<numset; i++) {
      cache[i] = (CacheTag*)malloc(assoc*sizeof(CacheTag));
      assert(cache[i] != NULL);
      for ( j=0; j<assoc; j++) {
         cache[i][j].tag = INVALID_TAG;
         cache[i][j].hit_count=0;
      }
   }
   return cache;
}

countTableEntry_s* create_count_table(int size){
   countTableEntry_s* ct;
   ct = (countTableEntry_s*)malloc(size*sizeof(countTableEntry_s));
   assert(ct != NULL);
   int j;

   for (j=0; j<size; j++) {
      ct[j].pagenum=INVALID_TAG;
      ct[j].next = NULL;
   }
   return ct;
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
   const int LOG_PAGE_SIZE = atoi(argv[2]);
   const int n_counters = atoi(argv[3]);
   const int D = atoi(argv[4]);
   assert(n_counters<50);
   printf("LOG_PAGE_SIZE: %d n_counters %d D %d\n", LOG_PAGE_SIZE, n_counters, D);
   

   int  i, j, LLCsetid, maxindex, tid;
   ull block_addr, max, *uniqueId;
   char output_name[256], input_name[256];
   FILE *fp_in;
   int llcway;
   ull prob_cand;
   int hash_index, page_hash;
   CacheTag** LLCcache;
   countTableEntry_s* ct, *ctptr, *prevctptr;
   IntegerListEntry * ilptr;
   int block_type;
   ull pagenum;
   ull temp_hit_count;
   double prob[LLC_ASSOC], sum;
   HashTableEntry *ht, *prev, *ptr;
   ull next_use[LLC_ASSOC];
   int rank, rank_stat[LLC_ASSOC];
   if (argc != 5) {
      printf("Need 5 arguments: input file,LOG_PAGE_SIZE,n_counters,D. Aborting...\n");
      exit (1);
   }

   for(i=0;i<LLC_ASSOC;++i){
      rank_stat[i]=0;
   }

   LLCcache = (CacheTag**)create_cache(LLC_NUMSET, LLC_ASSOC);
   ct = (countTableEntry_s*)create_count_table(SIZE);
   ht = (HashTableEntry*)create_hash_table(SIZE);
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
      fscanf(fp_in, "%d %llu %d", &tid, &block_addr, &block_type);
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



   printf("Access list prepared.\n"); fflush(stdout);


   printf("Starting simulation...\n"); fflush(stdout);

   // Simulate
   ull num_misses=0;


   fp_in = fopen(input_name, "r");
   assert(fp_in != NULL);

   while (!feof(fp_in)) {
      fscanf(fp_in, "%d %llu %d", &tid, &block_addr, &block_type);

      LLCsetid = block_addr % LLC_NUMSET;
      pagenum = block_addr >> LOG_PAGE_SIZE;
      page_hash = pagenum % SIZE;
      hash_index = block_addr % SIZE;


      /* LLC cache lookup */
      for (llcway=0; llcway<LLC_ASSOC; llcway++) {
         if (LLCcache[LLCsetid][llcway].tag == block_addr) {
            /* LLC cache hit; Update access list */

            LLCcache[LLCsetid][llcway].hit_count++;
            temp_hit_count = LLCcache[LLCsetid][llcway].hit_count;
            ctptr = LLCcache[LLCsetid][llcway].countTableEntry;
            assert(ctptr != NULL);
            if(temp_hit_count<n_counters)
               ctptr->count[temp_hit_count]++;

            // LLCcache[LLCsetid][llcway].lru = uniqueId[LLCsetid];

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
            if (LLCcache[LLCsetid][llcway].tag == INVALID_TAG){ 
               break;
            }
         }

         if (llcway==LLC_ASSOC) {
            /* no invalid way*/
            /* Belady policy */

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
            for (prob_cand=0; prob_cand<LLC_ASSOC; prob_cand++) {
               if ((LLCcache[prob_cand][prob_cand].htPtr == NULL) || (LLCcache[LLCsetid][prob_cand].htPtr->currentPtr == NULL)) {
                  //Storing all the nextuses
                  next_use[prob_cand]=LLONG_MAX;
               }
               else{
                  next_use[prob_cand]=LLCcache[LLCsetid][prob_cand].htPtr->currentPtr->id;
               }
            }

            /* Probabilistic Policy Prediction*/
            sum=0;
            for (prob_cand=0; prob_cand<LLC_ASSOC; prob_cand++) {
               ctptr = LLCcache[LLCsetid][prob_cand].countTableEntry;
               assert(ctptr != NULL);
               temp_hit_count= LLCcache[LLCsetid][prob_cand].hit_count;
               temp_hit_count= (temp_hit_count > n_counters-2 ? n_counters-2 : temp_hit_count); 

               prob[prob_cand]= 1-(ctptr->count[temp_hit_count+1]*1.0)/ctptr->count[temp_hit_count];
               if(prob[prob_cand]-0 < EPSILON){
                  prob[prob_cand] += EPSILON;
               }
               if(1-prob[prob_cand]<EPSILON){
                  prob[prob_cand] -= EPSILON;
               }

               sum+=prob[prob_cand];
            }

            for(prob_cand=0;prob_cand<LLC_ASSOC;++prob_cand){// Normalizing the probability values
               prob[prob_cand] = prob[prob_cand]/sum;
            }
            for(prob_cand=1;prob_cand<LLC_ASSOC;++prob_cand){// Cumulative sum
               prob[prob_cand] += prob[prob_cand-1];
               
            }
            // assert ( abs(prob[LLC_ASSOC-1]-1) < 0.0001);
            sum = (double)rand() / (double)RAND_MAX;

            for(prob_cand=0;prob_cand<LLC_ASSOC;++prob_cand){// Normalizing the probability values
               if(prob[prob_cand]>=sum){
                  maxindex=prob_cand;
                  break;
               }
            }
            prob_cand = maxindex; //prob_cand index

            // Now, we calculate next use of the prob_cand
            prev = LLCcache[LLCsetid][prob_cand].htPtr;
            if(prev == NULL){
               prob_cand = LLONG_MAX;
            }
            else{
               ilptr = prev->currentPtr;
               if(ilptr==NULL){
                  prob_cand=LLONG_MAX;
               }
               else{
                  prob_cand = ilptr->id;
               }
            }
            // prob_cand = LLCcache[LLCsetid][prob_cand].htPtr->currentPtr->id; 
            // Now we count how many of them are greater than this one
            rank=0;
            for(i=0;i<LLC_ASSOC;++i){
               if(next_use[i]>prob_cand){
                  rank++;
               }
            }
            rank_stat[rank]++;
         }


         assert (llcway < LLC_ASSOC);
         
         LLCcache[LLCsetid][llcway].tag = block_addr;
         LLCcache[LLCsetid][llcway].hit_count=0;
         LLCcache[LLCsetid][llcway].htPtr = ptr;

         ctptr = &ct[page_hash];
         if(ctptr->pagenum == INVALID_TAG){ // No page at this idx
            ctptr->pagenum = pagenum;
            ctptr->next=NULL;
            for(j=0;j<n_counters;++j){
               ctptr->count[j]=0;
            }
         }
         else{  // A page at this idx already exists
            prevctptr = NULL;
            while(ctptr != NULL){
               if(ctptr->pagenum == pagenum ){
                  break;
               }
               prevctptr = ctptr;
               ctptr = ctptr->next;
            }
            if(ctptr == NULL){// This is the first block for this page
               prevctptr->next = (countTableEntry_s*)malloc(sizeof(countTableEntry_s));
               ctptr = prevctptr->next;
               ctptr->pagenum=pagenum;
               ctptr->next=NULL;
               for(j=0;j<n_counters;++j){
                  ctptr->count[j]=0;
               }
            }
         }
         LLCcache[LLCsetid][llcway].countTableEntry = ctptr;
         // LLCcache[LLCsetid][llcway].lru = uniqueId[LLCsetid];
         ctptr->count[0]++;

      }
      // uniqueId[LLCsetid]++;
   }
   fclose(fp_in);

   printf("Done Simulating!\n");
   ull tot=0;
   for (j=0; j<LLC_NUMSET; j++) {
      tot+=uniqueId[j];
   }
   

   printf("Miss rate: %lf\n", (num_misses*1.0)/tot);

   tot=0;
   for(j=0;j<LLC_ASSOC;++j){
      tot+=rank_stat[j];
   }
   for(j=0;j<LLC_ASSOC;++j){
      printf("%lf\n", rank_stat[j]/(double)tot);
   }
   
   printf("-------------------------------------------------------------------\n");
   return 0;
}
