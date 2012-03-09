#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 21, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function

import sys
from optparse import OptionParser

def main():
    try:
        parser = OptionParser(
                usage = "usage: %prog [options] [plots:A,B,C] [folders:A,B,C] "
                        "[channels:A,B,C]")

        parser.add_option("-b", "--batch",
            action = "store_true", default = False,
            help = "Run application in batch mode")

        parser.add_option("-v", "--verbose",
            action = "store_true", default = False,
            help = "Print additional info")

        parser.add_option("--filename",
            action = "store", default = "output_signal_p150_hlt.root",
            help = "input filename")

        parser.add_option("--scales",
            action = "store", 
            help = "scales filename")

        parser.add_option("--fractions",
            action = "store",
            help = "Use specified fractions insted of dynamic TFractionFitter results")

        parser.add_option("--ratio",
            action = "store", default = "default",
            help = "Use channels ratio in comparison")

        parser.add_option("--notff",
            action = "store_true",
            help = "Do NOT use TFraction Fitter")

        options, args = parser.parse_args()

        # import templates only here otherwise PyROOT inhercepts --help option
        import templates

        app = templates.Templates()
        app.run(options, args)

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
