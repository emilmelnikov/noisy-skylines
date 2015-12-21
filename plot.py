#!/usr/bin/env python3


'''Plot all items from the dataset and highlight the skyline.'''


import argparse

import numpy as np
import matplotlib.pyplot as plt


ap = argparse.ArgumentParser(description=__doc__)
ap.add_argument(
    'dataset',
    type=str,
    help='name of the file with the dataset',
)
ap.add_argument(
    'skyline',
    type=str,
    help='name of the file with the skyline indices',
)
args = ap.parse_args()

dataset = np.fromfile(args.dataset, dtype=np.float64)
dataset = dataset.reshape((len(dataset) / 2, 2))
skyline = np.loadtxt(args.skyline, dtype=int, delimiter=',')

if len(skyline):
    format_string = '{!s:>15} {!s:>15} {!s:>20} {!s:>20}'
    ruler_len = 15 + 1 + 15 + 1 + 20 + 1 + 20
    print('=' * ruler_len)
    print(format_string.format('#', 'item_index', 'x', 'y'))
    print('-' * ruler_len)
    for index, item_index in enumerate(skyline, 1):
        print(format_string.format(index, item_index, dataset[item_index,0], dataset[item_index,1]))
    print('=' * ruler_len)

if len(dataset):
    plt.scatter(dataset[:,0], dataset[:,1], color='blue', marker='.')

if len(skyline):
    plt.scatter(dataset[skyline,0], dataset[skyline,1], color='red', marker='x')

plt.show()
