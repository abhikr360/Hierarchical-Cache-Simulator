
#ifndef L2Cache_included
#define L2Cache_included


class L1Cache;

class L2Cache
{
public:
	int associativity;
	int num_sets;
	int type;
	int id;
	ull clock;
	L1Cache* parentData;
	L1Cache* parentInstruction;

	vector< vector<ll> > data;
	replacement_policy policy;
	unordered_map<ull, long long> last_use; // Will be useful only if the policy is LRU



	L2Cache(int num_sets, int associativity, int type, int id, replacement_policy policy);
	~L2Cache();


	void find_in_cache(ull addr, int category, ull pc);
	void set_parent(L1Cache* parentData, L1Cache* parentInstruction);
};

#endif