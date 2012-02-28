#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 28, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function

from loader import ChannelTemplateLoader

class Templates(object):
    def __init__(self, verbose = False):
        self.plots_to_use = []
        self.folders_to_use = []
        self.__verbose = verbose

    def run(self, channels):
        loader = ChannelTemplateLoader("output_signal_p150_hlt.root")
        loader.use_folders.extend(self.folders_to_use)
        loader.use_plots.extend(self.plots_to_use)
        loader.load(channels)

        if self.__verbose:
            print(loader)

        # Use TFraction Fitter to get QCD and MC fractions

        # Plot histograms

        # Don't forget to add plots, folders and channels support to main.py
