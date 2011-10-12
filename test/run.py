#!/usr/bin/env python
#
# Run the same executable for certain skimmed datasets
#
# Created by Samvel Khalatyan, Oct 04, 2011
# Copyright 2011, All rights reserved

import os
import shlex
import subprocess
import sys

class AppController:
    def __init__(self, datasets, executable, max_process = 0):
        self.datasets = datasets
        self.executable = executable
        self.processes = []
        self.max_process = max_process if 0 <= max_process else 0

    def run(self):
        pwd = os.getcwd()
        args = shlex.split(self.executable + " input.txt")

        f = open(self.datasets, 'r')
        for input in f:
            folder = os.path.splitext(os.path.basename(input))[0]

            if sys.version_info < (2, 5):
                print("Run %s" % (folder))
            else:
                print("Run {0}".format(folder))

            os.mkdir(folder)
            os.chdir(folder)
            os.symlink(input.strip(), "input.txt")
            os.symlink("../jec", "jec")
            os.symlink("../lib", "lib")

            stdout = open("cout.log", 'w')
            stderr = open("cerr.log", 'w')
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
            print("Usage: %s datasets.txt max_process executable [exec args]" % (sys.argv[0]))
        else:
            print("Usage: {0} datasets.txt max_process executable [exec args]".format(sys.argv[0]))
    else:
        app = AppController(sys.argv[1],
                " ".join(sys.argv[3:]),
                max_process = int(sys.argv[2]))

        app.run()
