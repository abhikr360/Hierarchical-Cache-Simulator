#include "constants.h"
#include "LLC.h"



map<ull, vector<ull> >  block_access_list;
map<ull, ull >          block_access_idx;


int main(int argc, char const *argv[])
{

	const string file_path(argv[1]);
	ifstream input_file(file_path);

	int NUM_CORES         = 8;
	int associativity_llc = 16;
	int num_sets_llc      = 4096;


	unsigned int tid;
	ull block_addr;
	int category;
	ull count=0;

    /*****************************************
		Belady Preprocessing
	******************************************/

	map<ull, vector<ull> >::iterator ir;
	
	while(input_file >> tid >> block_addr >> category){
		ir = block_access_list.find(block_addr);
		if(ir == block_access_list.end()){
			vector<ull> newList;
			newList.pb(count);
			block_access_list.insert(mp(block_addr, newList ));
		}
		else{
			ir->second.pb(count);
		}
      count++;
   }
   input_file.close();
   cout << "Done reading file: " << file_path << endl;

   string hitfile(argv[2]);
   string sharefile(argv[3]);
   string reusefile(argv[4]);

   LLC* llc = new LLC(num_sets_llc, NUM_CORES, BELADY, hitfile, sharefile, reusefile);
   input_file.open(file_path);
   while(input_file >> tid >> block_addr >> category){
		llc->find_in_cache(block_addr, tid, category);
   }
   input_file.close();
   delete llc;

   cout << "Done simulating file: " << file_path << endl;

	return 0;
}