#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 21, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function

import sys
import templates

def usage(argv):
    return ("usage: {0} [plots:A,B,C] [folders:A,B,C] "
            "[channels:A,B,C]").format(argv[0])

def main(argv = sys.argv):
    try:
        app = templates.Templates(verbose = True)
        app.run(argv[1:])

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
