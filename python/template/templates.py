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
    # Map channel-type to meaningful name for histogram Legend
    channel_names = {
            "qcd": "QCD data-driven",
            "stop": "Single-Top",
            "zjets": "Z/#gamma*#rightarrowl^{+}l^{-}",
            "wjets": "W#rightarrowl#nu",
            "ttbar": "t#bar{t}",
            "mc": "Total MC",

            "zprime_m1000_w10": "Z' 1 TeV/c^{2}",
            "zprime_m1500_w15": "Z' 1.5 TeV/c^{2}",
            "zprime_m2000_w20": "Z' 2 TeV/c^{2}",
            "zprime_m3000_w30": "Z' 3 TeV/c^{2}",
            "zprime_m4000_w40": "Z' 4 TeV/c^{2}"}

    def __init__(self, options, args, disable_systematics=True):
        '''
        Configure Templates object with parsed command-line arguments

        options and args should represent parsed arguments by OptionParser.
        Supported options:

            batch       keep all Canvases open and wait for user to press
                        Enter before exit application if set

            filename    specify input ROOT filename to load all plots from
            
            scales      absolute scale factors for specific channels. If set
                        then corresponding channel histogram will be scaled
                        with specified value

            fractions   relative scale factors for MC, QCD. The scale factor
                        is calculated per plot and equal to:

                            MC_scale * Data.Integral() / MC.Integral()
                            QCD_scale * Data.Integral() / QCD.Integral()

                        therefore Data needs to be loaded

                        Note:   TFractionFitter is automatically turned off if
                                --fractions options is used

            notff       Do not use TFractionFitter

                        Note:   TFractionFitter is automatically turned off if
                                --fractions options is used

            ratio       Plot ratio of specific channels in the comparision
                        plot. Any two channels cha be specified in the ratio.
                        Use 'bg' to reference background which is MC + QCD.

                        Note:   it is user responsibility to make sure
                                corresponding channels are loaded

            plots       list all plot names to be loaded. By default all
                        histograms are loaded from input file. Plot names
                        should be separated with comma. Use '-' to turn
                        specific plot OFF, e.g.:

                            --plots met,mass

                        that is plot met and mass

                            --plots -njets,-mass

                        plot everything except njets and mass

            folders     specify TDirectory(-ies) to load plots from. All
                        subfolders in ROOT file are scanned. Folder names
                        should be separated with comma. Use '-' to exclude
                        specific folders. See --plots comment for example.

                        Note:   only plots that are specified in plots option
                                will be loaded if --plots is used

            channels    list channels to be loaded. All channels are loaded by
                        default. Channel names should be separated with comma.
                        Use '-' to exclude specific channels. See --plots
                        comment for example.

                        Note: 'data' can be used to reference all Data channels
        
        '''

        self._verbose = options.verbose
        self._batch_mode = options.batch        # wait for Enter before exit
        self._input_filename = options.filename # ROOT file to load plots from

        # Absolute scales for specific channels
        if options.scales:
            self._scales = Scales()
            self._scales.load(options.scales)
        else:
            self._scales = None

        # Relative fractions for specific channels
        if options.fractions:
            fractions = Scales()
            fractions.load(options.fractions)

            tmp_fractions = {}
            for fraction_type in ["mc", "qcd"]:
                fraction = fractions.scales.get(fraction_type)
                if not fraction:
                    raise RuntimeError(("fractions file doe not contain {0} "
                                        "fraction").format(
                                            fraction_type.upper()))

                tmp_fractions[fraction_type] = fraction

            self.fractions = tmp_fractions
        else:
            self.fractions = {}

        # disable/enable TFractionFitter. It is enabled by default
        self._use_tfraction_fitter = not options.notff and not self.fractions

        # Use ratio in the comparison plot
        ratio = options.ratio.lower()
        if ratio:
            if "/" in ratio:
                self._ratio = ratio.split('/')
            else:
                self._ratio = None

                print("only simple ratios are supported: channel/channel",
                        file = sys.stderr)
        else:
            self._ratio = None

        self.use_plots = []
        self.ban_plots = []

        if options.plots:
            self.use_plots, self.ban_plots = map(list,
                                                     split_use_and_ban(set(
                    plot.strip() for plot in options.plots.split(','))))

        self.use_folders = []
        self.ban_folders = []

        if options.folders:
            self.use_folders, self.ban_folders = map(list,
                                                     split_use_and_ban(set(
                    folder.strip() for folder in options.folders.split(','))))

        self.use_channels = []

        if options.channels:
            use_channels, ban_channels = split_use_and_ban(set(
                channel.strip() for channel in options.channels.split(',')))

            # use only allowed channels or all if None specified
            if disable_systematics:
                channels = set(channel
                        for channel in channel_type.ChannelType.channel_types.keys()
                            if "matching" not in channel and
                               "scaling" not in channel)
            else:
                channels = set(channel_type.ChannelType.channel_types.keys())

            if use_channels:
                channels &= use_channels

            # remove banned channels
            if ban_channels:
                channels -= ban_channels

            self.use_channels = list(channels)

        self.loader = None

        self._str_format = "{0:>20}: {1}" # nice __str__ format

    def run(self):
        '''
        Entry point: run application
        '''

        # Apply TDR style to all plots
        style = root.style.tdr()
        style.cd()

        # print run configuration
        if self._verbose:
            print("{0:-<80}".format("-- Configuration "))
            print(self)
            print()

        self._process()

    def _process(self):
        if not self.use_channels:
            raise RuntimeError("all channels are turned off")

        self._load_channels()
        self._run_fraction_fitter()
        self._apply_fractions()
        self._apply_scales()

        canvases = self._plot()

        # Save canvases
        for obj in canvases:
            obj.canvas.SaveAs("{0}.pdf".format(obj.canvas.GetName()))

        if canvases and not self._batch_mode:
            raw_input('enter')

    @Timer(label="[load all channels]", verbose=False)
    def _load_channels(self):
        '''
        Load channel templates

        Each available input will be opened and loaded. Some inputs are going
        to be merged into channels depending on channel policy. Consult
        ChannelTemplate class for details

        '''

        if self._verbose:
            print("{0:-<80}".format("-- Load Channels "))

        # Create and configure new loader
        self.loader = ChannelTemplateLoader(self._input_filename)

        self.loader.use_plots = self.use_plots
        self.loader.ban_plots = self.ban_plots

        self.loader.use_folders = self.use_folders
        self.loader.ban_folders = self.ban_folders

        # load channels
        self.loader.load(self.use_channels)

        # print loader summary
        if self._verbose:
            print(self.loader)
            print()

    def _run_fraction_fitter(self):
        '''
        Run TFractionFitter to get QCD, MC fractions that better match data

        '''

        if not self._use_tfraction_fitter:
            return

        try:
            if self._verbose:
                print("{0:-<80}".format("-- TFractionFitter "))

            if ("/met" not in self.loader.plots or
                "/met_noweight" not in self.loader.plots):

                raise RuntimeError("load plots 'met', 'met_noweight'")

            met = self.loader.plots["/met"]
            met_noweight = self.loader.plots["/met_noweight"].get("mc")

            # make sure weighted channels are available
            missing_channels = set(["data", "qcd", "mc"]) - set(met.keys())
            if missing_channels:
                raise RuntimeError("channels {0!r} are not loaded".format(
                    map(str.upper, missing_channels)))

            # check if no-weighted MC is available
            if not met_noweight:
                raise RuntimeError("MC not weighted met plot is not loaded")

            # prepare MC weights for TFraction fitter
            mc_weights = met["mc"].hist.Clone()
            mc_weights.SetDirectory(0)
            mc_weights.Divide(met_noweight.hist)

            # Set any zero bins to at least 1 event
            for bin_ in range(1, mc_weights.GetNbinsX() + 1):
                if 0 >= mc_weights.GetBinContent(bin_):
                    mc_weights.SetBinContent(bin_, 1)

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

            # Extract MC and QCD fractions from TFractionFitter and keep
            # only central values (drop errors)
            fraction = ROOT.Double(0)
            fraction_error = ROOT.Double(0)

            fitter.GetResult(0, fraction, fraction_error)
            self.fractions["mc"] = float(fraction)

            fitter.GetResult(1, fraction, fraction_error)
            self.fractions["qcd"] = float(fraction)

            # plot Fitter result and save canvas
            tff_hist = fitter.GetPlot().Clone()
            qcd_hist = met["qcd"].hist.Clone()
            mc_hist = met["mc"].hist.Clone()
            data_hist = met["data"].hist.Clone()

            qcd_hist.Scale(self.fractions["qcd"] *
                           data_hist.Integral() /
                           qcd_hist.Integral())

            mc_hist.Scale(self.fractions["mc"] *
                          data_hist.Integral() /
                          mc_hist.Integral())

            tff_hist.SetLineStyle(2)
            tff_hist.SetLineColor(33)
            tff_hist.SetLineWidth(5)

            legend = ROOT.TLegend(.67, .60, .89, .88)
            legend.SetMargin(0.12);
            legend.SetTextSize(0.03);
            legend.SetFillColor(10);
            legend.SetBorderSize(0);

            canvas = ROOT.TCanvas("met_fit", "met_fit", 640, 640)
            pad = canvas.cd(1)

            pad.SetLeftMargin(0.2)
            pad.SetBottomMargin(0.15)

            tff_hist.Draw("hist 9")
            data_hist.Draw("e 9 same")
            mc_hist.Draw("hist same 9")
            qcd_hist.Draw("hist same 9")

            legend.AddEntry(data_hist, "Data 2011", "le")
            legend.AddEntry(qcd_hist, "QCD data-driven", "fe")
            legend.AddEntry(mc_hist, "Monte-Carlo", "fe")
            legend.AddEntry(tff_hist, "Fit", "l")

            legend.Draw("9 same")

<<<<<<< HEAD
        # Print found fractions
        if self.__verbose:
            print('\n'.join("{0:>3} Fraction: {1:.3f}".format(
                    key.upper(),
                    value)
                for key, value in self.fractions.items()))
            print()
=======
            canvas.SaveAs("met_fit.pdf")

            # Print found fractions
            if self._verbose:
                print('\n'.join("{0:>3} fraction: {1:.3f}".format(key.upper(),
                                                                  value)
                                for key, value in self.fractions.items()))
>>>>>>> 128bff96f950537429f70f60fa52428751e80b81

        except RuntimeError as error:
            if self._verbose:
                print("failed to use TFractionFitter - {0}".format(error),
                      file = sys.stderr)

        finally:
            if self._verbose:
                print()

    def _apply_fractions(self):
        '''
        Apply dynamic scale for each plot based on specified fractions

        Scales are calculated inidividually for each plot accoring to formulas:

            mc_scale = mc_fraction * Data.Integral() / MC.Integral()
            qcd_scale = qcd_fraction * Data.Integral() / QCD.Integral()

        Therefore, data needs to be loaded

        '''

        if not self.fractions:
            return

        try:
            if self._verbose:
                print("{0:-<80}".format("-- TFractionFitter "))

            mc_fraction = self.fractions["mc"]
            qcd_fraction = self.fractions["qcd"]

            # For each loaded plot scale MC and QCD
            for plot, channels in self.loader.plots.items():
                try:
                    # Make sure all necessary channels were loaded for the
                    # histogram
                    missing_channels = (set(["data", "mc", "qcd"]) -
                                        set(channels.keys()))
                    if missing_channels:
                        raise RuntimeError(("channels {0!r} are not loaded for "
                                            "{1} template").format(
                                                map(str.upper,
                                                    missing_channels),
                                                plot))

                    # Cache 
                    data_integral = channels["data"].hist.Integral()
                    mc_hist = channels["mc"].hist
                    qcd_hist = channels["qcd"].hist

                    qcd_scale = (qcd_fraction *
                                 data_integral /
                                 qcd_hist.Integral())

                    qcd_hist.Scale(qcd_scale)

                    mc_scale = (mc_fraction *
                                data_integral /
                                mc_hist.Integral())

                    mc_hist.Scale(mc_scale)

                    # scale each MC channel individually
                    loaded_mc_channels = (set(channels["mc"].allowed_inputs) &
                                          set(channels.keys()))
                    for channel_type in loaded_mc_channels:
                        channels[channel_type].hist.Scale(mc_scale)

                    if self._verbose and "/mttbar_after_htlep" == plot:
                        print(" mttbar scales",
                              " MC: {0:.2f}".format(mc_scale),
                              "QCD: {0:.2f}".format(qcd_scale),
                              "", sep = "\n")

                except RuntimeError as error:
                    print("failed to apply TFractionFitter scales - {0}".format(error),
                          file = sys.stderr)

        except RuntimeError as error:
            if self._verbose:
                print("failed to apply fractions - {0}".format(error),
                      file = sys.stderr)

        finally:
            if self._verbose:
                print()

    def _apply_scales(self):
        if not self._scales:
            return

        if self._verbose:
            print("{0:-<80}".format("-- Scales "))

        # For each loaded plot/channel apply loaded scale if channel type
        # matches scale type
        for plot, channels in self.loader.plots.items():
            # special treatment for MC background(s)
            if "mc" in self._scales.scales and "mc" in channels:
                mc_channels = (set(channels["mc"].allowed_inputs) &
                               set(channels.keys()))
                mc_scale = self._scales.scales["mc"]
            else:
                mc_channels = set()
                mc_scale = 0

            for channel_type, channel in channels.items():
                if channel_type in self._scales.scales:
                    scale = self._scales.scales[channel_type]
                elif channel_type in mc_channels:
                    scale = mc_scale
                else:
                    continue

                if "/mttbar_after_htlep" == plot:
                    print("scale {0} by {1:.2f}".format(channel_type, scale))

                channel.hist.Scale(scale)

        if self._verbose:
            print()

    @Timer(label = "[plot templates]", verbose = True)
    def _plot(self):
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
            if self._ratio:
                try:
                    # make sure specified channels are available
                    ratio = []
                    for term in self._ratio:
                        if "bg" == term:
                            if not obj.bg_combo:
                                raise KeyError("background is not loaded")

                            ratio.append({
                                "title": "BKGD",
                                "hist": obj.bg_combo
                                    })

                        elif term in channels:
                            ratio.append({
                                "title": self.channel_names.get(term, "unknown"),
                                "hist": channels[term].hist
                                })
                        else:
                            raise KeyError("unsupported channel {0!r}".format(
                                term))

                    obj.canvas_obj = ComparisonCanvas()
                    obj.canvas = obj.canvas_obj.canvas

                    obj.canvas.cd(2)

                    obj.ratio = compare.ratio(ratio[0]["hist"],
                            ratio[1]["hist"],
                            title = "#frac{" + ratio[0]["title"] + "}{"
                                    + ratio[1]["title"] + "}")

                    obj.ratio.GetXaxis().SetTitle("")
                    obj.ratio.Draw("9 e")

                except KeyError as error:
                    print("ratio error: {0}".format(error))

                    obj.canvas = ROOT.TCanvas()
                    obj.canvas.SetWindowSize(640, 640)
                    pad = obj.canvas.cd(1)
                    pad.SetRightMargin(5)
                    pad.SetBottomMargin(0.15)
                
            elif data and obj.bg_combo:
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
        '''
        Print Templates object configuraiton
        '''

        result = []

        result.append(["verbose", self._verbose])
        result.append(["batch mode", self._batch_mode])
        result.append(["input filename", self._input_filename])
        result.append(["scales", self._scales if self._scales else ""])

        result.append(["fractions",
                       self.fractions if self.fractions else ""])

        result.append(["TFractionFitter",
                       "on" if self._use_tfraction_fitter else "off"])

        result.append(["ratio",
                       self._ratio if self._ratio else "default"])

        result.append(["use plots", self.use_plots])
        result.append(["ban plots", self.ban_plots])
        result.append(["use folders", self.use_folders])
        result.append(["ban folders", self.ban_folders])
        result.append(["channels", self.use_channels])

        return '\n'.join(self._str_format.format(name, value)
                         for name, value in result)
