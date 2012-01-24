#!/usr/bin/env python3
#
# Created by Samvel Khalatyan, Jan 24, 2012
# Copyright 2011, All rights reserved

import math
import os
import re
import sys

def cutflow(filename):
    if not os.path.lexists(filename):
        raise Exception("input file does not exist: " + filename)

    cutflow = {}
    with open(filename) as in_file:
        for line in in_file:
            channel, *events = line.split()
            events = [float(x) for x in events]

            if channel.startswith("stop") or channel.startswith("satop"):
                if "stop" in cutflow:
                    cutflow["stop"] = list(map(lambda x, y: x + y, cutflow["stop"], events))
                else:
                    cutflow["stop"] = events
            else:
                cutflow[channel] = events

    cuts = []
    for channel in {"stop", "ttjets", "wjets", "zjets"} & cutflow.keys():
        cuts = list(map(lambda x, y: x + y, cuts, cutflow[channel])) if cuts else cutflow[channel]
    else:
        cutflow["mc"] = cuts
    
    cuts = []
    for channel in {"mc", "qcd_data"} & cutflow.keys():
        cuts = list(map(lambda x, y: x + y, cuts, cutflow[channel])) if cuts else cutflow[channel]
    else:
        cutflow["background"] = cuts

    lables = {
            "zprime_m1000_w10": "$\\Zprime$, $M=1\\TeVcc$",
            "zprime_m1500_w15": "$\\Zprime$, $M=1.5\\TeVcc$",
            "zprime_m2000_w20": "$\\Zprime$, $M=2\\TeVcc$",
            "zprime_m3000_w30": "$\\Zprime$, $M=3\\TeVcc$",
            "stop": "Single Top",
            "wjets": "$\\W$+jets",
            "zjets": "$\\Z$+jets",
            "ttjets": "$\\rm t\\bar{t}$", 
            "background": "Total Background",
            "data": "Data 2011",
            "qcd_data": "QCD data-driven",
            "mc": "Total MC"
            }

    order = (
            "zprime_m1000_w10",
            "zprime_m1500_w15",
            "zprime_m2000_w20",
            "zprime_m3000_w30",
            None,
            "stop",
            "wjets",
            "zjets",
            "ttjets",
            None,
            "mc",
            "qcd_data",
            None,
            "background",
            "data"
            )

    keys = lables.keys() & cutflow.keys()
    for key in order:
        if not key:
            print("\\hline")
        else:
            if key in keys:
                line = "{0:<30}".format(lables[key])
                for events in cutflow[key]:
                    line += " & ${0:>7.0f}".format(events)
                    if "data" != key:
                        line += " \\pm {0:<.0f}$".format(math.sqrt(events))
                    else:
                        line += "$"

                print(line, end = "\\\\\n")


def usage(argv):
    return "usage: {0} cutflow.txt".format(argv[0])

def main(argv = sys.argv):
    try:
        if 2 > len(argv):
            raise Exception(usage(argv))

        cutflow(argv[1])
    except Exception as error:
        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
