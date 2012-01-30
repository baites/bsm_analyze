#!/usr/bin/env python3
#
# Created by Samvel Khalatyan, Jan 27, 2011
# Copyright 2011, All rights reserved

'''
Configuration file to run any bsm executable
'''

import os
import sys

class RunConfig:
    def __init__(self):
        self.max_process = 1
        self.suffix = ("", "")
        self.datasets = ""
        self.command = ""
        self.link = {}

    def load(self, filename):
        if not os.path.lexists(filename):
            print("run config is not loaded b/c config file does not exist: "
                    + filename, file = sys.stderr)

            return

        with open(filename) as config:
            for line in config:
                line = line.strip()

                if not line or line.startswith('#'):
                    continue

                if ':' not in line:
                    print("didn't understand line: " + line, file = sys.stderr)

                    continue

                option, value = (x.strip() for x in line.split(':', 1))

                if option not in self.__dict__:
                    print("unsupported configuration option: " + option,
                            file = sys.stderr)

                    continue

                if "max_process" == option:
                    value = int(value)
                elif "link" == option:
                    value = value.split(' ', 1)

                    if 2 != len(value):
                        print("link format: link source", file = sys.stderr)

                        continue

                    if value and os.path.exists(value[1]):
                        self.link[value[0]] = os.path.realpath(value[1])
                    else:
                        print("broken link - source does not exist: "
                                + value[1], file = sys.stderr)

                    continue
                elif "suffix" == option:
                    if ':' not in value:
                        print("suffix format: pattern:replacement",
                                file = sys.stderr)

                        continue

                    value = value.split(':', 1)

                self.__dict__[option] = value

    def __str__(self):
        options = self.__dict__.copy()
        options["suffix"] = ':'.join(options["suffix"])

        if options["link"]:
            options["link"] = "\n".join("link: " + " ".join(x) for x in options["link"])
        else:
            options["link"] = ""

        return '\n'.join(["# This configuration is automatically generated",
                            "",
                            "max_process: {max_process}",
                            "suffix: {suffix}",
                            "datasets: {datasets}",
                            "{link}",
                            "command: {command}"]).format(**options)

if "__main__" == __name__:
    config = RunConfig()
    config.suffix = ("SFX", "test")
    config.datasets = "datasets.txt"
    config.command = "echo 'hello world'"
    config.link = [("link" + str(i), "file" + str(i)) for i in range(5)]

    sys.exit(print(config))
