#!/usr/bin/env python3
#
# Created by Samvel Khalatyan, Jan 27, 2011
# Copyright 2011, All rights reserved

import os
import re
import shlex
import subprocess

import run_bsm_config

class RunDatasets:
    def __init__(self):
        self.datasets = {}

    def load(self, filename):
        if not os.path.lexists(filename):
            print("datasets file does not exist: " + filename,
                    file = sys.stderr)

            return

        with open(filename) as file:
            for line in file:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue

                if ':' not in line:
                    # print error message to cerr
                    #
                    sys.stderr, sys.stdout = sys.stdout, sys.stderr

                    print("didn't understand line in dataset: " + line)
                    print()
                    print("accepted format...")
                    print("folder:/path/to/input.txt")
                    print()

                    # restore cout
                    #
                    sys.stderr, sys.stdout = sys.stdout, sys.stderr

                    continue

                key, value = [x.strip() for x in line.split(':', 1)]
                self.datasets[key] = value

    def __iter__(self):
        return iter(self.datasets.items())

class AppController:
    def __init__(self, *, config):
        self.config = run_bsm_config.RunConfig()
        self.config.load(config)

        self.process = []
        self.dirs = []

    def run(self):
        if not self.config.command:
            raise Exception("command is not defined in the configuration")

        command = self.config.command
        if self.config.suffix:
            command = command.replace(*self.config.suffix)

        if self.config.datasets:
            if not os.path.lexists(self.config.datasets):
                raise Exception("datasets file does not exist: "
                        + self.config.datasets)

            self.runDatasets(command)
        else:
            self.makeLinks()
            self.runCommand(command)

        self.wait()

    def pushd(self, folder):
        if os.path.isdir(folder):
            self.dirs.append(os.getcwd())
            os.chdir(folder)
        else:
            print("folder does not exist: " + folder, file = sys.stderr)

    def popd(self):
        if self.dirs:
            os.chdir(self.dirs.pop())
        else:
            print("dirs stack is empty")

    def waitOneProcess(self):
        if self.process:
            self.process.pop(0).wait()

    def wait(self):
        while self.process:
            self.waitOneProcess()

    def runDatasets(self, command):
        datasets = RunDatasets()
        datasets.load(self.config.datasets)

        args = command + " input.txt"
        for folder, file in datasets:
            if not os.path.isdir(folder):
                os.mkdir(folder)

            print("{0:<20}".format(folder), end = " ... ")
            self.pushd(folder)

            if not os.path.exists("input.txt"):
                os.symlink(file, "input.txt")

            self.makeLinks()

            print("run")
            self.runCommand(args)

            self.popd()

            if self.config.max_process <= len(self.process):
                self.waitOneProcess()

    def runCommand(self, command):
        suffix = ""
        if self.config.suffix and self.config.suffix[1]:
            suffix = "_" + self.config.suffix[1]

        cout = "cout{0}.log".format(suffix)
        cerr = "cerr{0}.log".format(suffix)

        self.process.append(subprocess.Popen(shlex.split(command),
            stdout = open(cout, "w"),
            stderr = open(cerr, "w")))

    def makeLinks(self):
        if not self.config.link:
            return

        for link, source in self.config.link.items():
            if not os.path.exists(link):
                os.symlink(source, link)
