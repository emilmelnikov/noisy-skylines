#!/usr/bin/env python3.5


'''Generate random dataset for skyline algorithms.'''


import argparse

import numpy as np


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument(
        '-n',
        '--nitems',
        type=int,
        help='number of items to generate; default is 100',
        default=100,
    )
    ap.add_argument(
        '-d',
        '--dimensions',
        type=int,
        help='number of dimensions for each item; default is 2',
        default=2,
    )
    ap.add_argument(
        '-o',
        '--output',
        type=str,
        help='name of the output file; default is "dataset.csv"',
        default='dataset.csv',
    )
    ap.add_argument(
        '-f',
        '--from',
        type=int,
        help='lower bound of the value range; default is 0',
        default=0,
    )
    ap.add_argument(
        '-t',
        '--to',
        type=int,
        help='upper bound of the value range; default is 99',
        default=99,
    )
    ap.add_argument(
        '-c',
        '--correlation',
        type=float,
        help='correlation coefficient for the data points; default is 0.0',
        default=0.0,
    )
    args = ap.parse_args()
    # Since 'from' is a Python keyword, get an argument with some trickery.
    data = np.random.randint(
        vars(args)['from'],
        args.to,
        size=(args.nitems, args.dimensions),
    )
    np.savetxt(args.output, data, delimiter='\t', fmt='%d')


if __name__ == '__main__':
    main()
