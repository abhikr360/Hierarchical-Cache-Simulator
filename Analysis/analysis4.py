import pandas as pd
import numpy as np
import argparse
import os
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(description='Plot for Reuse distance')
parser.add_argument('--size', type =str, default='test', help='size of trace')
parser.add_argument('--p', type =str, default='bodytrack', help='program')

args = parser.parse_args()

data_folder = '../../result/{}/'.format(args.size)
plot_folder = '../../result/plots/{}/'.format(args.size)
data_file = data_folder + 'LLCtrace_' + args.p + '_reusefile.out'

data = pd.read_csv(data_file, sep=' ')

try:
	n_sharers = int(data['n_sharers'].max())
except:
	n_sharers = 1
	print("analysis3", args.size, args.p)
