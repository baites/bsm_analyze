#!/usr/bin/env python
#
# Created by Samvel Khalatyan, Jan 20, 2012
# Copyright 2012, All rights reserved

from __future__ import division, print_function

import os
import sys

from ROOT import *

import root_style
from compare import Compare

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
    scaling_channels = set(["wjets", "ttbar"])
    jes_channels = scaling_channels | set(["zjets", "singletop"]) | set(map(lambda x: "zp" + str(x), (1000, 1500, 2000, 3000, 4000)))
    all_channels = jes_channels | set(["DATA", "eleqcd"])

    for channel in [y + x for x in "__matching__plus", "__matching__minus", "__scaling__plus", "__scaling__minus" for y in scaling_channels] + [y + x for x in "__pileup__plus", "__pileup__minus", "__jes__plus", "__jes__minus" for y in jes_channels] + list(all_channels):
        histogram = "el_mttbar__" + channel

        lh = left_input.Get(histogram)
        if not lh:
            print("histogram: " + histogram
                    + " is not found in the left file: " + left,
                    file = sys.stderr)

            continue

        lh.SetLineColor(ROOT.kRed + 1)
        lh.SetTitle("left")

        rh = right_input.Get(histogram)
        if not rh:
            print("histogram: " + histogram
                    + " is not found in the right file: " + right,
                    file = sys.stderr)

            continue

        rh.SetLineColor(ROOT.kGreen + 1)
        rh.SetTitle("right")

        for h in lh, rh:
            h.Rebin(100)
            h.SetMarkerSize(0.5)
            h.SetLineWidth(1)

        compare = Compare()
        compare(lh, rh, title = channel)

        objects.append(compare)

    if objects:
        raw_input("press enter")

def usage(argv):
    return "usage: {0} left:theta_input.root right:theta_input.root".format(argv[0])

def main(argv = sys.argv):
    try:
        if 3 > len(sys.argv):
            raise Exception(usage(argv))

        root_style.style("tdrstyle.C")

        compareFiles(**dict(x.split(':') for x in sys.argv[1:] if ':' in x))
    except Exception as error:
        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
