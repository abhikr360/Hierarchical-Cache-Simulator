#include "CE_Belady.h"

CE_Belady::CE_Belady(){

}
CE_Belady::~CE_Belady(){
	
}


void
CE_Belady::initialize(ull addr, unsigned int cur_owner, ull cur_use, int category){
	this->addr = addr;
	this->cur_owner = cur_owner;
	this->access_list.clear();
	this->access_list.pb(cur_use);
	this->sharers.clear();
	this->sharers.pb(mp(cur_owner, cur_use));
	this->category = category;
	this->num_hits=0;
	block_access_idx[this->addr]++;
}



void
CE_Belady::add_sharer(int thread_id, int cur_use){
	for(int i=0;i<this->sharers.size();++i){
		if(this->sharers[i].first == thread_id){
			return;
		}
	}
	this->sharers.pb(mp(cur_owner, cur_use));
}
