#!/usr/bin/env python
#
# Created by Samvel Khalatyan, Jan 20, 2012
# Copyright 2012, All rights reserved

from __future__ import division, print_function

import os
import sys

from ROOT import *

from compare import Compare

def rootStyle(filename):
    if os.path.lexists(filename):
        gROOT.ProcessLine(".L {0}".format(filename))
        ROOT.setTDRStyle()

        print("Loaded ROOT style from: {0}".format(filename))
    else:
        print("ROOT style is not available: " + filename)

def compareFiles(left, right):
    if not os.path.lexists(left):
        raise Exception("LEFT file does not exist: " + left)

    if not os.path.lexists(right):
        raise Exception("RIGHT file does not exist: " + right)

    left_input = TFile(left)
    if left_input.IsZombie():
        raise Exception("failed to open LEFT file: " + left)

    right_input = TFile(right)
    if right_input.IsZombie():
        raise Exception("failed to open RIGHT file: " + right)

    objects = []
    for channel in "ttbar", "zjets", "wjets", "singletop":
        histogram = "el_mttbar__" + channel

        lh = left_input.Get(histogram)
        if not lh:
            print("histogram: " + histogram
                    + " is not found in the left file: " + left,
                    file = sys.stderr)

            continue

        lh.SetLineColor(ROOT.kRed)
        lh.SetTitle("left")

        rh = right_input.Get(histogram)
        if not rh:
            print("histogram: " + histogram
                    + " is not found in the right file: " + right,
                    file = sys.stderr)

            continue

        rh.SetLineColor(ROOT.kGreen)
        rh.SetTitle("right")

        for h in lh, rh:
            h.Rebin(100)

        compare = Compare()
        compare(lh, rh)

        objects.append(compare)

    if objects:
        raw_input("press enter")

def usage(argv):
    return "usage: {0} left:theta_input.root right:theta_input.root".format(argv[0])

def main(argv = sys.argv):
    try:
        if 3 > len(sys.argv):
            raise Exception(usage(argv))

        rootStyle("tdrstyle.C")

        compareFiles(**dict(x.split(':') for x in sys.argv[1:] if ':' in x))
    except Exception as error:
        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
