#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 28, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function

import channel_type
import root.style

from loader import ChannelTemplateLoader
from util.arg import split_use_and_ban

class Templates(object):
    def __init__(self, verbose = False):
        self.use_plots = []
        self.ban_plots = []

        self.use_folders = []
        self.ban_folders = []

        self.channels = []

        self.__verbose = verbose

    def run(self, args):
        args = [x.split(':') for x in args if ':' in x]
        args = {key.strip():
                    set(x.strip() for x in values.split(','))
                        for key, values in args}

        self.use_plots, self.ban_plots = map(list,
                                             split_use_and_ban(args.get("plots",
                                                                        [])))

        use_folders, ban_folders = map(list,
                                       split_use_and_ban(args.get("folders",
                                                                  [])))

        use_channels, ban_channels = split_use_and_ban(args.get("channels",
                                                                []))

        channels = set(channel_type.ChannelType.channel_types.keys())
        if use_channels:
            channels &= use_channels

        if ban_channels:
            channels -= ban_channels

        self.channels = list(channels)

        if self.__verbose:
            print("{0:-<80}".format("-- Configuration "))
            print(self)
            print()

        self.process()

    def process(self):
        if not self.channels:
            return

        style = root.style.tdr()
        style.cd()

        loader = ChannelTemplateLoader("output_signal_p150_hlt.root")

        loader.use_plots = self.use_plots
        loader.ban_plots = self.ban_plots

        loader.use_folders = self.use_folders
        loader.ban_folders = self.ban_folders

        loader.load(self.channels)

        if self.__verbose:
            print(loader)

        # Use TFraction Fitter to get QCD and MC fractions

        # Plot histograms

    def __str__(self):
        result = []

        result.append("  use plots: {0}".format(self.use_plots))
        result.append("  ban plots: {0}".format(self.ban_plots))

        result.append("use folders: {0}".format(self.use_folders))
        result.append("ban folders: {0}".format(self.ban_folders))

        result.append("   channels: {0}".format(self.channels))

        return '\n'.join(result)
