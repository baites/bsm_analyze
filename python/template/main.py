#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 21, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function

import sys

from input_template import InputTemplate
from channel_type import ChannelType
from channel_template import ChannelTemplate, MCChannelTemplate

import root.style
import root.label

from root.comparison_canvas import ComparisonCanvas, compare

from loader import ChannelTemplateLoader

import ROOT

@compare
def ratio(data, background, title = None):
    ratio = data.Clone()
    ratio.SetDirectory(0)
    ratio.Reset()

    ratio.Divide(data, background)
    ratio.GetYaxis().SetTitle(title if title else "#frac{Data}{BKGD}")

    return ratio

@compare
def data_mins_bg_over_bg(data, background):
    h = data.Clone()
    h.SetDirectory(0)
    h.Add(background, -1)
    h.Divide(background)
    h.GetYaxis().SetTitle("#frac{Data - BKGD}{BKGD}")

    return h

def usage(argv):
    return "usage: {0} templates.cfg".format(argv[0])

def main(argv = sys.argv):
    try:
        if 2 > len(argv):
            raise RuntimeError(usage(argv))

        style = root.style.tdr()
        #style.SetOptStat(111111)
        style.cd()

        #folders_to_use = ["", "htop", "ltop"]
        #plots_to_load = ["mttbar_after_htlep", "mass", "mt"]
        folders_to_use = [""]
        plots_to_load = ["mttbar_after_htlep"]
        #folders_to_use = ["", "htop", "ltop"]
        #plots_to_load = ["mttbar_after_htlep", "mass", "pt"]

        # key: template.name    value: list of channels
        plots = {}
        combo_plots = {}

        loader = ChannelTemplateLoader("output_signal_p150_hlt.root")
        loader.use_folders.extend(folders_to_use)
        loader.use_plots.extend(plots_to_load)
        loader.load([
            "qcd", "data",

            "ttbar", "zjets", "wjets", "stop",

            "zprime_m1000_w10",
            "zprime_m1500_w15",
            "zprime_m2000_w20",
            "zprime_m3000_w30",
                  ])

        for k, v in loader.plots.items():
            print("{0:-<80}".format("-- {0} ".format(k)))
            
            for c in v:
                print("{0:>20}: {1}".format(c.type, [x.type for x in c.input_templates]))

        return 0

        for k, c in combo_plots.items():
            print("{0:-<80}".format("-- {0} ".format(k)))
            
            print("{0:>20}: {1}".format(c.type, [x.type for x in c.input_templates]))

        canvases = []
        for plot, channels in plots.items():
            obj = {}

            canvases = []

            comparison = ComparisonCanvas()
            canvases.append(comparison)

            comparison_zprime1000 = ComparisonCanvas();
            canvases.append(comparison_zprime1000)

            comparison_zprime1500 = ComparisonCanvas();
            canvases.append(comparison_zprime1500)

            comparison_zprime2000 = ComparisonCanvas();
            canvases.append(comparison_zprime2000)

            comparison_zprime3000 = ComparisonCanvas();
            canvases.append(comparison_zprime3000)

            obj["canvases"] = canvases

            data_channel = None
            combo_channel = combo_plots.get(plot)

            stack = ROOT.THStack()
            obj["stack"] = stack

            legend = ROOT.TLegend(.67, .60, .89, .88)
            legend.SetMargin(0.12);
            legend.SetTextSize(0.03);
            legend.SetFillColor(10);
            legend.SetBorderSize(0);
            obj["legend"] = legend

            zprime_channels = {}
            for channel in channels:
                if channel.type in combo_channel.allowed_inputs:
                    stack.Add(channel.hist)
                    legend.AddEntry(channel.hist, channel.type, 'fe')
                elif 'data' == channel.type:
                    data_channel = channel
                    legend.AddEntry(channel.hist, channel.type, 'lpe')
                elif channel.type in ['zprime_m1000_w10',
                                      'zprime_m1500_w15',
                                      'zprime_m2000_w20',
                                      'zprime_m3000_w30',
                                      'zprime_m4000_w40']:
                    zprime_channels[channel.type] = channel
                    legend.AddEntry(channel.hist, channel.type, "lpe")

            for c in canvases:
                c.canvas.cd(1)

                stack.Draw("9 hist")
                combo_channel.hist.Draw("9 e2 same")
                legend.Draw("9")

                if data_channel and data_channel.hist:
                    data_channel.hist.Draw("9 same")

                for zprime in zprime_channels.values():
                    zprime.hist.Draw("9 same")

                labels = [root.label.CMSLabel(),
                          root.label.LuminosityLabel(InputTemplate.luminosity())]
                for x in labels:
                    x.draw()
                c.labels = labels

            comparison.canvas.cd(2)
            if data_channel.hist and combo_channel.hist:
                ratio_plot = data_mins_bg_over_bg(data_channel.hist, combo_channel.hist)
                ratio_plot.GetYaxis().SetRangeUser(-1, 1)
                ratio_plot.Draw("e 9")

                comparison.ratio_plot = ratio_plot

            if combo_channel.hist:
                for c, (ch, t) in {
                        comparison_zprime1000: ['zprime_m1000_w10', "Z' 1 TeV"],
                        comparison_zprime1500: ['zprime_m1500_w15', "Z' 1.5 TeV"],
                        comparison_zprime2000: ['zprime_m2000_w20', "Z' 2 TeV"],
                        comparison_zprime3000: ['zprime_m3000_w30', "Z' 3 TeV"],
                        }.items():
                    c.canvas.cd(2)
                    zprime_channel = zprime_channels.get(ch)
                    if zprime_channel:
                        ratio_plot = ratio(zprime_channel.hist,
                                           combo_channel.hist,
                                           "#frac{" + t + "}{BKGD}")
                        ratio_plot.Draw("e 9")
                        c.ratio_plot = ratio_plot

            for c in canvases:
                c.canvas.Update()

            canvases.append(obj)

        raw_input("enter")

    except Exception as error:
        import traceback

        traceback.print_tb(sys.exc_info()[2])
        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
