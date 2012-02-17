#!/usr/bin/env python3
#
# Created by Samvel Khalatyan, Jan 30, 2012
# Copyright 2011, All rights reserved

'''
Set of condor related tools for Beyond the Standard Model searches
'''

class Config():
    '''
    Condor configuration file

    All BSM jobs share the same configuration for all jobs. This is a printable
    collection of attributes that are required to submit jobs
    '''
    def __init__(self):
        self.transfer_inputs = ""
        self.notify_email = ""
        self.executable = ""
        self.arguments = ""
        self.jobs = 1

    def __str__(self):
        return '''universe = vanilla
getenv = true
Requirements = Memory >= 199 && OpSys == "LINUX" && (Arch != "DUMMY" ) && Disk > 1000000
Should_Transfer_Files = YES
WhenToTransferOutput = ON_EXIT
Output = cout.log
Error = cerr.log
Log = clog.log
initialdir = job.$(Process)
transfer_input_files = {transfer_inputs}
notify_user = {notify_email}
Executable = {executable}
arguments = {arguments}
queue {jobs}'''.format(**self.__dict__)

if "__main__" == __name__:
    config = Config()
    config.transfer_inputs = "input1.txt input2.txt"
    config.notify_email = "user@host.name"
    config.executable = "/path/to/executable"
    config.arguments = "arg1 arg2"
    config.jobs = 1

    print(config)

    import sys
    sys.exit(1)
