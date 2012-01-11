#!/usr/bin/env python3.2

import os
import shlex
import subprocess
import sys

def usage():
    print("Usage: {0} max_processes".format(sys.argv[0]), file = sys.stderr)

class Morph:
    def __init__(self, max_process = 0):
        self.max_process = max_process 
        self.processes = []

    def run(self):
        for plot_suffix in ["",
                "__jes__minus",
                "__jes__plus",
                "__pileup__minus",
                "__pileup__plus"]:
            for bounds, mass_points in {
                    #(750, 1000): tuple(range(800, 1000, 100)),
                    #(1000, 1500): tuple(range(1100, 1500, 100)),
                    #(1500, 2000): tuple(range(1600, 2000, 100)),
                    #(2000, 3000): tuple(range(2100, 3000, 100))
                    (1000, 1500): (1250, )
                    }.items():
                for mass_point in mass_points:
                    stdout = open("cout_{0}{1}.log".format(mass_point,
                                                           plot_suffix), "w")

                    stderr = open("cerr_{0}{1}.log".format(mass_point,
                                                           plot_suffix), "w")

                    args = {
                            "low_plot": self.histogram(bounds[0], plot_suffix),
                            "high_plot": self.histogram(bounds[1], plot_suffix),
                            "low_mass": bounds[0],
                            "high_mass": bounds[1],
                            "target_mass": mass_point,
                            "target_plot": self.histogram(mass_point, plot_suffix),
                            "target_file": "theta_input_{0}{1}.root".format(mass_point, plot_suffix)
                            }

                    command = ("./morph theta_input.root {low_plot} "
                               "{high_plot} {low_mass} {high_mass} "
                               "{target_mass} {target_plot} "
                               "{target_file}").format(**args)

                    print(command)
                    self.processes.append(subprocess.Popen(shlex.split(command),
                                                           stdout = stdout,
                                                           stderr = stderr))

                    if self.max_process == len(self.processes):
                        self.wait()

        self.wait()

    def histogram(self, mass, suffix = ""):
        return "el_mttbar__zp{0}{1}".format(mass, suffix)

    def wait(self):
        for process in self.processes:
            process.wait()

        self.processes = []

def main(argv = sys.argv):
    if 2 > len(argv):
        usage()
    else:
        if not os.path.exists("morph"):
            print("executable does not exist: morph")
        else:
            try:
                morph = Morph(max_process = int(argv[1]))
            except ValueError:
                usage()
            else:
                morph.run()

                return 0

    return 1

if "__main__" == __name__:
    sys.exit(main())
