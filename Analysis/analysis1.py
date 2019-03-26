import pandas as pd
import numpy as np
import argparse
import os
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(description='Plot for Hit frequency')
parser.add_argument('--size', type =str, default='test', help='size of trace')
parser.add_argument('--p', type =str, default='facesim', help='program')
parser.add_argument('--property', type =str, default='mean', help='choose - mean or median')
args = parser.parse_args()

plot_folder = '../../result/plots/{}/'.format(args.size)
data_folder = '../../result/{}/'.format(args.size)
data_file = data_folder + 'LLCtrace_' + args.p + '_hitfile.out'

data = pd.read_csv(data_file, sep=' ')

# print(data.head())
print(data.groupby(['n_hits', 'category']).count())
exit()
# print(data.loc[data['n_sharers'] == 2])
# print(data.loc[data['category'] == 2].count())

try:
	n_sharers = int(data['n_sharers'].max())
except:
	n_sharers = 1
	print("analysis1", args.size, args.p)

rec0 = []
rec1 = []
rec2 = []

for i in range(1, n_sharers+1):
	t = data.loc[data['n_sharers'] == i]
	if(args.property == 'mean'):
		rec0 += list(t.loc[t['category']==0, ['n_hits']].mean())
		rec1 += list(t.loc[t['category']==1, ['n_hits']].mean())
		rec2 += list(t.loc[t['category']==2, ['n_hits']].mean())
	else:
		rec0 += list(t.loc[t['category']==0, ['n_hits']].median())
		rec1 += list(t.loc[t['category']==1, ['n_hits']].median())
		rec2 += list(t.loc[t['category']==2, ['n_hits']].median())

index = np.arange(n_sharers)
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

xticks = [str(i) for i in range(1,n_sharers+1)]
plt.xlabel('n_sharers')
plt.ylabel('n_hits')
plt.title('Number of hits per sharer')
plt.xticks(index + bar_width, xticks)
plt.legend(loc = "upper right")
plt.savefig(plot_folder + 'hit_{}_{}.png'.format(args.p, args.size))
plt.tight_layout()
# plt.show()
