#include "constants.h"
#include "L2Cache.h"
#include "L1Cache.h"


int NUM_CORES = 8;
int main(int argc, char const *argv[])
{

	if (argc != 2) {
      printf("Need two arguments: input file. Aborting...\n");
      exit (1);
   }

	const string file_path(argv[1]);
	ifstream input_file(file_path);
	/******************************
		Simulation
	******************************/
	int associativity_l1 = 8;
	int associativity_l2 = 8;
	int num_sets_l1      = 64;
	int num_sets_l2      = 256;


	unsigned int tid;
	ull block_addr;
	int category;
	ull pc;

	L1Cache** l1data = new L1Cache*[NUM_CORES];
	L1Cache** l1instruction = new L1Cache*[NUM_CORES];
	L2Cache** l2unified = new L2Cache*[NUM_CORES];

	for(int i=0;i<NUM_CORES;++i){
		l1data[i] = new L1Cache(num_sets_l1, associativity_l1, 1, i, LRU);
		l1instruction[i] = new L1Cache(num_sets_l1, associativity_l1, INSTRUCTION, i, LRU);

		l2unified[i] = new L2Cache(num_sets_l2, associativity_l2, 1, i, LRU);

		l1data[i]->set_child(l2unified[i]);
		l1instruction[i]->set_child(l2unified[i]);
		l2unified[i]->set_parent(l1data[i], l1instruction[i]);
	}

	while(input_file >> pc >> tid >> block_addr >> category){
		tid = tid%NUM_CORES;
		if(category==INSTRUCTION){
			l1instruction[tid]->find_in_cache(block_addr, category, pc);
		}
		else{
			l1data[tid]->find_in_cache(block_addr, category, pc);
		}
   }
   input_file.close();

   for(int i=0;i<NUM_CORES;++i){
		delete l1data[i];
		delete l1instruction[i];
		delete l2unified[i];
	}
	delete l1data;
	delete l1instruction;
	delete l2unified;

	return 0;
}