import pandas as pd
import numpy as np
import argparse
import os
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(description='Plot for sharer distance')
parser.add_argument('--size', type =str, default='test', help='size of trace')
parser.add_argument('--p', type =str, default='bodytrack', help='program')
parser.add_argument('--property', type =str, default='mean', help='choose - mean,median')

args = parser.parse_args()

data_folder = '../../result/{}/'.format(args.size)
plot_folder = '../../result/plots/{}/'.format(args.size)
data_file = data_folder + 'LLCtrace_' + args.p + '_sharefile.out'

data = pd.read_csv(data_file, sep=' ')

try:
	n_sharers = int(data['n_sharers'].max())
except:
	n_sharers = 1
	print("analysis2", args.size, args.p)

for i in range(2, n_sharers+1):
	di = data.loc[data['n_sharers'] == i]
	rec0=[]
	rec1=[]
	rec2=[]
	for j in range(2,i+1):
		t = di.loc[di['sharers_idx'] ==j-1 ]
		if(args.property == 'mean'):
			rec0 += list(t.loc[t['category']==0, ['distance']].mean())
			rec1 += list(t.loc[t['category']==1, ['distance']].mean())
			rec2 += list(t.loc[t['category']==2, ['distance']].mean())
		else:
			rec0 += list(t.loc[t['category']==0, ['distance']].median())
			rec1 += list(t.loc[t['category']==1, ['distance']].median())
			rec2 += list(t.loc[t['category']==2, ['distance']].median())

	index = np.arange(i-1)
	fig, ax = plt.subplots()
	bar_width = 0.15
	opacity = 0.7
	 
	rects0 = plt.bar(index, rec0, bar_width,
	                 alpha=opacity,
	                 color='r',
	                 label='data_load')

	rects1 = plt.bar(index+bar_width, rec1, bar_width,
	                 alpha=opacity,
	                 color='g',
	                 label='data_store')

	rects2 = plt.bar(index+2*bar_width, rec2, bar_width,
	                 alpha=opacity,
	                 color='b',
	                 label='instruction')

	xticks = [str(k) for k in range(2,i+1)]
	plt.xlabel('sharer_idx')
	plt.ylabel('avg. distance to show up')
	plt.title('avg. distance to show u per sharer for n_sharers = {}'.format(i))
	plt.xticks(index + bar_width, xticks)
	plt.legend(loc='upper right')
	plt.savefig( plot_folder + 'share_distance_{}_{}_{}.png'.format(args.p, i, args.property))
	plt.tight_layout()
	# plt.show()