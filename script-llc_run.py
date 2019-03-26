import os
import argparse

parser = argparse.ArgumentParser(description='driver script for run.py')
parser.add_argument('--size',  type=str, default='test',
                    help='size of the benchmark suite')
args = parser.parse_args()

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

progs = [ 'bodytrack']


trace_folder = '../output_trace/{}/'.format(args.size)
# compile
os.system("g++ -std=c++11 llc_simulation.cpp LLC.cpp CE_Belady.cpp -o llc_simulation")

result_folder = '../result/{}/'.format(args.size)

try:
	os.mkdir(result_folder)
except:
	pass


command =''
for p in progs:
	command += 'time ./llc_simulation {}LLCtrace_{}.out {}LLCtrace_{}_hitfile.out {}LLCtrace_{}_sharefile.out {}LLCtrace_{}_reusefile.out & '.format(trace_folder, p,
																										result_folder, p, 
																										result_folder, p, 
																										result_folder, p)

command += 'wait'
os.system(command)