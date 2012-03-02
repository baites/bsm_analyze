#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 28, 2012
Copyright 2011, All rights reserved
'''

from __future__ import division, print_function

import compare
import channel_type
import numpy
import root.label
import root.style
import ROOT
import sys

from channel_template import MCChannelTemplate
from input_template import InputTemplate
from loader import ChannelTemplateLoader
from util.arg import split_use_and_ban
from root.comparison import ComparisonCanvas

class Templates(object):
    def __init__(self, verbose = False):
        self.__verbose = verbose

        self.use_plots = []
        self.ban_plots = []

        self.use_folders = []
        self.ban_folders = []

        self.use_channels = []

        self.loader = None
        self.fractions = dict.fromkeys(["mc", "qcd"])

    def run(self, args):
        # Create dictionary of arguments with key - arg name, value - arg value
        args = [x.split(':') for x in args if ':' in x]
        args = {key.strip(): set(x.strip() for x in values.split(','))
                    for key, values in args}

        self.use_plots, self.ban_plots = map(list,
                split_use_and_ban(args.get("plots", [])))

        self.use_folders, self.ban_folders = map(list,
                split_use_and_ban(args.get("folders", [])))

        use_channels, ban_channels = split_use_and_ban(args.get("channels", []))

        # use only allowed channels or all if None specified
        channels = set(channel_type.ChannelType.channel_types.keys())
        if use_channels:
            channels &= use_channels

        # remove banned channels
        if ban_channels:
            channels -= ban_channels

        self.use_channels = list(channels)

        # print run configuration
        if self.__verbose:
            print("{0:-<80}".format("-- Configuration "))
            print(self)
            print()

        self.__process()

    def __process(self):
        if not self.use_channels:
            raise RuntimeError("all channels are turned off")

        self.__load_channels()
        self.__fraction_fitter()
        self.__plot()

    def __load_channels(self):
        # Create and configure new loader
        self.loader = ChannelTemplateLoader("output_signal_p150_hlt.root")

        self.loader.use_plots = self.use_plots
        self.loader.ban_plots = self.ban_plots

        self.loader.use_folders = self.use_folders
        self.loader.ban_folders = self.ban_folders

        # load channels
        self.loader.load(self.use_channels)

        # print loader summary
        if self.__verbose:
            print(self.loader)

    def __fraction_fitter(self):
        try:
            self.__run_fraction_fitter()
            self.__apply_fractions()

        except RuntimeError as error:
            if self.__verbose:
                print("failed to use TFractionFitter - {0}".format(error),
                      file = sys.stderr)

    def __run_fraction_fitter(self):
        if ("/met" not in self.loader.plots or
            "/met_noweight" not in self.loader.plots):

            raise RuntimeError("load plots 'met', 'met_noweight'")

        # Use TFraction Fitter to get QCD and MC fractions
        met = self.loader.plots["/met"]
        met_noweight = self.loader.plots["/met_noweight"].get("mc")

        # make sure weighted channels are available
        for channel_type in ["data", "qcd", "mc"]:
            if channel_type not in met:
                raise RuntimeError("{0} is not loaded".format(
                    channel_type.upper()))

        # check if no-weighted MC is available
        if not met_noweight:
            raise RuntimeError("Monte-Carlo is not loaded")

        # prepare MC weights for TFraction fitter
        mc_weights = met["mc"].hist.Clone()
        mc_weights.SetDirectory(0)
        mc_weights.Divide(met_noweight.hist)

        # Set any zero bins to at least 1 event
        for xbin in range(1, mc_weights.GetNbinsX() + 1):
            if 0 >= mc_weights.GetBinContent(xbin):
                mc_weights.SetBinContent(xbin, 1)

        # prepare variable tempaltes for TFractionFitter
        templates = ROOT.TObjArray(2)
        templates.Add(met_noweight.hist)
        templates.Add(met["qcd"].hist)

        # Setup TFractionFitter
        fitter = ROOT.TFractionFitter(met["data"].hist, templates)
        fitter.SetWeight(0, mc_weights)

        # Run TFRactionFitter
        fit_status = fitter.Fit()
        if fit_status:
            raise RuntimeError("fitter error {0}".format(fit_status))

        # Extract MC and QCD fractions from TFractionFitter
        fraction = numpy.zeros(1, dtype = float)
        fraction_error = numpy.zeros(1, dtype = float)

        fitter.GetResult(0, fraction, fraction_error)
        self.fractions["mc"] = [fraction[0], fraction_error[0]]

        fitter.GetResult(1, fraction, fraction_error)
        self.fractions["qcd"] = [fraction[0], fraction_error[0]]

        # Print found fractions
        if self.__verbose:
            print('\n'.join("{0:>3} Fraction: {1:.3f}".format(
                    key.upper(),
                    value[0])
                for key, value in self.fractions.items()))

    def __apply_fractions(self):
        mc_fraction = self.fractions["mc"][0]
        qcd_fraction = self.fractions["qcd"][0]

        # For each loaded plot scale MC and QCD
        for plot, channels in self.loader.plots.items():
            try:
                # Make sure all necessary channels were loaded for the histogram
                for channel_type in ["data", "mc", "qcd"]:
                    if channel_type not in channels:
                        raise RuntimeError("{0} channel is not avialable for "
                                           "{1}".format(channel_type.upper(),
                                                        plot))

                # Cache data integral value
                data_integral = channels["data"].hist.Integral()
                mc_channel = channels["mc"]
                qcd_channel = channels["qcd"]

                qcd_channel.hist.Scale(qcd_fraction * data_integral /
                                       qcd_channel.hist.Integral())

                mc_scale = (mc_fraction * data_integral /
                            mc_channel.hist.Integral())

                mc_channel.hist.Scale(mc_scale)

                # scale those MC channels that were loaded
                for channel_type in mc_channel.allowed_inputs:
                    channel = channels.get(channel_type)
                    if channel:
                        channel.hist.Scale(mc_scale)

            except RuntimeError as error:
                print("failed to apply TFractionFitter scales - {0}".format(error),
                      file = sys.stderr)

    def __plot(self):
        # Apply TDR style to all plots
        style = root.style.tdr()
        style.cd()

        # container where all canvas related objects will be saved
        class Canvas: pass

        channel_names = {
                "qcd": "QCD data-driven",
                "stop": "Single-Top",
                "zjets": "Z/#gamma*#rightarrowl^{+}l^{-}",
                "wjets": "W#rightarrowl#nu",
                "ttbar": "t#bar{t}"
        }

        canvases = []

        '''
        loop over plots and draw them:

            1. all background channels stacked (MC channels + QCD)
            2. background error band (MC + QCD)
            3. data with errors
            4. ratio of data over background
        '''

        # loop over plots
        for name, channels in self.loader.plots.items():
            # create container for current objects
            obj = Canvas()

            # extact MC combined
            mc_combo = channels.get("mc")

            # extract Data
            data = channels.get("data")

            obj.legend = ROOT.TLegend(.67, .60, .89, .88)
            obj.legend.SetMargin(0.12);
            obj.legend.SetTextSize(0.03);
            obj.legend.SetFillColor(10);
            obj.legend.SetBorderSize(0);

            # background combined
            obj.bg_combo = None
            for channel_type in ["mc", "qcd"]:
                if channel_type in channels:
                    hist = channels[channel_type].hist

                    if obj.bg_combo:
                        obj.bg_combo.Add(hist)
                    else:
                        obj.bg_combo = hist.Clone()
                        obj.bg_combo.SetDirectory(0)

            if obj.bg_combo:
                # apply uncertainty style
                MCChannelTemplate.channel_styles["mc"].apply(obj.bg_combo)

            # stack all backgrounds
            obj.bg_stack = None

            bg_order = ["qcd"] + (mc_combo.allowed_inputs if mc_combo else [])
            bg_channels = set(channels.keys()) & set(bg_order)

            # Add channels in order: QCD + channel_type["mc"]
            for channel_type in bg_order:
                if channel_type in bg_channels:
                    hist = channels[channel_type].hist

                    obj.legend.AddEntry(hist, 
                            channel_names.get(channel_type,
                                              "unknown"),
                            "fe")

                    if not obj.bg_stack:
                        obj.bg_stack = ROOT.THStack()

                    obj.bg_stack.Add(hist)

            # Adjust y-Maximum to be drawn
            data_max_bin = data.hist.GetMaximumBin() if data else 0
            bg_max_bin = obj.bg_combo.GetMaximumBin() if obj.bg_combo else 0

            max_y = 1.2 * max([
                (obj.bg_combo.GetBinContent(bg_max_bin) +
                 obj.bg_combo.GetBinError(bg_max_bin)) if obj.bg_combo else 0,

                (data.hist.GetBinContent(data_max_bin) +
                 data.hist.GetBinError(data_max_bin)) if data else 0,
                ])

            # take care of ratio
            if data and obj.bg_combo:
                # Prepare comparison canvas: top pad plots, bottom - ratio
                obj.canvas = ComparisonCanvas()
                canvas = obj.canvas.canvas

                obj.canvas.canvas.cd(2)

                obj.ratio = compare.data_mins_bg_over_bg(data.hist, obj.bg_combo)
                obj.ratio.GetXaxis().SetTitle("")
                obj.ratio.Draw("9 e")

            else:
                obj.canvas = ROOT.TCanvas()
                canvas = obj.canvas
                canvas.SetWindowSize(640, 640)
                pad = canvas.cd(1)
                pad.SetRightMargin(5)
                pad.SetBottomMargin(0.15)

            canvas.cd(1)

            # use data or background to draw axes
            obj.axis_hist = None
            if data:
                obj.axis_hist = data.hist.Clone()
            else:
                obj.axis_hist = obj.bg_combo.Clone()

            obj.axis_hist.Reset()
            for axis in ROOT.TH1.GetXaxis, ROOT.TH1.GetYaxis:
                axis(obj.axis_hist).SetLabelSize(0.04)

            obj.axis_hist.Draw("9")

            # Draw plots
            if obj.bg_stack:
                obj.bg_stack.Draw("9 hist same")

            if obj.bg_combo:
                obj.legend.AddEntry(obj.bg_combo, "Uncertainty", "fe")
                obj.bg_combo.Draw("9 e2 same")

            if data:
                obj.legend.AddEntry(data.hist, "CMS Data 2011", "lpe")
                data.hist.Draw("9 same")

            obj.axis_hist.SetMaximum(max_y)

            obj.labels = [
                    root.label.CMSLabel(),
                    root.label.LuminosityLabel(InputTemplate.luminosity())
                        if data else root.label.CMSSimulationLabel()]

            # Draw Labels and Legend
            for label in obj.labels:
                label.draw()

            obj.legend.Draw("9")

            canvas.Update()

            canvases.append(obj)

        raw_input('enter')

    def __str__(self):
        result = []

        result.append("  use plots: {0}".format(self.use_plots))
        result.append("  ban plots: {0}".format(self.ban_plots))

        result.append("use folders: {0}".format(self.use_folders))
        result.append("ban folders: {0}".format(self.ban_folders))

        result.append("   channels: {0}".format(self.use_channels))

        return '\n'.join(result)
