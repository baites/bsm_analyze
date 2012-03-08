#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 28, 2012
Copyright 2011, All rights reserved
'''

from __future__ import division, print_function

import compare
import channel_type
import root.label
import root.style
import ROOT
import sys

from channel_template import MCChannelTemplate
from input_template import InputTemplate
from loader import ChannelTemplateLoader
from root.comparison import ComparisonCanvas
from scales import Scales
from util.arg import split_use_and_ban
from util.timer import Timer

class Templates(object):
    channel_names = {
            "qcd": "QCD data-driven",
            "stop": "Single-Top",
            "zjets": "Z/#gamma*#rightarrowl^{+}l^{-}",
            "wjets": "W#rightarrowl#nu",
            "ttbar": "t#bar{t}",

            "zprime_m1000_w10": "Z' 1 TeV/c^{2}",
            "zprime_m1500_w15": "Z' 1.5 TeV/c^{2}",
            "zprime_m2000_w20": "Z' 2 TeV/c^{2}",
            "zprime_m3000_w30": "Z' 3 TeV/c^{2}",
            "zprime_m4000_w40": "Z' 4 TeV/c^{2}"
    }

    def __init__(self):
        self.__verbose = False
        self.__batch_mode = False
        self.__input_filename = "output_signal_p150_hlt.root"
        self.__scales = None

        self.use_plots = []
        self.ban_plots = []

        self.use_folders = []
        self.ban_folders = []

        self.use_channels = []

        self.loader = None
        self.fractions = dict.fromkeys(["mc", "qcd"])
        self.scales = None

    def run(self, options, args):
        # Apply TDR style to all plots
        style = root.style.tdr()
        style.cd()

        self.__verbose = options.verbose
        self.__batch_mode = options.batch
        self.__input_filename = options.filename
        if options.scales:
            self.__scales = Scales()
            self.__scales.load(options.scales)

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
        self.__apply_scales()

        canvases = self.__plot()

        # Save canvases
        for obj in canvases:
            obj.canvas.SaveAs("{0}.pdf".format(obj.canvas.GetName()))

        if canvases and not self.__batch_mode:
            raw_input('enter')

    @Timer(label = "[load all channels]", verbose = True)
    def __load_channels(self):
        # Create and configure new loader
        self.loader = ChannelTemplateLoader(self.__input_filename)

        self.loader.use_plots = self.use_plots
        self.loader.ban_plots = self.ban_plots

        self.loader.use_folders = self.use_folders
        self.loader.ban_folders = self.ban_folders

        # load channels
        self.loader.load(self.use_channels)

        # print loader summary
        if self.__verbose:
            print(self.loader)

    @Timer(label = "[fraction fitter]", verbose = True)
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
        fraction = ROOT.Double(0)
        fraction_error = ROOT.Double(0)

        fitter.GetResult(0, fraction, fraction_error)
        self.fractions["mc"] = map(float, [fraction, fraction_error])

        fitter.GetResult(1, fraction, fraction_error)
        self.fractions["qcd"] = map(float, [fraction, fraction_error])

        fitter_plot = fitter.GetPlot().Clone()
        qcd = met["qcd"].hist.Clone()
        mc = met["mc"].hist.Clone()
        data = met["data"].hist.Clone()

        qcd.Scale(self.fractions["qcd"][0] * data.Integral() / qcd.Integral())
        mc.Scale(self.fractions["mc"][0] * data.Integral() / mc.Integral())

        fitter_plot.SetLineStyle(2)
        fitter_plot.SetLineColor(33)
        fitter_plot.SetLineWidth(5)

        legend = ROOT.TLegend(.67, .60, .89, .88)
        legend.SetMargin(0.12);
        legend.SetTextSize(0.03);
        legend.SetFillColor(10);
        legend.SetBorderSize(0);

        canvas = ROOT.TCanvas("met_fit", "met_fit", 640, 480)
        pad = canvas.cd(1)

        pad.SetLeftMargin(0.2)
        pad.SetBottomMargin(0.15)

        fitter_plot.Draw("hist 9")
        data.Draw("e 9 same")
        mc.Draw("hist same 9")
        qcd.Draw("hist same 9")

        legend.AddEntry(data, "Data 2011", "le")
        legend.AddEntry(qcd, "QCD data-driven", "fe")
        legend.AddEntry(mc, "Monte-Carlo", "fe")
        legend.AddEntry(fitter_plot, "Fit", "l")

        legend.Draw("9 same")

        canvas.SaveAs("met_fit.pdf")


        # Print found fractions
        if self.__verbose:
            print('\n'.join("{0:>3} Fraction: {1:.3f}".format(
                    key.upper(),
                    value[0])
                for key, value in self.fractions.items()))
            print()

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

                qcd_scale = (qcd_fraction * data_integral /
                             qcd_channel.hist.Integral())

                qcd_channel.hist.Scale(qcd_scale)

                mc_scale = (mc_fraction * data_integral /
                            mc_channel.hist.Integral())

                mc_channel.hist.Scale(mc_scale)

                # scale those MC channels that were loaded
                for channel_type in mc_channel.allowed_inputs:
                    channel = channels.get(channel_type)
                    if channel:
                        channel.hist.Scale(mc_scale)

                if "/mttbar_after_htlep" == plot:
                    print("{0:-<80}".format("-- [MTTBAR scales] "),
                          "MC : {0:.2f}".format(mc_scale),
                          "QCD: {0:.2f}".format(qcd_scale),
                          "", sep = "\n")

            except RuntimeError as error:
                print("failed to apply TFractionFitter scales - {0}".format(error),
                      file = sys.stderr)

    def __apply_scales(self):
        if not self.__scales:
            return

        # For each loaded plot/channel apply loaded scale if channel type
        # matches scale type
        for plot, channels in self.loader.plots.items():
            for channel_type, channel in channels.items():
                scale = self.__scales.scales.get(channel_type)
                if scale:
                    channel.hist.Scale(scale)

    @Timer(label = "[plot templates]", verbose = True)
    def __plot(self):
        # container where all canvas related objects will be saved
        class Canvas: pass

        canvases = []

        '''
        loop over plots and draw them:

            1. all background channels stacked (MC channels + QCD)
            2. background error band (MC + QCD)
            3. data with errors
            4. ratio of data over background
        '''

        # loop over plots
        for plot_name, channels in self.loader.plots.items():
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
                            self.channel_names.get(channel_type,
                                              "unknown"),
                            "fe")

                    if not obj.bg_stack:
                        obj.bg_stack = ROOT.THStack()

                    obj.bg_stack.Add(hist)

            # Adjust y-Maximum to be drawn
            max_y = 1.2 * max(
                (h.GetBinContent(h.GetMaximumBin()) +
                 h.GetBinError(h.GetMaximumBin())) if h else 0

                for h in [obj.bg_combo if obj.bg_combo else None,
                          data.hist if data else None] +
                         [ch.hist for name, ch in channels.items()
                             if name.startswith("zprime")] if h)

            # take care of ratio
            if data and obj.bg_combo:
            #if "zprime_m1000_w10" in channels and obj.bg_combo:
                # Prepare comparison canvas: top pad plots, bottom - ratio
                obj.canvas_obj = ComparisonCanvas()
                obj.canvas = obj.canvas_obj.canvas

                obj.canvas.cd(2)

                obj.ratio = compare.data_mins_bg_over_bg(data.hist, obj.bg_combo)
                '''
                obj.ratio = compare.ratio(channels["zprime_m1000_w10"].hist,
                        obj.bg_combo,
                        title = "#frac{Z' 1 TeV}{BKGD}")
                '''
                obj.ratio.GetXaxis().SetTitle("")
                obj.ratio.Draw("9 e")

            else:
                obj.canvas = ROOT.TCanvas()
                obj.canvas.SetWindowSize(640, 640)
                pad = obj.canvas.cd(1)
                pad.SetRightMargin(5)
                pad.SetBottomMargin(0.15)

            obj.canvas.SetName("canvas_" + plot_name.replace("/", "_"))
            obj.canvas.cd(1)

            # use data or background to draw axes
            obj.axis_hist = None
            if data:
                obj.axis_hist = data.hist.Clone()
            elif obj.bg_combo:
                obj.axis_hist = obj.bg_combo.Clone()
            else:
                for name, channel in channels.items():
                    if name.startswith("zprime"):
                        obj.axis_hist = channel.hist.Clone()
                        break

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

            # draw signals
            for channel_type, channel in channels.items():
                if channel_type.startswith("zprime"):
                    obj.legend.AddEntry(channel.hist,
                            self.channel_names.get(channel_type, "unknown signal"),
                            "l")
                    channel.hist.Draw("9 hist same")

            # Draw Labels and Legend
            for label in obj.labels:
                label.draw()

            obj.legend.Draw("9")

            obj.canvas.Update()

            canvases.append(obj)

        return canvases

    def __str__(self):
        result = []

        result.append("  use plots: {0}".format(self.use_plots))
        result.append("  ban plots: {0}".format(self.ban_plots))

        result.append("use folders: {0}".format(self.use_folders))
        result.append("ban folders: {0}".format(self.ban_folders))

        result.append("   channels: {0}".format(self.use_channels))

        return '\n'.join(result)
