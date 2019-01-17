class CE_Belady
{
public:
	ull addr;
	vector< pair<ull, unsigned int> > access_list;
	ull cur_idx;
	unsigned int cur_owner;
	vector<int> sharers;

	CE_Belady(ull addr, unsigned int cur_owner, ull cur_use);
	~CE_Belady();

	void add_sharer(int thread_id);
};
