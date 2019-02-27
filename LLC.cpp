#include "constants.h"
#include "LLC.h"

LLC::LLC(int associativity, int num_sets, replacement_policy policy, string hitfile, string sharefile, string reusefile){
	this->associativity = associativity;
	this->num_sets = num_sets;
	this->policy = policy;
	this->clock= new ull[num_sets];
	this->data.resize(num_sets, vector<CE_Belady>(this->associativity));
	for(int i=0;i<this->num_sets;++i){
		for(int j=0;j<this->associativity;++j){
			this->data[i][j].addr=INVALID;
		}
		this->clock[i]=0;
	}
	this->hitinfo.open(hitfile);
	this->shareinfo.open(sharefile);
	this->reuseinfo.open(reusefile);

	this->hitbuffer =  "addr n_sharers n_hits category\n";
	this->sharebuffer = "addr n_sharers sharers_idx distance category\n";
	this->resuebuffer = "addr n_sharers hit_idx distance category\n";

	this->write_time=0;
	this->add_time=0;
	this->hit_time=0;
	this->invalid_time=0;
	this->init_time=0;
	this->replace_time=0;
}


void
LLC::find_in_cache(ull addr, int tid, int category){
	// assert(block_access_list[addr][block_access_idx[addr]] == this->clock);
	clock_t temp_time = std::clock();
	ull idx = addr%num_sets;
	this->clock[idx]++;
	int is_hit=0;
	for(int j=0;j<associativity;++j){
		if(data[idx][j].addr==addr){ // Hit
			// cout <<  "LLCHit" << addr << endl;
			this->data[idx][j].num_hits++;
			this->data[idx][j].access_list.pb(this->clock[idx]);
			block_access_idx[addr]++;
			if(this->data[idx][j].cur_owner != tid){
				this->data[idx][j].add_sharer(tid, this->clock[idx]);
			}
			this->data[idx][j].cur_owner = tid;
			switch(this->policy){
				case BELADY:
					
					break;
				case LRU:
					this->last_use[addr]=this->clock[idx];
					break;
			}
			is_hit=1;
			break;
		}
	}
	this->hit_time += double(std::clock() - temp_time )/CLOCKS_PER_SEC;
	if(is_hit){
		return;
	}


	// Recording  a Miss
	temp_time = std::clock();
	// If cache has space left
	for(int j=0;j<this->associativity;++j){
		if(this->data[idx][j].addr==INVALID){
			// cout <<  "LLCMiss " << addr << " Found way " << j << endl;
			this->data[idx][j].initialize(addr, tid, this->clock[idx], category);
			switch(this->policy){
				case BELADY:
					break;
				case LRU:
					this->last_use[addr]=this->clock[idx];
					break;
			}
			is_hit = 1;
			break;
		}
	}

	this->invalid_time += double(std::clock() - temp_time )/CLOCKS_PER_SEC;
	if(is_hit){
		return;
	}

	//Need to invoke replacement policy
	temp_time = std::clock();
	int evict_way;
	long long next_use = 0;
	switch(this->policy){
		case BELADY:
			for(int j=0;j<associativity;++j){
				ull temp_addr = this->data[idx][j].addr;
				ull idx = block_access_idx[temp_addr];
				if( idx >= block_access_list[temp_addr].size() ){
					next_use = LLONG_MAX;
					evict_way = j;
					break;
				}
				ull temp_next_use = block_access_list[temp_addr][idx];
				if( temp_next_use > next_use){
					next_use = temp_next_use;
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
			this->last_use[addr]=clock[idx];
			break;	
	}


	this->replace_time += double(std::clock() - temp_time )/CLOCKS_PER_SEC;
	this->recordInfo(this->data[idx][evict_way]);

	temp_time = std::clock();
	this->data[idx][evict_way].initialize(addr, tid, this->clock[idx], category);

	this->init_time += double(std::clock() - temp_time )/CLOCKS_PER_SEC;
}

void
LLC::recordInfo(CE_Belady & ce){
	// cout << "Evicting " << ce.addr << endl;
	clock_t temp_time = std::clock();
	if(this->hitbuffer.size()>MAX_BUF_SIZE){
		this->hitinfo << this->hitbuffer;
		this->hitbuffer = "";
	}
	if(this->sharebuffer.size() > MAX_BUF_SIZE){
		this->shareinfo << this->sharebuffer;
		this->sharebuffer = "";
	}

	if(this->resuebuffer.size() > MAX_BUF_SIZE){
		this->reuseinfo << this->resuebuffer;
		this->resuebuffer = "";
	}

	this->write_time += double(std::clock() -  temp_time)/CLOCKS_PER_SEC;
	

	temp_time = std::clock();
	this->hitbuffer += to_string(ce.addr) + " " + to_string(ce.sharers.size()) + " " + to_string(ce.num_hits) +  " " + to_string(ce.category) +'\n';

	ull prev = ce.sharers[0].second;
	for(int i=1;i<ce.sharers.size();++i){
		this->sharebuffer += to_string(ce.addr) + " " + to_string(ce.sharers.size()) + " " + to_string(i) + " " + to_string(ce.sharers[i].second - prev) + " " + to_string(ce.category) +'\n';
		prev = ce.sharers[i].second;
	}

	prev = ce.access_list[0];
	for(int i=1;i<ce.access_list.size();++i){
		this->resuebuffer += to_string(ce.addr) + " " + to_string(ce.sharers.size()) + " " + to_string(i) + " " + to_string(ce.access_list[i] - prev) + " " + to_string(ce.category) + '\n';
		prev = ce.access_list[i];
	}
	this->add_time += double(std::clock() - temp_time)/CLOCKS_PER_SEC;

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
	if(this->hitbuffer.size()){
		this->hitinfo << this->hitbuffer;
		this->hitbuffer="";
	}
	if(this->sharebuffer.size()){
		this->shareinfo << this->sharebuffer;
		this->sharebuffer = "";
	}
	if(this->resuebuffer.size()){
		this->reuseinfo << this->resuebuffer;
		this->resuebuffer = "";
	}

	this->hitinfo.close();
	this->shareinfo.close();
	this->reuseinfo.close();
}
