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
    for background in {"stop", "ttjets", "wjets", "zjets"} & cutflow.keys():
        cuts = list(map(lambda x, y: x + y, cuts, cutflow[background])) if cuts else cutflow[background]
    else:
        cutflow["background"] = cuts
    
    lables = {
            "zprime_m1000_w10": "\$Z^\\prime\$, \$M=1\TeVcc\$",
            "zprime_m1500_w15": "\$Z^\\prime\$, \$M=1.5\TeVcc\$",
            "zprime_m2000_w20": "\$Z^\\prime\$, \$M=2\TeVcc\$",
            "zprime_m3000_w30": "\$Z^\\prime\$, \$M=3\TeVcc\$",
            "stop": "Single Top",
            "wjets": "\$W\$+jets",
            "zjets": "\$Z\$+jets",
            "ttjets": "\$t\\bar{t}\$", 
            "background": "Total Background",
            "data": "Data 2011"
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
            "background",
            "data"
            )

    keys = lables.keys() & cutflow.keys()
    for key in order:
        if not key:
            print("\\\\hline")
        else:
            if key in keys:
                line = "| {0:<30}".format(lables[key])
                for events in cutflow[key][:-2]:
                    line += " | {0:>7.0f}".format(events)
                    if "data" != key:
                        line += " \\pm {0:<.0f}".format(math.sqrt(events))

                line += " |"

                print(line)

    print()
    for key in order:
        if not key:
            print("\\\\hline")
        else:
            if key in keys:
                line = "| {0:<30}".format(lables[key])
                for events in cutflow[key][-2:]:
                    line += " | {0:>7.0f}".format(events)
                    if "data" != key:
                        line += " \\pm {0:<.0f}".format(math.sqrt(events))

                line += " |"

                print(line)


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
