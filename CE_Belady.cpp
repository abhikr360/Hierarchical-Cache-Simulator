#include "CE_Belady.h"

CE_Belady::CE_Belady(ull addr, unsigned int cur_owner, ull cur_use){
	this->addr = addr;
	this->cur_owner = cur_owner;
	this->cur_idx = 0;
	access_list.pb(mp(cur_use, cur_owner));
}


CE_Belady::~CE_Belady();

void
CE_Belady::add_sharer(int thread_id){
	if(find(sharers.begin(), sharers.end(), thread_id)==sharers.end()){
		sharers.pb(thread_id);
	}
}
