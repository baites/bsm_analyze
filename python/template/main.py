#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 21, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function

import sys
import templates

def main():
    try:
        app = templates.Templates()
        app.run()

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
