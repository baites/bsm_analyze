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

        channel = plot_name.split('_')[1]
        plots["bkgd" if "mc" == channel else channel] = h

    integrals = {x: integral(y, start) for x, y in plots.items()}

    print("-- Integral ".ljust(80, '-'))
    for k, v in integrals.items():
        print("{0:<10} {1:<.1f}".format(k, v.Integral()))
    print()

    legend = TLegend(.8, .3, .9, .5)
    legend.SetMargin(0.12)  
    legend.SetTextSize(0.03)
    legend.SetFillColor(10) 
    legend.SetBorderSize(0) 

    stack = THStack()
    for k, v in sorted(integrals.items(), reverse = True):
        color = ROOT.kBlack if "data" == k else ROOT.kRed
        v.SetMarkerColor(color)
        v.SetLineColor(color)

        stack.Add(v)
        legend.AddEntry(v, k, "l")

    ratio = divide(numerator = integrals["data"], denominator = integrals["bkgd"])

    canvas = TCanvas()
    canvas.SetWindowSize(640, 480)
    canvas.Divide(1, 2)

    pad = canvas.cd(1)
    pad.SetPad(0, 0.3, 1, 1)
    pad.SetTopMargin(5)
    pad.SetRightMargin(5)

    stack.Draw("nostack 9")

    stack.GetHistogram().GetXaxis().SetTitle(integrals["data"].GetXaxis().GetTitle())
    stack.GetHistogram().GetYaxis().SetTitle("#int_{0}^{x} M_{t#bar{t}}")
    stack.GetHistogram().GetYaxis().SetTitleSize(0.03)
    stack.GetHistogram().GetYaxis().SetTitleOffset(2)
    stack.GetHistogram().GetYaxis().SetRangeUser(0, 500)
    TGaxis.SetMaxDigits(2)

    legend.Draw("9")

    pad = canvas.cd(2)
    pad.SetPad(0, 0, 1, 0.3)
    pad.SetGrid()
    pad.SetBottomMargin(0.3)
    pad.SetRightMargin(5)

    ratio.Draw("e 9")

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

def divide(*parg, **karg):
    if parg:
        raise Exception("positional arguments are not supported by ratio")

    numerator = karg.pop("numerator", None)
    denominator = karg.pop("denominator", None)

    if karg:
        raise Exception("unexpected positional arguments in divide function")

    if not numerator:
        raise Exception("numerator is not supplied")

    if not denominator:
        raise Exception("denominator is not supplied")

    h = numerator.Clone()
    h.Reset()
    h.GetYaxis().SetTitle("#frac{Data}{BKGD}")
    h.GetYaxis().SetTitleSize(0.08)
    h.GetYaxis().SetTitleOffset(0.6)
    h.GetYaxis().SetLabelSize(0.09)
    h.GetYaxis().SetNdivisions(4)
    h.GetXaxis().SetLabelSize(0.09)
    h.GetXaxis().SetTitleSize(0.1)
    h.SetMarkerSize(0.5)
    h.SetLineWidth(1)
    h.SetLineColor(ROOT.kGray + 2)

    max_ratio = 0
    for bin in range(1, numerator.GetXaxis().GetNbins() + 2):
        top = numerator.GetBinContent(bin)
        bot = denominator.GetBinContent(bin)

        top_err = numerator.GetBinError(bin)
        bot_err = denominator.GetBinError(bin)

        ratio = top / bot if bot else 0

        if ratio > max_ratio:
            max_ratio = ratio

        h.SetBinContent(bin, ratio)
        h.SetBinError(bin, sqrt((ratio * bot_err) ** 2 + top_err ** 2) / bot_err ** 2 if bot else 0)

    h.SetMaximum(max_ratio)
    h.GetYaxis().SetRangeUser(0, int(max_ratio * 1.5))

    return h

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
