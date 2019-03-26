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


const double EPSILON = 0.001;
typedef unsigned long long ull;

//Structure to hold the counts used in probabilistic eviction scheme
typedef struct countTableEntry_s{
   ull pc;
   ull count[50];
   struct countTableEntry_s* next;
} countTableEntry_s;

//Structure to hold cache entry
typedef struct {
   ull tag;
   ull lru;
   ull hit_count;
   countTableEntry_s* countTableEntry;
} CacheTag;



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
      ct[j].pc=INVALID_TAG;
      ct[j].next = NULL;
   }
   return ct;
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
   int hash_index;
   CacheTag** LLCcache;
   countTableEntry_s* ct, *ctptr, *prevctptr;
   int block_type;
   ull pc;
   ull temp_hit_count;
   double prob[LLC_ASSOC], sum;

   if (argc != 5) {
      printf("Need 5 arguments: input file,LOG_PAGE_SIZE,n_counters,D. Aborting...\n");
      exit (1);
   }

   LLCcache = (CacheTag**)create_cache(LLC_NUMSET, LLC_ASSOC);
   ct = (countTableEntry_s*)create_count_table(SIZE);
   uniqueId = (unsigned long long*)malloc(LLC_NUMSET*sizeof(unsigned long long));
   assert(uniqueId != NULL);
   for (i=0; i<LLC_NUMSET; i++) {
      uniqueId[i] = 0;
   }

   printf("Starting simulation...\n"); fflush(stdout);

   // Simulate
   ull num_misses=0;

   sprintf(input_name, "%s", argv[1]);
   fp_in = fopen(input_name, "r");
   assert(fp_in != NULL);

   while (!feof(fp_in)) {
      fscanf(fp_in, "%llu %d %llu %d", &pc, &tid, &block_addr, &block_type);

      LLCsetid = block_addr % LLC_NUMSET;
      pc = pc >> LOG_PAGE_SIZE;
      hash_index = pc % SIZE;

      printf("%lld\n", pc);
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
            if (LLCcache[LLCsetid][llcway].tag == INVALID_TAG){ 
               
               break;
            }
         }

         if (llcway==LLC_ASSOC) {
            /* no invalid way*/



            if(uniqueId[LLCsetid]>D){
            /* Probabilistic Policy*/
               sum=0;
               for (llcway=0; llcway<LLC_ASSOC; llcway++) {
                  ctptr = LLCcache[LLCsetid][llcway].countTableEntry;
                  assert(ctptr != NULL);
                  temp_hit_count= LLCcache[LLCsetid][llcway].hit_count;
                  temp_hit_count= (temp_hit_count > n_counters-2 ? n_counters-2 : temp_hit_count); 

                  prob[llcway]= 1-(ctptr->count[temp_hit_count+1]*1.0)/ctptr->count[temp_hit_count];
                  if(prob[llcway]-0 < EPSILON){
                     prob[llcway] += EPSILON;
                  }
                  if(1-prob[llcway]<EPSILON){
                     prob[llcway] -= EPSILON;
                  }

                  sum+=prob[llcway];
               }

               for(llcway=0;llcway<LLC_ASSOC;++llcway){// Normalizing the probability values
                  prob[llcway] = prob[llcway]/sum;
               }
               for(llcway=1;llcway<LLC_ASSOC;++llcway){// Cumulative sum
                  prob[llcway] += prob[llcway-1];
                  
               }
               // assert ( abs(prob[LLC_ASSOC-1]-1) < 0.0001);
               sum = (double)rand() / (double)RAND_MAX;

               for(llcway=0;llcway<LLC_ASSOC;++llcway){// Normalizing the probability values
                  if(prob[llcway]>=sum){
                     maxindex=llcway;
                     break;
                  }
               }
               llcway = maxindex;
               // return 0;
            }
            else{
                max = LLONG_MAX;
               for (llcway=0; llcway<LLC_ASSOC; llcway++) {
                  if (LLCcache[LLCsetid][llcway].lru < max) {
                     max = LLCcache[LLCsetid][llcway].lru;
                     maxindex = llcway;
                  }
               }
               llcway = maxindex;
            }

         }
         assert (llcway < LLC_ASSOC);
         
         LLCcache[LLCsetid][llcway].tag = block_addr;
         LLCcache[LLCsetid][llcway].hit_count=0;
         

         ctptr = &ct[hash_index];
         if(ctptr->pc == INVALID_TAG){ // No page at this idx
            ctptr->pc = pc;
            ctptr->next=NULL;
            for(j=0;j<n_counters;++j){
               ctptr->count[j]=0;
            }
         }
         else{  // A page at this idx already exists
            prevctptr = NULL;
            while(ctptr != NULL){
               if(ctptr->pc == pc ){
                  break;
               }
               prevctptr = ctptr;
               ctptr = ctptr->next;
            }
            if(ctptr == NULL){// This is the first block for this page
               prevctptr->next = (countTableEntry_s*)malloc(sizeof(countTableEntry_s));
               ctptr = prevctptr->next;
               ctptr->pc=pc;
               ctptr->next=NULL;
               for(j=0;j<n_counters;++j){
                  ctptr->count[j]=0;
               }
            }
         }
         LLCcache[LLCsetid][llcway].countTableEntry = ctptr;
         LLCcache[LLCsetid][llcway].lru = uniqueId[LLCsetid];
         ctptr->count[0]++;

      }
      uniqueId[LLCsetid]++;
   }
   fclose(fp_in);

   printf("Done Simulating!\n");
   ull tot=0;
   for (j=0; j<LLC_NUMSET; j++) {
      tot+=uniqueId[j];
   }
   

   printf("Miss rate: %lf\n", (num_misses*1.0)/tot);

   printf("-------------------------------------------------------------------\n");
   return 0;
}
