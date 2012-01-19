#!/usr/bin/env python3
#
# Created by Samvel Khalatyan, Jan 13, 2012
# Copyright 2011, All rights reserved

from datetime import datetime
import os
import shlex
import shutil
import subprocess
import sys

def createHistogramFilter(mass_point):
    with open("histogram_filter.py", "w") as output:
        print('''
#!/usr/bin/env python
#
# Code is automatically generated

def histogram_filter(name):
    plots = ["el_mttbar__zp" + str(x) for x in range(900, 4100, 100) if x != {0}]

    return name.startswith("el_mttbar") and not name in plots
        '''.format(mass_point), file = output)

def createCondorFile(jobs = 1, exe = "", transfer = ""):
    if not exe:
        raise Exception("Executable is not specified")

    with open('condor.cfg', 'w') as output:
        print('''
universe = vanilla
getenv = true
Requirements = Memory >= 199 && OpSys == "LINUX" && (Arch != "DUMMY" ) && Disk > 1000000
Should_Transfer_Files = YES
WhenToTransferOutput = ON_EXIT
Output = cout.log
Error = cerr.log
Log = clog.log
initialdir = job.$(Process)
transfer_input_files = {transfer}
notify_user = 
Executable = {exe}
queue {jobs}
        '''.format(jobs = jobs, exe = exe, transfer = transfer),
        file = output)

def copyAndSymlink(src, symlink):
    shutil.copy(src, "./")
    base_name = os.path.basename(src)
    if base_name != symlink:
        os.symlink(base_name, symlink)

def inputRealpath(name, path):
    if not os.path.lexists(path):
        raise Exception(name + " file does not exist: " + path)
    else:
        return os.path.realpath(path)

def submit(mass_points = set(),
        hist = "theta_input.root",
        analysis = "analysis.py",
        xsec = "theory-xsecs.py",
        run = "run.sh"):
    if not mass_points:
        raise Exception("No mass points specified")

    hist = inputRealpath("Theta input", hist)
    analysis = inputRealpath("Analysis script", analysis)
    xsec = inputRealpath("Theory xsec", xsec)
    run = inputRealpath("Run script", run)

    output_folder = "prod_" + datetime.now().strftime("%Y_%m_%d_%H_%M_%S")

    if os.path.lexists(output_folder):
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
                for f in links.keys() - {"run.sh"}:
                    os.symlink("../" + f, f)

                createHistogramFilter(mass)
            finally:
                os.chdir("../")

        createCondorFile(jobs = len(mass_points),
                exe = "run.sh",
                transfer = ','.join((links.keys() - {"run.sh"})
                    | {"histogram_filter.py"}))

        print("Submit {0} CONDOR jobs".format(len(mass_points)))
        with open("condor_cout.log", "w") as stdout:
            with open("condor_cerr.log", "w") as stderr:
                process = subprocess.Popen(shlex.split("condor_submit condor.cfg"),
                                            stdout = stdout,
                                            stderr = stderr)

                process.wait()

    finally:
        os.chdir("../")

def usage(argv):
    return ("usage: {0} mass [mass] [hist:theta_input.root] "
            "[analysis:analysis.py] [xsec:theory-xsecs.py] "
            "[run:run.sh]").format(argv[0])

def main(argv = sys.argv):
    try:
        if 2 > len(argv):
            raise Exception(usage(argv))

        submit(mass_points = set(int(x) for x in argv[1:] if ':' not in x),
                **dict((x.split(":") for x in argv[1:] if ":" in x)))

    except Exception as error:
        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
