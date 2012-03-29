#!/usr/bin/env python
#
# Created by Samvel Khalatyan, Jan 30, 2012
# Copyright 2011, All rights reserved

from __future__ import print_function

from datetime import datetime
import os
import re
import shlex
import shutil
import subprocess

import bsm_condor

def createHistogramFilter(mass_point, ban_plots=set()):
    with open("histogram_filter.py", "w") as output:
        print('''
#!/usr/bin/env python
#
# Code is automatically generated

def histogram_filter(name):
    ban_plots = [{ban}]

    return (re.match("^el\w*_mttbar.*$", name) and
            ("zp{signal}" in name if re.match("^el\w*_mttbar__zp.*$", name) else True) and
            not any(x in name for x in ban_plots))'''.format(
                signal=mass_point,
                ban=','.join('"{0}"'.format(x) for x in ban_plots)
                    if ban_plots else ""),
                file=output)

def copyAndSymlink(src, symlink):
    shutil.copy(src, "./")
    base_name = os.path.basename(src)
    if base_name != symlink:
        os.symlink(base_name, symlink)

def inputRealpath(name, path):
    if not os.path.exists(path):
        raise Exception(name + " file does not exist: " + path)
    else:
        return os.path.realpath(path)

def submit(mass_points=set(),
           hist="theta_input.root",
           analysis="analysis.py",
           xsec="theory-xsecs.py",
           run="run.sh",
           ban=set()):

    if not mass_points:
        raise Exception("No mass points specified")

    hist = inputRealpath("Theta input", hist)
    analysis = inputRealpath("Analysis script", analysis)
    xsec = inputRealpath("Theory xsec", xsec)
    run = inputRealpath("Run script", run)

    if ban:
        if not re.match("^((\w+),)*(\w+)$", ban):
            raise Exception("didn't understan plot names to ban: " + ban)

        ban = ban.split(',')

    output_folder = "prod_" + datetime.now().strftime("%Y_%m_%d_%H_%M_%S")

    if os.path.exists(output_folder):
        raise Exception("output folder exists: " + output_folder)

    os.mkdir(output_folder, 0o755)
    os.chdir(output_folder)

    try:
        links = {
                "theta_input.root": hist,
                "theory-xsecs.py": xsec,
                "analysis.py": analysis,
                "run.sh": run
                }

        for link, src in links.items():
            copyAndSymlink(src, link)

        for i, mass in enumerate(mass_points):
            job_folder = "job.{0}".format(i)

            os.mkdir(job_folder, 0o755)
            os.chdir(job_folder)
            
            try:
                for f in set(links.keys()) - set(["run.sh"]):
                    os.symlink("../" + f, f)

                createHistogramFilter(mass, ban)
            finally:
                os.chdir("../")

        config = bsm_condor.Config()
        config.jobs = len(mass_points)
        config.executable = "run.sh"
        config.transfer_inputs = ','.join((set(links.keys()) -
                                           set(["run.sh"])) |
                                          set(["histogram_filter.py"]))

        with open('condor.cfg', 'w') as output:
            print(config, file=output)

        print("Submit {0} CONDOR jobs".format(len(mass_points)))
        with open("condor_cout.log", "w") as stdout:
            with open("condor_cerr.log", "w") as stderr:
                process = subprocess.Popen(shlex.split("condor_submit condor.cfg"),
                                           stdout=stdout,
                                           stderr=stderr)

                process.wait()

    finally:
        os.chdir("../")
