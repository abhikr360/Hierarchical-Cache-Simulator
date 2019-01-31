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

# progs = [ 'canneal']


trace_folder = '../output_trace/test/'
# compile
os.system("g++ -std=c++11 llc_simulation.cpp LLC.cpp CE_Belady.cpp -o llc")

result_folder = '../result/test/'

try:
	os.mkdir(result_folder)
except:
	pass


command =''
for p in progs:
	command += './llc {}LLCtrace_{}.out {}LLCtrace_{}_hitfile.out {}LLCtrace_{}_sharefile.out {}LLCtrace_{}_reusefile.out & '.format(trace_folder, p,
																										result_folder, p, 
																										result_folder, p, 
																										result_folder, p)

command += 'wait'
os.system(command)