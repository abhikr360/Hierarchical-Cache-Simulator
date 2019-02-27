#include "constants.h"
#include "LLC.h"



unordered_map<ull, vector<ull> >  block_access_list;
unordered_map<ull, ull >          block_access_idx;

int NUM_CORES         = 8;
int main(int argc, char const *argv[])
{

	const string file_path(argv[1]);
	ifstream input_file(file_path);

	int associativity_llc = 16;
	int num_sets_llc      = 4096;


	unsigned int tid;
	ull block_addr;
	int category;
	ull count=0;

    /*****************************************
		Belady Preprocessing
	******************************************/

	unordered_map<ull, vector<ull> >::iterator ir;
	
	while(input_file >> tid >> block_addr >> category){
		ir = block_access_list.find(block_addr);
		if(ir == block_access_list.end()){
			vector<ull> newList;
			newList.pb(count);
			block_access_list.insert(mp(block_addr, newList ));
			block_access_idx.insert(mp(block_addr, 0));
		}
		else{
			ir->second.pb(count);
		}
      count++;
   }
   input_file.close();
   cout << "Done reading file: " << file_path << endl;

   cout << block_access_list.size() << endl;
   // return 0;
   string hitfile(argv[2]);
   string sharefile(argv[3]);
   string reusefile(argv[4]);

   LLC* llc = new LLC(num_sets_llc, NUM_CORES, BELADY, hitfile, sharefile, reusefile);
   input_file.open(file_path);
   const clock_t total_time = std::clock();
   while(input_file >> tid >> block_addr >> category){
		
		llc->find_in_cache(block_addr, tid, category);

   }
	std::cout << "Total CPU time " << double( std::clock () - total_time ) /  CLOCKS_PER_SEC << endl;
	std::cout << "Total add time " << llc->add_time << endl;
	std::cout << "Total write time " << llc->write_time << endl;
	std::cout << "Total hit time " << llc->hit_time << endl;
	std::cout << "Total invalid time " << llc->invalid_time << endl;
	std::cout << "Total init time " << llc->init_time << endl;
	std::cout << "Total replace time " << llc->replace_time << endl;
   input_file.close();
   delete llc;

   cout << "Done simulating file: " << file_path << endl;

	return 0;
}