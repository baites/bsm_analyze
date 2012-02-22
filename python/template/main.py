#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 21, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function

import sys

def usage(argv):
    return "usage: {0} templates.cfg".format(argv[0])

def main(argv = sys.argv):
    try:
        if 2 > len(argv):
            raise RuntimeError(usage(argv))

    except Exception as error:
        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
