#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 28, 2012
Copyright 2011, All rights reserved
'''

from __future__ import division, print_function

import compare
import channel_type
import root.style
import ROOT
import numpy

from channel_template import MCChannelTemplate
from loader import ChannelTemplateLoader
from util.arg import split_use_and_ban
from root.comparison import ComparisonCanvas

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
        self.apply_qcd_mc_fractions()
        self.plot()

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

                for channel_type in ["data", "qcd", "mc"]:
                    if channel_type not in met:
                        raise RuntimeError("{0} not loaded".format(channel_type.upper()))

                if "mc" not in met_noweight:
                    raise RuntimeError("Monte-Carlo is not loaded")

                mc_weights = met["mc"].hist.Clone()
                mc_weights.SetDirectory(0)

                mc_weights.Divide(met_noweight["mc"].hist)
                for xbin in range(1, mc_weights.GetNbinsX() + 1):
                    if 0 >= mc_weights.GetBinContent(xbin):
                        mc_weights.SetBinContent(xbin, 1)

                templates = ROOT.TObjArray(2)
                templates.Add(met_noweight["mc"].hist)
                templates.Add(met["qcd"].hist)

                fitter = ROOT.TFractionFitter(met["data"].hist, templates)
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

    def apply_qcd_mc_fractions(self):
        mc_fraction = self.fractions["mc"][0]
        qcd_fraction = self.fractions["qcd"][0]

        if not mc_fraction or not qcd_fraction:
            return

        for channels in self.loader.plots.values():
            mc_scale = (mc_fraction *
                        channels["data"].hist.Integral() /
                        channels["mc"].hist.Integral())

            qcd_scale = (qcd_fraction *
                         channels["data"].hist.Integral() /
                         channels["qcd"].hist.Integral())

            channels["qcd"].hist.Scale(qcd_scale)

            for channel_type in channels["mc"].allowed_inputs:
                channel = channels.get(channel_type)
                if channel:
                    channel.hist.Scale(mc_scale)

            channels["mc"].hist.Scale(mc_scale)

    def plot(self):
        # container where all canvas related objects will be saved
        class Canvas: pass

        canvases = []
        for name, channels in self.loader.plots.items():
            obj = Canvas()

            c = ComparisonCanvas()
            obj.canvas = c

            c.canvas.cd(1)

            mc_stack = ROOT.THStack()
            obj.mc_stack = mc_stack

            background_channels = set(channels.keys()) & set(MCChannelTemplate("mc").allowed_inputs + ["qcd"])
            for channel_type in ["qcd"] + MCChannelTemplate("mc").allowed_inputs:
                if channel_type in background_channels:
                    mc_stack.Add(channels[channel_type].hist)

            mc_stack.Draw("9 hist")

            if "data" in channels:
                channels["data"].hist.Draw("9 same")

            bg_error = channels["mc"].hist.Clone()
            bg_error.SetDirectory(0)

            obj.bg_error = bg_error

            bg_error.Add(channels["qcd"].hist)

            bg_error.Draw("9 e2 same")

            c.canvas.cd(2)

            ratio = compare.data_mins_bg_over_bg(channels["data"].hist,
                                                 bg_error)
            obj.ratio = ratio
            ratio.Draw("9 e")

            c.canvas.Update()

            canvases.append(obj)

        raw_input('enter')

    def __str__(self):
        result = []

        result.append("  use plots: {0}".format(self.use_plots))
        result.append("  ban plots: {0}".format(self.ban_plots))

        result.append("use folders: {0}".format(self.use_folders))
        result.append("ban folders: {0}".format(self.ban_folders))

        result.append("   channels: {0}".format(self.channels))

        return '\n'.join(result)
