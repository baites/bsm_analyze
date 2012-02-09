#!/usr/bin/env python3
#
# Created by Samvel Khalatyan, Dec 21, 2011
# Copyright 2011, All rights reserved

'''
Read Theta limits from text files and convert these to LaTeX format
'''
import sys

import theta_limit

def usage(argv):
    return ("usage: {0} [exp:exp_limit.txt] [obs:obs_limit.txt] "
            "[format:text|latex|wiki] [mass:1000,2000] [errors:yes]").format(argv[0])

def main(argv = sys.argv):
    try:
        args = {}
        if 1 < len(argv):
            args = dict(x.split(":") for x in argv[1:] if ":" in x)

        limits = {
                "expected": theta_limit.load(args.get("exp", "exp_limit.txt")),
                "observed": theta_limit.load(args.get("obs", "obs_limit.txt"))
                }

        mass_points = args.get("mass", set())
        if mass_points:
            mass_points = set(int(x) for x in mass_points.split(','))

        {
            "text": theta_limit.printInTextFormat,
            "latex": theta_limit.printInLatexFormat,
            "wiki": theta_limit.printInWikiFormat
        }[args.get("format", "text")](limits, mass_points, args.get("errors", False))

    except Exception as error:
        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
