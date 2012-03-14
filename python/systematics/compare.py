#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Mar 13, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function

import sys
from optparse import OptionParser

def main():
    try:
        parser = OptionParser(usage = "usage: %prog [options]")

        parser.add_option(
                "-b", "--batch",
                action = "store_true", default = False,
                help = "Run application in batch mode")

        parser.add_option(
                "-v", "--verbose",
                action = "store_true", default = False,
                help = "Print additional info")

        parser.add_option(
                "--filename",
                action = "store", default = "output_signal_p150_hlt.root",
                help = "input filename")

        parser.add_option(
                "--channels",
                action = "store",
                help = "Load only comma separated channels")

        parser.add_option(
                "--systematic",
                action = "store",
                help = ("Systematics type to be loaded: jes, pileup, "
                        "matching, scaling"))

        options, args = parser.parse_args()

        from systematics.comparator import Comparator
        
        app = Comparator(options, args)
        app.run()

    except RuntimeError as error:
        # print error traceback for debug
        import traceback

        traceback.print_tb(sys.exc_info()[2])

        print(error, file = sys.stderr)

        return 1

    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
