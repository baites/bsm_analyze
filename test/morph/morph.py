#!/usr/bin/env python3.2

import os
import shlex
import subprocess
import sys

def usage(argv):
    return "Usage: {0} [process:number] [file:theta_input.root]".format(argv[0])

class Morph:
    def __init__(self, process = 1, file = "theta_input.root"):
        if not os.path.lexists(file):
            raise Exception("input file does not exist: " + file)

        self.max_process = min(1, int(process))
        self.processes = []
        self.file = file

    def run(self):
        for plot_suffix in ["",
                "__jes__minus",
                "__jes__plus",
                "__pileup__minus",
                "__pileup__plus"]:
            for bounds, mass_points in {
                    (1000, 1500): (1250,),
                    (1500, 2000): (1750,),
                    (2000, 3000): (2500,)
                    }.items():
                for mass_point in mass_points:
                    stdout = open("cout_{0}{1}.log".format(mass_point,
                                                           plot_suffix), "w")

                    stderr = open("cerr_{0}{1}.log".format(mass_point,
                                                           plot_suffix), "w")

                    args = {
                            "input_file": self.file,
                            "low_plot": self.histogram(bounds[0], plot_suffix),
                            "high_plot": self.histogram(bounds[1], plot_suffix),
                            "low_mass": bounds[0],
                            "high_mass": bounds[1],
                            "target_mass": mass_point,
                            "target_plot": self.histogram(mass_point, plot_suffix),
                            "target_file": "theta_input_{0}{1}.root".format(mass_point, plot_suffix)
                            }

                    command = ("./morph {input_file} {low_plot} "
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
    try:
        if not os.path.exists("morph"):
            raise Exception("executable does not exist: morph")

        morph = Morph(**dict((x.split(':') for x in argv[1:] if ':' in x)))

        morph.run()
    except Exception as error:
        print(error, file = sys.stderr)
        print(file = sys.stderr)
        print(usage(argv), file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
