import numpy as np
import argparse
import os


parser = argparse.ArgumentParser(description='Script for analysis')
parser.add_argument('--size', type =str, default='simlarge', help='size of trace')

args = parser.parse_args()

if(args.size == 'simlarge'):
	progs = [
	# 'blackscholes',
	# 'bodytrack',
	'canneal',
	# 'facesim',
	'ferret',
	'fluidanimate',
	'freqmine',
	# 'raytrace',
	'streamcluster',
	'vips',
	# 'dedup',
	# 'swaptions',
	'x264',
	]
else:
	progs = [
	# 'blackscholes',
	'bodytrack',
	'canneal',
	'facesim',
	'ferret',
	'fluidanimate',
	'freqmine',
	# 'raytrace',
	'streamcluster',
	'vips',
	# 'dedup',
	# 'swaptions',
	'x264',
	]

# progs = ['ferret']
page_size_list = [12,15,17,20,23,25]
n_counters_list = [7,12,17,22,27]
D_list = [500,1000,2500,5000,10000]



os.system('gcc -g llc-prob.c -o llc-prob')
os.system('gcc -g llc-prob-share.c -o llc-prob-share')

for p in progs:
	try:
		os.mkdir('trash/{}/{}/'.format(args.size, p))
	except:
		pass
	for page_size in page_size_list:
		for n_counters in n_counters_list:
			command = ''
			for D in D_list:
				command += './llc-prob ../output_trace/{}/LLCtrace_{}.out {} {} {} > trash/{}/{}/{}_{}_{} & '.\
							format(args.size, p, page_size, n_counters, D, args.size, p, page_size, n_counters, D)

				command += './llc-prob-share ../output_trace/{}/LLCtrace_{}.out {} {} {} > trash/{}/{}/share_{}_{}_{} & '.\
							format(args.size, p, page_size, n_counters, D, args.size, p, page_size, n_counters, D)

			command += 'wait'
			os.system(command)
