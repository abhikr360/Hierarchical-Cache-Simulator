#ifndef LLC_included
#define LLC_included
#include "CE_Belady.h"

extern unordered_map<ull, vector<ull> >  block_access_list;
extern unordered_map<ull, ull >          block_access_idx;

class LLC
{
public:
	int associativity;
	int num_sets;

	ull* clock;

	vector< vector<CE_Belady> > data;

	ofstream hitinfo;
	ofstream shareinfo;
	ofstream reuseinfo;

	replacement_policy policy;
	unordered_map<ull, long long> last_use; // Will be useful only if the policy is LRU

	string hitbuffer;
	string sharebuffer;
	string resuebuffer;
	double write_time;
	double add_time;
	double hit_time;
	double invalid_time;
	double init_time;
	double replace_time;
	LLC(int associativity, int num_sets, replacement_policy policy, string hitfile, string sharefile, string resuefile);
	~LLC();



	void find_in_cache(ull addr, int tid, int category);
	void recordInfo(CE_Belady & ce);
};

#endif