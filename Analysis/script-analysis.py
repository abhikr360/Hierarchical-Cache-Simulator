import numpy as np
import argparse
import os


parser = argparse.ArgumentParser(description='Script for analysis')
parser.add_argument('--size', type =str, default='test', help='size of trace')

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




for p in progs:
	command = ''
	# command += 'python analysis1.py --p {} --size {} & '.format(p, args.size)

	command += 'python analysis2.py --p {} --size {} --property {} & '.format(p, args.size, 'mean')
	command += 'python analysis2.py --p {} --size {} --property {} & '.format(p, args.size, 'median')

	# command += 'python analysis3.py --p {} --size {} --property {} & '.format(p, args.size, 'median')
	# command += 'python analysis3.py --p {} --size {} --property {} & '.format(p, args.size, 'median')

	command += 'wait'
	os.system(command)
	print("Plots generated for {}, {}".format(p, args.size))
