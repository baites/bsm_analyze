#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 28, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function

import channel_type
import root.style
import ROOT
import numpy

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

        self.loader = None
        self.fractions = dict.fromkeys(["mc", "qcd"])

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

        self.load()

        self.get_qcd_mc_fractions()

        # Plot histograms

    def load(self):
        self.loader = ChannelTemplateLoader("output_signal_p150_hlt.root")

        self.loader.use_plots = self.use_plots
        self.loader.ban_plots = self.ban_plots

        self.loader.use_folders = self.use_folders
        self.loader.ban_folders = self.ban_folders

        self.loader.load(self.channels)

        if self.__verbose:
            print(self.loader)

    def get_qcd_mc_fractions(self):
        if not self.loader:
            return

        try:
            if ("/met" in self.loader.plots and
                "/met_noweight" in self.loader.plots):

                # Use TFraction Fitter to get QCD and MC fractions
                met = self.loader.plots["/met"]
                met_noweight = self.loader.plots["/met_noweight"]

                plots = dict.fromkeys(["data", "qcd", "mc"])

                for channel in met:
                    if channel.type in plots:
                        plots[channel.type] = channel

                for channel_type, channel in plots.items():
                    if not channel:
                        raise RuntimeError("{0} not loaded".format(channel_type.upper()))

                for channel in met_noweight:
                    if "mc" == channel.type:
                        plots["mc_noweight"] = channel
                        break
                else:
                    raise RuntimeError("Monte-Carlo is not loaded")

                mc_weights = plots["mc"].hist.Clone()
                mc_weights.SetDirectory(0)

                mc_weights.Divide(plots["mc_noweight"].hist)
                for xbin in range(1, mc_weights.GetNbinsX() + 1):
                    if 0 >= mc_weights.GetBinContent(xbin):
                        mc_weights.SetBinContent(xbin, 1)

                templates = ROOT.TObjArray(2)
                templates.Add(plots["mc_noweight"].hist)
                templates.Add(plots["qcd"].hist)

                fitter = ROOT.TFractionFitter(plots["data"].hist, templates)
                fitter.SetWeight(0, mc_weights)

                fit_status = fitter.Fit()
                if fit_status:
                    raise RuntimeError("fitter error {0}".format(fit_status))

                fraction = numpy.zeros(1, dtype = float)
                fraction_error = numpy.zeros(1, dtype = float)

                fitter.GetResult(0, fraction, fraction_error)
                self.fractions["mc"] = [fraction[0], fraction_error[0]]

                fitter.GetResult(1, fraction, fraction_error)
                self.fractions["qcd"] = [fraction[0], fraction_error[0]]

                if self.__verbose:
                    print('\n'.join("{0:>3} Fraction: {1:.3f}".format(
                            key.upper(),
                            value[0])
                        for key, value in self.fractions.items()))

            elif self.__verbose:
                raise RuntimeError("load plots 'met', 'met_noweight'")

        except RuntimeError as error:
            print("failed to use TFractionFitter - {0}".format(error),
                  file = sys.stderr)

    def __str__(self):
        result = []

        result.append("  use plots: {0}".format(self.use_plots))
        result.append("  ban plots: {0}".format(self.ban_plots))

        result.append("use folders: {0}".format(self.use_folders))
        result.append("ban folders: {0}".format(self.ban_folders))

        result.append("   channels: {0}".format(self.channels))

        return '\n'.join(result)
