#!/usr/bin/env python
#
# Created by Samvel Khalatyan, Feb 06, 2012
# Copyright 2012, All rights reserved

from __future__ import division, print_function

import os
import sys

import ROOT

def merge(file_in, file_out):
    if not os.path.exists(file_in):
        raise Exception("file does not exist: " + file_in)

    in_file = ROOT.TFile.Open(file_in, "read")
    if not in_file:
        raise Exception("failed to open input file: " + file_in)

    out_file = ROOT.TFile.Open(file_out, "recreate")
    if not out_file:
        raise Exception("failed to open output file: " + file_out)

    keys = set([x.GetName() for x in in_file.GetListOfKeys()])

    # remove all zjets and single-tops
    keys = set(x for x in keys if "zjets" not in x and "singletop" not in x)

    for key in keys:
        h = in_file.Get(key)
        if not h:
            print("failed to extract template: " + key, file = sys.stderr)

            continue

        h = h.Clone()
        if not h:
            print("failed to clone plot: " + key, file = sys.stderr)

            continue
        h.SetDirectory(out_file)

        if "wjets" in key:
            plot = key.replace("wjets", "zjets")
        elif "_ttbar_" in key:
            plot = key.replace("_ttbar_", "_singletop_")
        else:
            plot = None

        if plot:
            h1 = in_file.Get(plot)

            if not h1:
                print("failed to extract plot: " + plot, file = sys.stderr)
            else:
                h.Add(h1)

        h.Write(key)

    out_file.Close()

def usage(argv):
    return "usage: {0} theta_input.root output.root".format(argv[0])

def main(argv = sys.argv):
    try:
        if 3 > len(sys.argv):
            raise Exception(usage(argv))

        merge(argv[1], argv[2])
    except Exception as error:
        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
