#!/usr/bin/env python3
#
# Created by Samvel Khalatyan, Jan 24, 2012
# Copyright 2011, All rights reserved

import os
import re
import sys

def loadCutflowFromLog(*, folder, log):
    cutflow = []
    if not os.path.lexists(folder):
        print("path does not exist: " + folder, file = sys.stderr)
    else:
        file = "{0}/{1}".format(os.path.realpath(folder), log)

        if not os.path.lexists(file):
            print("log file does not exist: " + file, file = sys.stderr)
        else:
            pattern = re.compile("\s+\[\+\].*\s+(\d+)\s+")
            with open(file) as in_file:
                for x in in_file:
                    match = pattern.match(x)
                    if match:
                        cutflow.append(int(match.group(1)))

    return cutflow

def loadCutflow(*, key, folders, log):
    cutflow = {}

    if folders:
        for folder in folders:
            loaded_cutflow = loadCutflowFromLog(folder = folder, log = log)
            if key in cutflow:
                cutflow[key] = list(map(lambda x, y: x + y,
                                                cutflow[key],
                                                loaded_cutflow))
            else:
                cutflow[key] = loaded_cutflow

    return cutflow

def load(*,
        log = "debug_signal_p150_hlt.log",
        data = set(),
        stop = set(),
        ttjets = None,
        wjets = None, 
        zjets = None,
        zprime = set(),
        qcd_data = set()):

    cutflow = {}
    cutflow.update(loadCutflow(key = "data", folders = data, log = log))

    cutflow.update(loadCutflow(key = "qcd_data",
                    folders = qcd_data,
                    log = log.replace("hlt", "qcd")))

    for folder in stop:
        cutflow.update(loadCutflow(key = os.path.basename(folder),
                                    folders = (folder, ),
                                    log = log))

    cutflow.update(loadCutflow(key = "ttjets", folders = ttjets, log = log))
    cutflow.update(loadCutflow(key = "wjets", folders = wjets, log = log))
    cutflow.update(loadCutflow(key = "zjets", folders = zjets, log = log))

    for folder in zprime:
        cutflow.update(loadCutflow(key = os.path.basename(folder),
                                    folders = (folder, ),
                                    log = log))

    return cutflow

if "__main__" == __name__:
    print("module is not designed for execution", file = sys.stderr)

    sys.exit(1)
