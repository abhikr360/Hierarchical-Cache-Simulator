#include "constants.h"
#include "L1Cache.h"
#include "L2Cache.h"


L1Cache::L1Cache(int num_sets, int associativity, int type, int id, replacement_policy policy){
	this->associativity = associativity;
	this->num_sets = num_sets;
	this->type = type;
	this->id = id;
	this->clock=1;
	this->policy = policy;
	this->data.resize(num_sets, vector<ll>(associativity, INVALID));
}

void
L1Cache::set_child(L2Cache* child){
	this->child = child;
}

void
L1Cache::find_in_cache(ull addr, int category){
	this->clock++;
	ull idx = addr%num_sets;
	for(int j=0;j<this->associativity;++j){
		if(this->data[idx][j]==addr){ //Hit
			switch(this->policy){
				case BELADY:
					
					break;
				case LRU:
					this->last_use[addr]=this->clock;
					// cout << "L1 Hit " << addr << endl;
					break;
			}
			return;
		}
	}
	//Miss
	this->child->find_in_cache(addr, category);

	// If cache has space left
	for(int j=0;j<this->associativity;++j){
		if(this->data[idx][j]==INVALID){
			this->data[idx][j]=addr;
			switch(this->policy){
				case BELADY:
					
					break;
				case LRU:
					this->last_use[addr]=this->clock;
					// cout << "L1 Miss " << addr << endl;
					break;
			}
			return;
		}
	}

	// Need to invoke replacement policy

	switch(this->policy){
		case BELADY:

			break;
		case LRU:
			int min_way;
			long long min_use = LLONG_MAX;
			for(int j=0;j<associativity;++j){
				if(this->last_use[this->data[idx][j]] < min_use){
					min_use = this->last_use[this->data[idx][j]];
					min_way = j;
				}
			}
			this->last_use.erase(this->data[idx][min_way]);
			this->last_use[addr]=clock;
			// cout << "L1 Miss " << addr << " Replacing " << this->data[idx][min_way] << endl;
			this->data[idx][min_way]=addr;
			break;
	}
}

void
L1Cache::invalidate(ull addr){
	ull idx = addr%num_sets;
	// cout << "L1 invalidate " << addr << endl;
	for(int j=0;j<this->associativity;++j){
		if(this->data[idx][j]==addr){
			last_use.erase(addr);
			data[idx][j] = INVALID;
			return;
		}
	}
}

L1Cache::~L1Cache(){
	
}