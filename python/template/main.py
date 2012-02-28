#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 21, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function

import root.style
import sys
import templates


def usage(argv):
    return ("usage: {0} [plots:A,B,C] [folders:A,B,C] "
            "[channels:A,B,C]").format(argv[0])

def main(argv = sys.argv):
    try:
        style = root.style.tdr()
        style.cd()

        # Don't forget to add plots, folders and channels support to main.py

        app = templates.Templates(verbose = True)
        app.folders_to_use = [""]
        app.plots_to_use = ["mttbar_after_htlep"]
        app.run([
            "qcd", "data",

            "ttbar", "zjets", "wjets", "stop",

            "zprime_m1000_w10",
            "zprime_m1500_w15",
            "zprime_m2000_w20",
            "zprime_m3000_w30",
                  ])

    except Exception as error:
        # print Exception traceback for debug
        import traceback

        traceback.print_tb(sys.exc_info()[2])

        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
