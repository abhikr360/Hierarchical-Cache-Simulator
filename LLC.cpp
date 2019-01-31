#include "constants.h"
#include "LLC.h"

LLC::LLC(int associativity, int num_sets, replacement_policy policy, string hitfile, string sharefile, string reusefile){
	this->associativity = associativity;
	this->num_sets = num_sets;
	this->policy = policy;
	this->clock=1;
	this->data.resize(num_sets, vector<CE_Belady>(this->associativity));
	for(int i=0;i<this->num_sets;++i){
		for(int j=0;j<this->associativity;++j){
			this->data[i][j].addr=INVALID;
		}
	}

	this->hitinfo.open(hitfile);
	this->shareinfo.open(sharefile);
	this->reuseinfo.open(reusefile);
	this->hitinfo << "addr" << " " << "n_sharers" << " " << "n_hits" <<  " " << "category" << endl;
	this->shareinfo << "addr" << " " << "n_sharers" << " " << "sharers_idx" << " " << "distance" << " " << "category" << endl;
	this->reuseinfo << "addr" << " " << "n_sharers" << " " << "hit_idx" << " " << "distance" << " " << "category" << endl;
}


void
LLC::find_in_cache(ull addr, int tid, int category){
	this->clock++;
	ull idx = addr%num_sets;
	for(int j=0;j<associativity;++j){
		if(data[idx][j].addr==addr){ // Hit
			// cout <<  "LLCHit" << addr << endl;
			this->data[idx][j].num_hits++;
			this->data[idx][j].access_list.pb(this->clock);
			block_access_idx[addr]++;
			if(this->data[idx][j].cur_owner != tid){
				this->data[idx][j].add_sharer(tid, this->clock);
			}
			this->data[idx][j].cur_owner = tid;
			switch(this->policy){
				case BELADY:
					
					break;
				case LRU:
					this->last_use[addr]=this->clock;
					break;
			}
			return;
		}
	}

	// Recording  a Miss

	// If cache has space left
	for(int j=0;j<this->associativity;++j){
		if(this->data[idx][j].addr==INVALID){
			// cout <<  "LLCMiss " << addr << " Found way " << j << endl;
			this->data[idx][j].initialize(addr, tid, this->clock, category);
			switch(this->policy){
				case BELADY:
					break;
				case LRU:
					this->last_use[addr]=this->clock;
					break;
			}
			return;
		}
	}

	//Need to invoke replacement policy

	int evict_way;
	long long next_use = 0;
	switch(this->policy){
		case BELADY:
			for(int j=0;j<associativity;++j){
				if(block_access_idx[this->data[idx][j].addr] >= block_access_list[this->data[idx][j].addr].size()){
					next_use = LLONG_MIN;
					evict_way = j;
				}
				else if(block_access_list[this->data[idx][j].addr][block_access_idx[this->data[idx][j].addr]] > next_use){
					next_use = block_access_list[this->data[idx][j].addr][block_access_idx[this->data[idx][j].addr]];
					evict_way = j;
				}
			}
			
			break;

		case LRU:
			long long min_use = LLONG_MAX;
			for(int j=0;j<associativity;++j){
				if(this->last_use[this->data[idx][j].addr] < min_use){
					min_use = this->last_use[this->data[idx][j].addr];
					evict_way = j;
				}
			}
			this->last_use.erase(this->data[idx][evict_way].addr);
			this->last_use[addr]=clock;
			break;	
	}
	// cout <<  "LLCMiss " << addr << " Replace way " << data[idx][evict_way].addr << endl;
	this->recordInfo(this->data[idx][evict_way]);
	this->data[idx][evict_way].initialize(addr, tid, this->clock, category);


}

void
LLC::recordInfo(CE_Belady & ce){
	// cout << "Evicting " << ce.addr << endl;
	this->hitinfo << ce.addr << " " << ce.sharers.size() << " " << ce.num_hits <<  " " << ce.category << endl;

	ull prev = ce.sharers[0].second;
	for(int i=1;i<ce.sharers.size();++i){
		this->shareinfo << ce.addr << " " << ce.sharers.size() << " " << i << " " << ce.sharers[i].second - prev << " " << ce.category << endl;
		prev = ce.sharers[i].second;
	}

	prev = ce.access_list[0];
	for(int i=1;i<ce.access_list.size();++i){
		this->reuseinfo << ce.addr << " " << ce.sharers.size() << " " << i << " " << ce.access_list[i] - prev << " " << ce.category << endl;
		prev = ce.access_list[i];
	}
}

LLC::~LLC(){

	for (int i = 0; i <this->num_sets; ++i)
	{
		for(int j=0;j<this->associativity; ++j){
			if(this->data[i][j].addr != INVALID){
				this->recordInfo(this->data[i][j]);
			}
		}
	}

	this->hitinfo.close();
	this->shareinfo.close();
	this->reuseinfo.close();
}
