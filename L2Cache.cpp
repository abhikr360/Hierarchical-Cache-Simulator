#include "constants.h"
#include "L1Cache.h"
#include "L2Cache.h"
#include "LLC.h"



L2Cache::L2Cache(int num_sets, int associativity, replacement_policy policy){
	this->associativity = associativity;
	this->num_sets = num_sets;
	this->clock=1;
	this->policy = policy;
	this->data.resize(num_sets, vector<ll>(associativity, INVALID));
}



void
L2Cache::set_parent(L1Cache* parent){
	this->parent=parent;
}
void
L2Cache::set_child(LLC* child){
	this->child=child;
}


void
L2Cache::find_in_cache(ull addr){
	this->clock++;
	ull idx = addr%num_sets;
	for(int j=0;j<associativity;++j){
		if(this->data[idx][j]==addr){ //Hit
			switch(this->policy){
				case BELADY:
					
					break;
				case LRU:
					this->last_use[addr]=this->clock;
					cout << "L2 Hit " << addr << endl;
					break;
			}
			return;
		}
	}
	//Miss

	// this->child->find_in_cache(addr);
	
	// If cache has space left
	for(int j=0;j<this->associativity;++j){
		if(this->data[idx][j]==INVALID){
			this->data[idx][j]=addr;
			switch(this->policy){
				case BELADY:
					
					break;
				case LRU:
					this->last_use[addr]=this->clock;
					cout << "L2 Miss " << addr << endl;
					break;
			}
			return;
		}
	}

	// Need to invoke replacement policy
	int evict_way;
	switch(this->policy){
		case BELADY:

			break;
		case LRU:
			long long min_use = LLONG_MAX;
			for(int j=0;j<associativity;++j){
				if(this->last_use[this->data[idx][j]] < min_use){
					min_use = this->last_use[this->data[idx][j]];
					evict_way = j;
				}
			}
			this->last_use.erase(this->data[idx][evict_way]);
			this->last_use[addr]=clock;
			cout << "L2 Miss " << addr << " Replacing " << this->data[idx][evict_way] << endl;
			break;
	}
	this->parent->invalidate(this->data[idx][evict_way]);
	this->data[idx][evict_way]=addr;
}


void
L2Cache::invalidate(ull addr){
	ull idx = addr%num_sets;
	cout << "L2 invalidate " << addr << endl;
	for(int j=0;j<this->associativity;++j){
		if(this->data[idx][j]==addr){
			last_use.erase(addr);
			data[idx][j] = INVALID;
			return;
		}
	}
}