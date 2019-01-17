#include "LLC.h"

#ifndef L2Cache_included
#define L2Cache_included



class L1Cache;

class L2Cache
{
public:
	int associativity;
	int num_sets;
	ull clock;
	L1Cache* parent;
	LLC* child;
	vector< vector<ll> > data;
	replacement_policy policy;
	unordered_map<ull, long long> last_use; // WIll be useful only if the policy is LRU



	L2Cache(int num_sets, int associativity, replacement_policy policy);
	~L2Cache();

	void set_parent(L1Cache* parent);
	void set_child(LLC* child);

	void find_in_cache(ull addr);
	void invalidate(ull addr);

};

#endif