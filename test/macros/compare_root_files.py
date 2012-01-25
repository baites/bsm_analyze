#!/usr/bin/env python
#
# Created by Samvel Khalatyan, Jan 24, 2012
# Copyright 2012, All rights reserved

from __future__ import division, print_function

import os
import sys

from ROOT import *

import root_style
import compare

def usage(argv):
    return "usage: {0} left:input.root right:input.root".format(argv[0])

def main(argv = sys.argv):
    try:
        if 3 > len(sys.argv):
            raise Exception(usage(argv))

        root_style.style("tdrstyle.C")

        inputs = dict(x.split(':') for x in argv[1:] if ':' in x)
        inputs = dict((x + "_input", inputs.get(x)) for x in ["left", "right"])

        comparison = compare.CompareFiles()
        comparison(**inputs);

        raw_input("enter")
    except Exception as error:
        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
