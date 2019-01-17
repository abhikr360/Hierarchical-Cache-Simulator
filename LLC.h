#ifndef LLC_included
#define LLC_included



class L2Cache;

class LLC
{
public:
	int associativity;
	int num_sets;
	ull clock;
	L2Cache* parent;
	vector< vector<ull> > data;

	LLC(int associativity, int num_sets);
	~LLC();

	void set_parent(L2Cache* parent);

	void find_in_cache(ull addr);
};

#endif