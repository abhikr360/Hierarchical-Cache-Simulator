import os


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

# progs = [ 'canneal', 'vips']


trace_folder = '../output_trace/test/'

# compile
os.system("g++ -std=c++11 simulate.cpp L1Cache.cpp L2Cache.cpp -o simulate")


command =''
for p in progs:
	command += './simulate {}addrtrace_{}.out > {}LLCtrace_{}.out & '.format(trace_folder, p, trace_folder, p)

command += 'wait'
os.system(command)