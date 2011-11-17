#!/usr/bin/env python
#
# Run the same executable for certain skimmed datasets
#
# Created by Samvel Khalatyan, Oct 04, 2011
# Copyright 2011, All rights reserved

import os
import re
import shlex
import subprocess
import sys

class AppController:
    def __init__(self, datasets, executable, max_process = 0, suffix = ""):
        self.datasets = datasets
        self.suffix = suffix
        self.executable = re.sub("SFX", self.suffix, executable)
        self.processes = []
        self.max_process = max_process if 0 <= max_process else 0

    def run(self):
        pwd = os.getcwd()
        args = shlex.split(self.executable + " input.txt")

        f = open(self.datasets, 'r')
        suffix = ("_" + self.suffix) if self.suffix else ""
        for input in f:
            folder = os.path.splitext(os.path.basename(input))[0]

            if sys.version_info < (2, 5):
                print("Run %s" % (folder))
            else:
                print("Run {0}".format(folder))

            if not os.path.isdir(folder):
                os.mkdir(folder)

            os.chdir(folder)
            
            if not os.path.islink("input.txt"):
                os.symlink(input.strip(), "input.txt")

            if not os.path.islink("jec"):
                os.symlink("../jec", "jec")

            if not os.path.islink("lib"):
                os.symlink("../lib", "lib")

            if not os.path.islink("pileup"):
                os.symlink("../pileup", "pileup")

            stdout = open("cout" + suffix +  ".log", 'w')
            stderr = open("cerr" + suffix + ".log", 'w')
            self.processes.append(subprocess.Popen(args, stdout = stdout, stderr = stderr))

            if self.max_process == len(self.processes):
                self.wait()

            os.chdir(pwd)

        self.wait()

    def wait(self):
        for process in self.processes:
            process.wait()

        self.processes = []

if "__main__" == __name__:
    if 4 > len(sys.argv):
        if sys.version_info < (2, 5):
            print("Usage: %s datasets.txt max_process suffix executable [exec args]" % (sys.argv[0]))
        else:
            print("Usage: {0} datasets.txt max_process suffix executable [exec args]".format(sys.argv[0]))
    else:
        app = AppController(sys.argv[1],
                " ".join(sys.argv[4:]),
                max_process = int(sys.argv[2]),
                suffix = sys.argv[3])

        app.run()
