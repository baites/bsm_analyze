#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Mar 13, 2012
Copyright 2011, All rights reserved
'''

from __future__ import division, print_function

import root.style
import template.compare as compare
from template.templates import Templates
from root.comparison import ComparisonCanvas
import root.label
from systematics.loader import SystematicLoader
from util.arg import split_use_and_ban
import ROOT

class Comparator(object):
    channels = set([
        "ttbar", "zjets", "wjets", "stop",
        "zprime_m1000_w10",
        "zprime_m1500_w15",
        "zprime_m2000_w20",
        "zprime_m3000_w30"
    ])

    def __init__(self, options, args):
        self._batch_mode = options.batch
        self._verbose = options.verbose

        self._input_filename = options.filename

        self.use_channels = []

        self._systematic = options.systematic
        if self._systematic:
            if self._systematic.endswith("+"):
                self._load_systematic = { "plus": True, "minus": False}

            elif self._systematic.endswith("-"):
                self._load_systematic = { "plus": False, "minus": True}

            else:
                self._load_systematic = None

            self._systematic = self._systematic.rstrip("+-")

        if options.channels:
            use_channels, ban_channels = split_use_and_ban(set(
                channel.strip() for channel in options.channels.split(',')))

            # use only allowed channels or all if None specified
            channels = self.channels
            if use_channels:
                channels &= use_channels

            # remove banned channels
            if ban_channels:
                channels -= ban_channels

            self.use_channels = list(channels)
        else:
            self.use_channels = self.channels

        self.loader = None

    def run(self):
        if not self._systematic:
            raise RuntimeError("systematic is not specified")

        # Apply TDR style to all plots
        style = root.style.tdr()
        style.cd()

        # print run configuration
        if self._verbose:
            print("{0:-<80}".format("-- Configuration "))
            print(self)
            print()

        self._load()
        self._plot()

    def _load(self):
        if self._verbose:
            print("{0:-<80}".format("-- Load Systematics "))

        loader = {
                "jes": SystematicLoader,
                "pileup": SystematicLoader
                }.get(self._systematic)

        if not loader:
            raise RuntimeError("unsupported systematic type")

        self.loader = loader(self._input_filename,
                             self.use_channels,
                             self._systematic,
                             self._load_systematic)

        self.loader.load()

        if self._verbose:
            print(self.loader)
            print()

    def _plot(self):
        if not self.loader:
            raise RuntimeError("templates are not loaded")

        class Container: pass
        containers = []

        for plot, channels in self.loader.plots.items():
            for channel, systematics in channels.items():
                obj = Container()
                obj.ratios = []
                obj.stack = ROOT.THStack()
                obj.axis_hist = None

                obj.legend = ROOT.TLegend(.67, .60, .89, .88)
                obj.legend.SetMargin(0.12);
                obj.legend.SetTextSize(0.03);
                obj.legend.SetFillColor(10);
                obj.legend.SetBorderSize(0);
                obj.legend.SetHeader("{0}: {1}".format(
                    self._systematic.capitalize(),
                    Templates.channel_names.get(
                        channel, "Unknown Channel")))

                self.style(systematics)

                max_y = 0
                for channel in (systematics.nominal,
                                systematics.plus,
                                systematics.minus):
                    if not channel:
                        continue

                    if not obj.axis_hist:
                        obj.axis_hist = channel.hist.Clone()

                    channel_max_y = channel.hist.GetBinContent(
                            channel.hist.GetMaximumBin())

                    if channel_max_y > max_y:
                        max_y = channel_max_y

                obj.axis_hist.Reset()
                obj.axis_hist.SetMaximum(1.2 * max_y)

                if systematics.nominal:
                    obj.stack.Add(systematics.nominal.hist)
                    obj.legend.AddEntry(systematics.nominal.hist,
                                        "nominal",
                                        "lp")

                if systematics.plus:
                    obj.stack.Add(systematics.plus.hist)
                    obj.legend.AddEntry(systematics.plus.hist,
                                        "systematic plus",
                                        "lp")

                    if systematics.nominal:
                        obj.ratios.append(compare.ratio(
                                systematics.plus.hist,
                                systematics.nominal.hist,
                                title="#frac{plus}{nominal}"))

                if systematics.minus:
                    obj.stack.Add(systematics.minus.hist)
                    obj.legend.AddEntry(systematics.minus.hist,
                                        "systematic minus",
                                        "lp")

                    if systematics.nominal:
                        obj.ratios.append(compare.ratio(
                                systematics.minus.hist,
                                systematics.nominal.hist,
                                title="#frac{minus}{nominal}"))

                obj.canvas = ComparisonCanvas(len(obj.ratios) + 1)
                canvas = obj.canvas.canvas

                canvas.cd(1)

                obj.axis_hist.Draw("9")
                obj.stack.Draw("hist 9 nostack same")

                obj.labels = [root.label.CMSSimulationLabel()]

                for label in obj.labels:
                    label.draw()

                obj.legend.Draw("9")

                for pad, ratio in enumerate(obj.ratios, 2):
                    canvas.cd(pad)

                    ratio.GetYaxis().SetRangeUser(0, 2)
                    ratio.GetXaxis().SetTitle("")
                    ratio.Draw("9 e")

                canvas.Update()

                containers.append(obj)

        if containers and  not self._batch_mode:
            raw_input('enter')

    def style(self, systematics):
        for channel in systematics.nominal, systematics.plus, systematics.minus:
            if not channel:
                continue

            plot = channel.hist

            plot.SetFillStyle(0)
            plot.SetFillColor(0)

            if channel == systematics.nominal:
                plot.SetLineColor(ROOT.kBlack)
                plot.SetLineStyle(1)
            else:
                plot.SetLineStyle(2)

                if channel == systematics.plus:
                    plot.SetLineColor(ROOT.kRed + 1)
                elif channel == systematics.minus:
                    plot.SetLineColor(ROOT.kGreen + 1)

    def __str__(self):
        result = []

        result.append(["verbose", self._verbose])
        result.append(["batch mode", self._batch_mode])
        result.append(["input filename", self._input_filename])
        result.append(["channels", self.use_channels])

        return '\n'.join("{0:>15}: {1}".format(name, value)
                         for name, value in result)
