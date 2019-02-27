#include "constants.h"

#ifndef CE_BELADY_included
#define CE_BELADY_included

extern unordered_map<ull, vector<ull> >  block_access_list;
extern unordered_map<ull, ull >          block_access_idx;

class CE_Belady
{
public:
	ull addr;
	unsigned int cur_owner;
	vector<ull> access_list;
	vector< pair<ull, int> > sharers;
	int category;
	int num_hits;
	CE_Belady();
	~CE_Belady();

	void initialize(ull addr, unsigned int cur_owner, ull cur_use, int category);

	void add_sharer(int thread_id, int cur_use);
};

#endif
