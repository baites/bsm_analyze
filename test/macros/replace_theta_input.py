#!/usr/bin/env python
#
# Created by Samvel Khalatyan, Jan 22, 2012
# Copyright 2012, All rights reserved

from __future__ import division, print_function

import sys

import replace_template
import root_style

def usage(argv):
    return "usage: {0} in:input.root with:replace.root to:output.root plot [plot]".format(argv[0])

def main(argv = sys.argv):
    try:
        if 5 > len(argv):
            raise Exception(usage(argv))

        root_style.style("tdrstyle.C")

        files = dict([x.split(':') for x in argv[1:] if ':' in x])
        templates = set([x for x in argv[1:] if ':' not in x])

        files = dict([[x + "_file", files.get(x)] for x in ["in", "with", "to"]])

        print("files: {0}".format(files))

        replace_template.replace(templates, **files)

    except Exception as error:
        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
