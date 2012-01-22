#!/usr/bin/env python
#
# Created by Samvel Khalatyan, Jan 22, 2012
# Copyright 2012, All rights reserved

from __future__ import division, print_function

import os
import sys

import ROOT

def style(filename):
    if os.path.lexists(filename):
        ROOT.gROOT.ProcessLine(".L {0}".format(filename))
        ROOT.setTDRStyle()

        print("Loaded ROOT style from: " + filename)
    else:
        print("ROOT style is not available: " + filename, file = sys.stderr)
