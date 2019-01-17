#include "constants.h"
#include "L2Cache.h"
#include "LLC.h"

LLC::LLC(int associativity, int num_sets){
		this->associativity = associativity;
		this->num_sets = num_sets;
		this->clock=1;
		data.resize(num_sets, vector<ull>(associativity, INVALID));
	}

void
LLC::set_parent(L2Cache* parent){
	this->parent=parent;
}

void
LLC::find_in_cache(ull addr){
	
	ull idx = addr%num_sets;
	for(int j=0;j<associativity;++j){
		if(data[idx][j]==addr){
			//Hit



			return;
		}
	}
	//Miss
}
