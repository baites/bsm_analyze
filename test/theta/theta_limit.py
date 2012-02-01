#!/usr/bin/env python3
#
# Created by Samvel Khalatyan, Dec 21, 2011
# Copyright 2011, All rights reserved

'''
Read Theta computed observed and expected limits
'''

import re

def load(filename):
    '''
    load limits from file
    '''
    limits = {}
    with open(filename, 'r') as input:
        for line in input:
            line = line.strip()
            if not line or line.startswith('#'):
                continue

            # Line convention
            #
            # Observed: mass, central
            # Expected: mass, central, low_two_sigma, high_two_sigma, low_one_sigma, high_one_sigma
            #
            mass, *values = [float(x) for x in re.split("\s+", line)]

            limits[int(mass)] = values

    return limits

def printInTextFormat(limits, masses = set()):
    '''
    print observed and expected limits in text format

    limits is a dictionary with two keys: observed, and expected
    '''
    format_string = "{process:<15} {expected:<20} {observed:<20}"
    print(format_string.format(process = "Process",
                               expected = "Expected [pb]",
                               observed = "Observed [pb]"))

    keys = limits["expected"].keys() & limits["observed"].keys()
    if masses:
        keys &= masses

    for key in sorted(keys):
        expected = limits["expected"][key]
        observed = limits["observed"][key]

        values = dict(process = "Z' {0} TeV".format(key // 1000 if 0 == (key % 1000) else key / 1000),
                expected = "{0:.2f}".format(expected[0]),
                observed = "{0:.2f}".format(observed[0]))

        print(format_string.format(**values))

def printInLatexFormat(limits, masses = set()):
    '''
    print observed and expected lmist in Latex format

    limits is a dictionary with two keys: observed, and expected
    '''
    print("\\hline")
    print("Process & Expected [pb] & Central 1$\\sigma$ & Central 2$\\sigma$ & Observed [pb] \\\\")
    print("\\hline")
    print("\\hline")

    keys = limits["expected"].keys() & limits["observed"].keys()
    if masses:
        keys &= masses

    for key in sorted(keys):
        expected = limits["expected"][key]
        observed = limits["observed"][key]

        print("$\\Zprime$ ($M = {0}~\TeVcc$) & {1:.2f} & {2} & {3} & {4:.2f} \\\\".format(
            key // 1000 if 0 == (key % 1000) else key / 1000,
            expected[0],
            '-'.join(["{0:.2f}".format(x) for x in expected[-2:]]),
            '-'.join(["{0:.2f}".format(x) for x in expected[-4:-2]]),
            observed[0]))

    print("\\hline")

def printInWikiFormat(limits, masses = set()):
    '''
    print observed and expected lmist in Wiki format

    limits is a dictionary with two keys: observed, and expected
    '''
    print("| Process | Expected [pb] | Central 1sigma | Central 2sigma | Observed [pb] |")

    keys = limits["expected"].keys() & limits["observed"].keys()
    if masses:
        keys &= masses

    for key in sorted(keys):
        expected = limits["expected"][key]
        observed = limits["observed"][key]

        print("| Zprime (M = {0}~\TeVcc) | {1:.2f} | {2} | {3} | {4:.2f} |".format(
            key // 1000 if 0 == (key % 1000) else key / 1000,
            expected[0],
            '-'.join(["{0:.2f}".format(x) for x in expected[-2:]]),
            '-'.join(["{0:.2f}".format(x) for x in expected[-4:-2]]),
            observed[0]))
