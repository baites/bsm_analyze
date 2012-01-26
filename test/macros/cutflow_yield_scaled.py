#!/usr/bin/env python3
#
# Created by Samvel Khalatyan, Jan 24, 2012
# Copyright 2011, All rights reserved

import math
import os
import re
import sys


print_order = (
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

def printInTextFormat(cutflow):
    line_length = 20 + 15 * len(list(cutflow.values())[0])
    for key in print_order:
        if not key:
            print("-" * line_length)
        else:
            if key in cutflow.keys():
                line = "{0:>20}".format(key)
                for events, err in cutflow[key]:
                    line += " {0:>7.0f}".format(events)
                    if "data" != key:
                        line += " + {0:<5.0f}".format(err)

                print(line)

def printInLatexFormat(cutflow):
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

    keys = lables.keys() & cutflow.keys()
    for key in print_order:
        if not key:
            print("\\hline")
        else:
            if key in keys:
                line = "{0:<30}".format(lables[key])
                for events, err in cutflow[key]:
                    line += " & ${0:>7.0f}".format(events)
                    if "data" != key:
                        line += " \\pm {0:<.0f}$".format(err)
                    else:
                        line += "$"

                print(line, end = "\\\\\n")

def printInWikiFormat(cutflow):
    for key in print_order:
        if key in cutflow.keys():
            line = "| {0:>20} |".format(key)
            for events, err in cutflow[key]:
                line += " {0:>7.0f}".format(events)
                if "data" != key:
                    line += " + {0:<.0f}".format(err)
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
    for channel in {"stop", "ttjets", "wjets", "zjets"} & cutflow_table.keys():
        cuts = [addCutflowRows(x, y) for x, y in zip(cuts, cutflow_table[channel])] if cuts else cutflow_table[channel]
    else:
        cutflow_table["mc"] = cuts
    
    cuts = []
    for channel in {"mc", "qcd_data"} & cutflow_table.keys():
        cuts = [addCutflowRows(x, y) for x, y in zip(cuts, cutflow_table[channel])] if cuts else cutflow_table[channel]
    else:
        cutflow_table["background"] = cuts

    print_format(cutflow_table)

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
