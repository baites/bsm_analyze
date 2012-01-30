#!/usr/bin/env python3
#
# Created by Samvel Khalatyan, Oct 04, 2011
# Copyright 2011, All rights reserved

'''
Run the same executable for certain skimmed datasets
'''

import os
import sys

import run_bsm

def usage(argv):
    return ("usage: {0} config.txt [config.txt]").format(argv[0])

def main(argv = sys.argv):
    try:
        if 2 > len(argv):
            raise Exception(usage(argv))

        for config in argv[1:]:
            if not os.path.lexists(argv[1]):
                print("configuration file does not exist: " + config,
                        file = sys.stderr)

                continue

            app = run_bsm.AppController(config = config)
            app.run()
    except Exception as error:
        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main(sys.argv))
