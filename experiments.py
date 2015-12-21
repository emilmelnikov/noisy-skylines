#!/usr/bin/env python3


'''Run experiments.'''


import csv
import itertools
import statistics
import subprocess


CORRELATION_TYPES = 'independent', 'correlated', 'anticorrelated'

DATASET_FILENAME = 'dataset.csv'
SKYLINE_FILENAME = 'skyline.csv'
RESULTS_FILENAME = 'results.csv'

NRUNS = 100

CONFIG = {
    '': {
        'algorithm': ('nestedloops', 'noisless', 'noisy'),
        'correlation': ('independent', 'correlated'),
        'cardinality': (1000, 10000, 100000, 1000000),
        'dimensionality': (1, 2, 4, 8, 16),
        'tolerance': (0.1, 0.2, 0.3, 0.4),
        'error_probability': (0.001, 0.01, 0.1),
    }
}


def main():
    with open(RESULTS_FILENAME, 'w', newline='') as csvfile:
        csvwriter = csv.writer(csvfile)
        csvwriter.writerow((
                'algorithm',
                'type',
                'cardinality',
                'dimensionality',
                'tolerance',
                'error_probability',
                'running_time',
                'comparson_count',
        ))

        for algorithm in CONFIG:
            for datatype in CONFIG[algorithm]:
                params = CONFIG[algorithm][datatype]:
                variables = 'cardinality', 'dimensionality'
                if algorithm == 'noisy':
                    variables += 'tolerance', 'error_probability'
                params_instances = itertools.product(params[v] for v in variables)



def run_skyline(algorithm, exec_path):
    args_string = ', '.join('{}={}'.format(name, value) for name, value in {**generator_kwargs, **alg_kwargs}.items())
    log_string = '{} ({})'.format(algorithm, args_string)

    run_args = exec_path, DATASET_FILENAME, SKYLINE_FILENAME, *map(str, alg_kwargs.values())
    times, counts = [], []

    for run in range(1, runs+1):
        generate_dataset(**generator_kwargs)
        print('[{}/{}]'.format(run, runs), log_string)
        run_result = subprocess.run(run_args, check=True, stdout=subprocess.PIPE)
        time, count = map(int, run_result.stdout.split())
        times.append(time)
        counts.append(count)

    return statistics.mean(times), statistics.stdev(times), statistics.mean(counts), statistics.stdev(counts)


def generate_dataset(corr, n, d):
    corr_switch = '-' + corr[0]
    args = './randdataset', corr_switch, '-n', str(n), '-d', str(d)
    with open(DATASET_FILENAME, 'w') as f:
        subprocess.run(args, check=True, stdout=f)


def main():
    with open(RESULTS_FILENAME, 'w', newline='') as csvfile:
        csvwriter = csv.writer(csvfile)
        csvwriter.writerow((
                'algorithm', 'type', 'cardinality', 'dimensionality',
                'running_time_mean', 'running_time_variance', 'comparson_count_mean', 'comparison_count_variance',
                'tolerance', 'error_probability',
        ))

        if args.nestedloops or args.all:
            for corr in CORRELATION_TYPES:
                for n in ns:
                    for d in ds:
                        if corr != 'independent' and d == 1:
                            continue
                        stats = run_skyline(
                                args.runs, 'nestedloops', './nl_skyline',
                                {'corr': corr, 'n': n, 'd': d},
                                {},
                        )
                        csvwriter.writerow(('nestedloops', corr, n, d) + stats + (0, 0))

        if args.outputsensitive or args.all:
            for corr in CORRELATION_TYPES:
                for n in ns:
                    for d in ds:
                        if corr != 'independent' and d == 1:
                            continue
                        stats = run_skyline(
                                args.runs, 'noisless', './noisless_skyline',
                                {'corr': corr, 'n': n, 'd': d},
                                {},
                        )
                        csvwriter.writerow(('noisless', corr, n, d) + stats + (0, 0))

        if args.noisy or args.all:
            for corr in CORRELATION_TYPES:
                for n in ns:
                    for d in ds:
                        if corr != 'independent' and d == 1:
                            continue
                        for t in ts:
                            for p in ps:
                                stats = run_skyline(
                                        args.runs, 'noisy', './noisy_skyline',
                                        {'corr': corr, 'n': n, 'd': d},
                                        {'t': t, 'p': p},
                                )
                                csvwriter.writerow(('noisy', corr, n, d) + stats + (t, p))


if __name__ == '__main__':
    main()
