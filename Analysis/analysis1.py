import pandas as pd
import numpy as np
import argparse
import os
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(description='Plot for Hit frequency')
parser.add_argument('--size', type =str, default='test', help='size of trace')
parser.add_argument('--p', type =str, default='facesim', help='program')

args = parser.parse_args()

plot_folder = '../../result/plots/{}/'.format(args.size)
data_folder = '../../result/{}/'.format(args.size)
data_file = data_folder + 'LLCtrace_' + args.p + '_hitfile.out'

data = pd.read_csv(data_file, sep=' ')

# print(data.head())
# print(data.groupby(['n_sharers', 'category']).count())
# print(data.loc[data['n_sharers'] == 2])
# print(data.loc[data['category'] == 2].count())


n_sharers = data['n_sharers'].max()

rec0 = []
rec1 = []
rec2 = []

for i in range(1, n_sharers+1):
	t = data.loc[data['n_sharers'] == i]
	rec0 += list(t.loc[t['category']==0, ['n_hits']].count())
	rec1 += list(t.loc[t['category']==1, ['n_hits']].count())
	rec2 += list(t.loc[t['category']==2, ['n_hits']].count())

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
plt.legend()
plt.savefig(plot_folder + 'hit_{}_{}.png'.format(args.size, args.p))
plt.tight_layout()
# plt.show()
