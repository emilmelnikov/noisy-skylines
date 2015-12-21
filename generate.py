#!/usr/bin/env python3


'''Generate random dataset for skyline algorithms.'''


import argparse

import numpy as np


ap = argparse.ArgumentParser(description=__doc__)
ap.add_argument(
    'nitems',
    type=int,
    help='number of items to generate',
)
ap.add_argument(
    'dimensions',
    type=int,
    help='number of dimensions in each generated item',
)
ap.add_argument(
    '-f',
    '--file',
    type=str,
    help='name of output file; default is "dataset.bin"',
    default='dataset.bin'
)
ap_corr_group = ap.add_mutually_exclusive_group(required=True)
ap_corr_group.add_argument(
    '-i',
    '--independent',
    action='store_true',
    help='generate independent dataset',
)
ap_corr_group.add_argument(
    '-c',
    '--correlated',
    action='store_true',
    help='generate correlated dataset',
)
ap_corr_group.add_argument(
    '-a',
    '--anticorrelated',
    action='store_true',
    help='generate anticorrelated dataset',
)
args = ap.parse_args()

if args.independent:
    dataset = np.random.rand(args.nitems, args.dimensions)
    dataset = dataset.astype(np.float64)
    dataset.tofile(args.file)
