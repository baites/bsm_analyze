#!/usr/bin/env python3
#
# Created by Samvel Khalatyan, Jan 24, 2012
# Copyright 2011, All rights reserved

import math
import os
import re
import sys

print_order = (
    "zprime_1000",
    "zprime_1500",
    "zprime_2000",
    "zprime_3000",
    None,
    "stop",
    "wjets",
    "zjets",
    "ttbar",
    None,
    "mc",
    "data"
)

def printInTextFormat(cutflow):
    line_length = 20 + 15 * len(list(cutflow.values())[0])
    for key in print_order:
        if not key:
            print("-" * line_length)
        else:
            if key in cutflow.keys():
                line = "{0:>20}".format(key)
                for events, err in cutflow[key]:
                    if "data" == key:
                        line += " {0:>7.0f}        ".format(events)
                    else:
                        line += " {0:>7.0f} + {1:<5.0f}".format(events, err)

                print(line)

def printInLatexFormat(cutflow):
    lables = {
            "zprime_1000": "$\\Zprime$, $M=1\\TeVcc$",
            "zprime_1500": "$\\Zprime$, $M=1.5\\TeVcc$",
            "zprime_2000": "$\\Zprime$, $M=2\\TeVcc$",
            "zprime_3000": "$\\Zprime$, $M=3\\TeVcc$",
            "stop": "Single Top",
            "wjets": "$\\W$+jets",
            "zjets": "$\\Z$+jets",
            "ttbar": "$\\rm t\\bar{t}$", 
            "background": "Total Background",
            "data": "Data 2011",
            "qcd": "QCD data-driven",
            "mc": "Total MC"
            }

    for cutflow in ({k: v[:-3] for k, v in cutflow.items()}, {k: v[-3:] for k, v in cutflow.items()}):
        keys = lables.keys() & cutflow.keys()
        for key in print_order:
            if not key:
                print("\\hline")
            else:
                if key in keys:
                    line = "{0:<30}".format(lables[key])
                    for events, err in cutflow[key]:
                        if "data" == key:
                            line += " & ${0:>7.0f}$".format(events)
                        else:
                            line += " & ${0:>7.0f} \\pm {1:<.0f}$".format(events, err)

                    print(line, end = " \\\\\n")

        print()

def printInWikiFormat(cutflow):
    for key in print_order:
        if key in cutflow.keys():
            line = "| {0:>20} |".format(key)
            for events, err in cutflow[key]:
                if "data" == key:
                    line += " {0:>7.0f}".format(events)
                else:
                    line += " {0:>7.0f} + {0:<.0f}".format(events, err)

                line += " |"

            print(line)

def cutflow(filename, print_format = printInTextFormat):
    if not os.path.lexists(filename):
        raise Exception("input file does not exist: " + filename)

    def addCutflowRows(x, y):
        return x[0] + y[0], math.sqrt(x[1] ** 2 + y[1] ** 2)

    cutflow_table = {}
    with open(filename) as in_file:
        for line in in_file:
            line =line.strip()
            if line.startswith('#') or not line:
                continue

            # split events into: x+error entries
            #
            channel, *events = line.split()

            # split each x+error into (x, error) and convert these to floats
            # results would be a list of pairs: (x, error)
            #
            events = [(float(y), float(z)) for y, z in (x.split("+") for x in events)]

            # merge all single-tops
            #
            if channel.startswith("stop") or channel.startswith("satop"):
                if "stop" in cutflow_table:
                    cutflow_table["stop"] = [addCutflowRows(x, y) for x, y in zip(cutflow_table["stop"], events)]
                else:
                    cutflow_table["stop"] = events
            else:
                cutflow_table[channel] = events

    cuts = []
    for channel in {"stop", "ttbar", "wjets", "zjets"} & cutflow_table.keys():
        cuts = [addCutflowRows(x, y) for x, y in zip(cuts, cutflow_table[channel])] if cuts else cutflow_table[channel]
    else:
        cutflow_table["mc"] = cuts
    
    cuts = []
    for channel in {"mc", "qcd"} & cutflow_table.keys():
        cuts = [addCutflowRows(x, y) for x, y in zip(cuts, cutflow_table[channel])] if cuts else cutflow_table[channel]
    else:
        cutflow_table["background"] = cuts

    print_format({k: v[-8:-6] + v[-4:] for k, v in cutflow_table.items()})

def usage(argv):
    return "usage: {0} cutflow.txt [format:(latex|wiki|txt)]".format(argv[0])

def main(argv = sys.argv):
    try:
        if 2 > len(argv):
            raise Exception(usage(argv))

        print_format = {
                "text": printInTextFormat,
                "wiki": printInWikiFormat,
                "latex": printInLatexFormat
                }

        user_format = dict(x.split(':') for x in argv[1:] if ':' in x).get("format", "text")
        if user_format not in print_format:
            raise Exception("supported print formats: " + ", ".join(print_format.keys()))

        cutflow([x for x in argv[1:] if ':' not in x][0], print_format[user_format])
    except Exception as error:
        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
