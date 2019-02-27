import os
import argparse

parser = argparse.ArgumentParser(description='driver script for run.py')
parser.add_argument('--size',  type=str, default='test',
                    help='size of the benchmark suite')
args = parser.parse_args()

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

# progs = [ 'canneal', 'vips']


trace_folder = '../output_trace/{}/'.format(args.size)

# compile
os.system("g++ -std=c++11 simulate.cpp L1Cache.cpp L2Cache.cpp -o simulate")


command =''
for p in progs:
	command += './simulate {}addrtrace_{}.out > {}LLCtrace_{}.out & '.format(trace_folder, p, trace_folder, p)

command += 'wait'
os.system(command)
