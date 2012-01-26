#!/usr/bin/env python3
#
# Created by Samvel Khalatyan, Jan 24, 2012
# Copyright 2011, All rights reserved

import os
import re
import sys

import cutflow

def run(**argv):
    cutflow_table = cutflow.load(**argv)

    for key in sorted(cutflow_table.keys()):
        print("{0:<20} {1:<10}".format(key, cutflow_table[key][-1]))

def usage(argv):
    return ("usage: {0} [log:debug_signal_p150_hlt.log] [data:folder,folder] "
            "[stop:folder,folder] [ttjets:folder] [wjets:folder] "
            "[zjets:folder] [zprime:folder,folder] [qcd_data:folder,folder]"
            ).format(argv[0])

def main(argv = sys.argv):
    try:
        if 2 > len(argv):
            raise Exception("No input is specified\n\n" + usage(argv))

        inputs = dict((x.split(':') for x in argv[1:] if ':' in x))
        inputs = {k: v.split(',') for k, v in inputs.items() if k in {"log", "data", "stop", "ttjets", "wjets", "zjets", "zprime", "qcd_data"}}

        run(**inputs)

    except Exception as error:
        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
