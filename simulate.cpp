#include "constants.h"

// #include "LLC.h"

#include "L2Cache.h"

#include "L1Cache.h"

// #ifndef CE_Belady_included
// #define CE_Belady_included
// #include "CE_Belady.h"
// #endif



// map<ull, CE_Belady*> block_data;

int main(int argc, char const *argv[])
{
	/*
	Setup your cache Hierarchy
	*/

	// L1Cache *l1 = new L1Cache(3,4, LRU);

	// LLC* llc = new LLC(1,2);

	// L2Cache *l2= new L2Cache(1,2);


	// l1->set_child(l2);
	// l2->set_parent(l1);

	
	// l2->set_child(llc);
	// llc->set_parent(l2);


	/*
	Preproceesing needed for simulation
	*/



	/*
	string folder = "../output_trace/test/";
	string file_path = folder + "addrtrace_bodytrack.out" ; //to_string(argv[1]);

	FILE *fp_in;

	fp_in = fopen(file_path.c_str(), "r");
	assert(fp_in != NULL);

	unsigned int tid;
	ull block_addr;
	ull count=0;
	map<ull, CE_Belady*>::iterator ir;
	while(!feof(fp_in)){
		fscanf(fp_in, "%d %llu", &tid, &block_addr);
		ir = block_data.find(block_addr);
		if(ir ==block_data.end()){
			block_data.insert(mp(block_addr, new CE_Belady(block_addr, tid, count)));
		}

		else{
			if(ir->second->cur_owner != tid){
				ir->second->add_sharer(tid);
			}
			ir->second->access_list.pb(mp(count, tid));
		}
      count++;
   }
   fclose(fp_in);
   printf("Done reading file!\n");
*/
	ifstream input_file("temp_trace.out");

	unsigned int tid;
	ull block_addr;

	L1Cache* l1 = new L1Cache(4,3,LRU);
	L2Cache* l2 = new L2Cache(2,3,LRU);
	l1->set_child(l2);
	l2->set_parent(l1);

	while(input_file >> tid >> block_addr){
		// cout << tid << " " << block_addr << "a" << endl;
		l1->find_in_cache(block_addr);
   }
   // fclose(fp_in);

	return 0;
}