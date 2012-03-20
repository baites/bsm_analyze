#!/usr/bin/env python3
#
# Created by Samvel Khalatyan, Jan 13, 2012
# Copyright 2011, All rights reserved

import sys

import bsm_theta


def usage(argv):
    return ("usage: {0} mass [mass] [hist:theta_input.root] "
            "[analysis:analysis.py] [xsec:theory-xsecs.py] "
            "[run:run.sh] [ban:jes,pileup]").format(argv[0])

def main(argv = sys.argv):
    try:
        if 2 > len(argv):
            raise Exception(usage(argv))

        args = dict(x.split(":") for x in argv[1:] if ":" in x)

        args = {k: args.get(k) for k in args.keys() & {"hist", "analysis", "xsec", "run", "ban"}}

        mass_points = set(int(x) for x in argv[1:] if ':' not in x)

        bsm_theta.submit(mass_points, **args)

    except Exception as error:
        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
