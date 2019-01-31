/*****************************************
		Belady Preprocessing
	*****************************************/

	/*
	string folder = "../output_trace/test/";
	string file_path = folder + "addrtrace_bodytrack.out" ; //to_string(argv[1]);

	FILE *fp_in;

	fp_in = fopen(file_path.c_str(), "r");
	assert(fp_in != NULL);

	unsigned int tid;
	ull block_addr;
	ull count=0;
	map<ull, CE_Belady*>::iterator ir;
	while(!feof(fp_in)){
		fscanf(fp_in, "%d %llu", &tid, &block_addr);
		ir = block_data.find(block_addr);
		if(ir ==block_data.end()){
			block_data.insert(mp(block_addr, new CE_Belady(block_addr, tid, count)));
		}

		else{
			if(ir->second->cur_owner != tid){
				ir->second->add_sharer(tid);
			}
			ir->second->access_list.pb(mp(count, tid));
		}
      count++;
   }
   fclose(fp_in);
   printf("Done reading file!\n");
	*/