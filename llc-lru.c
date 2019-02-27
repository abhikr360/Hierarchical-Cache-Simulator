/*********** Author: Abhishek Kumar **************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>


typedef struct {
   unsigned long long tag;
   unsigned long long lru;
} CacheTag;

#define SIZE 4194304
#define INVALID_TAG 0xfffffffffffffffULL

#define LLC_NUMSET 8192 /* 8 MB LLC: 8 X 1024 */
#define LLC_ASSOC 16



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


int main (int argc, char **argv)
{
   int  i, LLCsetid, maxindex, tid;
   unsigned long long block_addr, max, *uniqueId;
   char output_name[256], input_name[256];
   FILE *fp_in;
   int llcway;
   int hash_index;
   CacheTag** LLCcache;
   int block_type;


   if (argc != 2) {
      printf("Need two arguments: input file. Aborting...\n");
      exit (1);
   }

   LLCcache = (CacheTag**)create_cache(LLC_NUMSET, LLC_ASSOC);

   /* The following array is used to find the sequence number for an access to a set;
   This sequence number acts as the timestamp for the access */
   uniqueId = (unsigned long long*)malloc(LLC_NUMSET*sizeof(unsigned long long));
   assert(uniqueId != NULL);
   for (i=0; i<LLC_NUMSET; i++) {
      uniqueId[i] = 0;
   }
   printf("Starting simulation...\n"); fflush(stdout);

   // Simulate
   unsigned long long num_misses=0;

   sprintf(input_name, "%s", argv[1]);
   fp_in = fopen(input_name, "r");
   assert(fp_in != NULL);

   while (!feof(fp_in)) {
      fscanf(fp_in, "%d %llu %d", &tid, &block_addr, &block_type);

      hash_index = block_addr % SIZE;
      LLCsetid = block_addr % LLC_NUMSET;

      /* LLC cache lookup */
      for (llcway=0; llcway<LLC_ASSOC; llcway++) {
         if (LLCcache[LLCsetid][llcway].tag == block_addr) {
            /* LLC cache hit; Update last use*/
            LLCcache[LLCsetid][llcway].lru = uniqueId[LLCsetid];
            break;
         }
      }
      if (llcway==LLC_ASSOC) {
         /* LLC cache miss */
         num_misses++;
         /* find victim block and replace it with current block */

         /* check if there is invalid way */
         for (llcway=0; llcway<LLC_ASSOC; llcway++) {
            if (LLCcache[LLCsetid][llcway].tag == INVALID_TAG) break;
         }

         if (llcway==LLC_ASSOC) {
            /* no invalid way; find MIN */ 
            max = LLONG_MAX;
            for (llcway=0; llcway<LLC_ASSOC; llcway++) {
               if (LLCcache[LLCsetid][llcway].lru < max) {
                  max = LLCcache[LLCsetid][llcway].lru;
                  maxindex = llcway;
               }
            }
            llcway = maxindex;
         }
         assert (llcway < LLC_ASSOC);
         
         LLCcache[LLCsetid][llcway].tag = block_addr;
         LLCcache[LLCsetid][llcway].lru = uniqueId[LLCsetid];
      }
      uniqueId[LLCsetid]++;
      
   }
   fclose(fp_in);

   printf("Done reading file!\n");
   unsigned long long tot=0;
   for (i=0; i<LLC_NUMSET; i++) {
      tot+=uniqueId[i];
   }
   printf("Miss rate: %lf\n", (num_misses*1.0)/tot);
   return 0;
}
