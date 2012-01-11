#!/usr/bin/env python3.2
#
# Created by Samvel Khalatyan, Jan 09, 2012
# Copyright 2012, All rights reserved

import os
import sys

if "__main__" == __name__:
    if 2 != len(sys.argv):
        print("usage: {0[0]} weights.txt".format(sys.argv))
        exit(1)

    if not os.path.exists(sys.argv[1]):
        print("input does not exist: {0}".format(sys.argv[1]))
        exit(1)

    with open(sys.argv[1]) as input:
        for line in input:
            line = line.strip()
            if line.startswith("#"):
                continue

            channel, weight = line.split()

            print("{0:<10} {1:.2f}".format(channel, float(weight)))

    exit(0)
