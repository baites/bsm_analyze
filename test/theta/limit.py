#!/usr/bin/env python3.2
#
# Read Theta limits from text files and convert these to LaTeX format
#
# Created by Samvel Khalatyan, Dec 21, 2011
# Copyright 2011, All rights reserved

import re

def getLimits(filename):
    limits = {}
    with open(filename, 'r') as input:
        for line in input:
            if line.startswith('#'):
                continue

            # Line convention
            # Observed: mass, central
            # Expected: mass, central, low_two_sigma, high_two_sigma, low_one_sigma, high_one_sigma
            #
            mass, *values = [float(x) for x in re.split("\s+", line.strip())]

            limits[int(mass)] = values

    return limits

if "__main__" == __name__:
    expected_limits = getLimits("exp_limit.txt")
    print("Expected Limits")
    for k,v in expected_limits.items():
        print("{0}: {1}".format(k, ' '.join(map(str, v))))
    print("")

    observed_limits = getLimits("obs_limit.txt")
    print("Observed Limits")
    for k,v in observed_limits.items():
        print("{0}: {1}".format(k, v[0]))
    print("")

    print("\\hline")
    print("Process & Expected [pb] & Central 1$\\sigma$ & Central 2$\\sigma$ & Observed [pb] \\\\")
    print("\\hline")
    print("\\hline")
    for key in sorted(expected_limits.keys()):
        if not key in observed_limits:
            print("key {} is not present in the observed limits".format(key))
            continue

        expected = expected_limits[key]
        observed = observed_limits[key]

        print("$\\Zprime$ ($M = {0}~\TeVcc$) & {1:.2f} & {2} & {3} & {4:.2f} \\\\".format(
            key // 1000 if 0 == (key % 1000) else key / 1000,
            expected[0],
            '-'.join(["{0:.2f}".format(x) for x in expected[-2:]]),
            '-'.join(["{0:.2f}".format(x) for x in expected[-4:-2]]),
            observed[0]))
    print("\\hline")

    print()
    print("| Process | Expected [pb] | Central 1sigma | Central 2sigma | Observed [pb] |")
    for key in sorted(expected_limits.keys()):
        if not key in observed_limits:
            print("key {} is not present in the observed limits".format(key))
            continue

        expected = expected_limits[key]
        observed = observed_limits[key]

        print("| Zprime (M = {0}~\TeVcc) | {1:.2f} | {2} | {3} | {4:.2f} |".format(
            key // 1000 if 0 == (key % 1000) else key / 1000,
            expected[0],
            '-'.join(["{0:.2f}".format(x) for x in expected[-2:]]),
            '-'.join(["{0:.2f}".format(x) for x in expected[-4:-2]]),
            observed[0]))

    print()
    format_string = "{process:<15} {expected:<20} {observed:<20}"
    print(format_string.format(process = "Process",
                               expected = "Expected [pb]",
                               observed = "Observed [pb]"))

    for key in sorted(expected_limits.keys()):
        if not key in observed_limits:
            print("key {} is not present in the observed limits".format(key))
            continue

        expected = expected_limits[key]
        observed = observed_limits[key]

        values = dict(process = "Z' {0} TeV".format(key // 1000 if 0 == (key % 1000) else key / 1000),
                expected = "{0:.2f}".format(expected[0]),
                observed = "{0:.2f}".format(observed[0]))

        print(format_string.format(**values))
