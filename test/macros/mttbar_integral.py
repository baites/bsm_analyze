#!/usr/bin/env python
#
# Created by Samvel Khalatyan, Jan 17, 2012
# Copyright 2012, All rights reserved

from __future__ import division, print_function

import os
import sys

from ROOT import *

def rootStyle(filename):
    if os.path.isfile(filename):
        gROOT.ProcessLine(".L {0}".format(filename))
        ROOT.setTDRStyle()

        print("Loaded ROOT style from: " + filename)
    else:
        print("ROOT style is not available: " + filename)

def generateCDF(filename, start = 0):
    start = float(start)

    if not os.path.lexists(filename):
        raise Exception("input file does not exist: " + filename)

    in_file = TFile(filename)
    if in_file.IsZombie():
        raise Exception("failed to open input: " + filename)

    plots = {}
    for plot_name in "mttbar_mc", "mttbar_data":
        h = in_file.Get(plot_name)
        if not h:
            raise Exception("filed to extract histogram: " + plot_name)

        plots[plot_name.split('_')[1]] = h

    integrals = {x: integral(y, start) for x, y in plots.items()}

    legend = TLegend(.8, .3, .9, .5)
    legend.SetMargin(0.12)  
    legend.SetTextSize(0.03)
    legend.SetFillColor(10) 
    legend.SetBorderSize(0) 

    stack = THStack()
    for k, v in integrals.items():
        color = ROOT.kBlack if "data" == k else ROOT.kRed
        v.SetMarkerColor(color)
        v.SetLineColor(color)

        stack.Add(v)
        legend.AddEntry(v, k, "l")

    canvas = TCanvas()
    canvas.SetWindowSize(640, 480)
    canvas.cd().SetTopMargin(5)

    stack.Draw("nostack 9")

    stack.GetHistogram().GetXaxis().SetTitle(integrals["mc"].GetXaxis().GetTitle())
    stack.GetHistogram().GetYaxis().SetTitle("#int_{0}^{x} M_{t#bar{t}}")
    stack.GetHistogram().GetYaxis().SetTitleSize(0.03)
    stack.GetHistogram().GetYaxis().SetTitleOffset(2)
    stack.GetHistogram().GetYaxis().SetRangeUser(0, 500)
    TGaxis.SetMaxDigits(2)

    legend.Draw("9")

    canvas.Update()

    canvas.SaveAs("mttbar_integral.pdf")
    canvas.SaveAs("mttbar_integral.png")

def integral(h, start):
    plot = h.Clone()
    plot.Reset()

    start_bin = plot.GetXaxis().FindBin(start)
    integral, variance = 0, 0
    
    for bin in range(start_bin, plot.GetXaxis().GetNbins() + 2):
        integral += h.GetBinContent(bin)
        variance += h.GetBinError(bin) ** 2

        plot.SetBinContent(bin, integral)
        plot.SetBinError(bin, sqrt(variance))

    plot.SetMaximum(integral * 1.2)

    return plot

def usage(argv):
    return "usage: {0} mttbar.root [start:mass]".format(argv[0])

def main(argv = sys.argv):
    try:
        if 2 > len(argv):
            raise Exception(usage(argv))

        rootStyle("tdrstyle.C")

        generateCDF([x for x in argv[1:] if ':' not in x][0],
                    **dict((x.split(':') for x in argv[1:] if ':' in x)))

    except Exception as error:
        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
