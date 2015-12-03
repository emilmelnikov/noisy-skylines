#!/usr/bin/env python3


'''Plot all items from the dataset and highlight the skyline.'''


import argparse

import numpy as np
import matplotlib.pyplot as plt


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument(
        '-d',
        '--dataset',
        type=str,
        help='name of the file with the entire dataset; default is "dataset.csv"',
        default='dataset.csv',
    )
    ap.add_argument(
        '-s',
        '--skyline',
        type=str,
        help='name of the file with the skyline indices; default is "skyline.csv"',
        default='skyline.csv',
    )
    args = ap.parse_args()
    dataset = np.loadtxt(args.dataset, dtype=int, ndmin=2)
    skyline = np.loadtxt(args.skyline, dtype=int)
    if len(skyline):
        format_string = '{!s:>10} {!s:>10} {!s:>10} {!s:>10}'
        print(format_string.format('index', 'item_index', 'x', 'y'))
        print('-' * 43)
        for index, item_index in enumerate(skyline, 1):
            print(format_string.format(index, item_index, dataset[item_index,0], dataset[item_index,1]))
    if len(dataset):
        plt.scatter(dataset[:,0], dataset[:,1], color='blue', marker='.')
    if len(skyline):
        plt.scatter(dataset[skyline,0], dataset[skyline,1], color='red', marker='x')
    plt.show()


if __name__ == '__main__':
    main()
