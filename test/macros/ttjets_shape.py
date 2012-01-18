#!/usr/bin/env python
#
# Created by Samvel Khalatyan, Jan 12, 2012
# Copyright 2012, All rights reserved

from __future__ import division, print_function

import os
import sys

from ROOT import *

def rootStyle(filename):
    if os.path.isfile(filename):
        gROOT.ProcessLine(".L {0}".format(filename))
        ROOT.setTDRStyle()

        print("Loaded ROOT style from: {0}".format(filename))

def usage(argv):
    return ("usage: {0} powheg:theta_input.root "
            "madgraph:theta_input.root").format(argv[0])

def compare(powheg = None, madgraph = None):
    if not powheg:
        raise Exception("powheg file is not specified")

    if not madgraph:
        raise Exception("madgraph file is not specified")

    if not os.path.exists(powheg):
        raise Exception("powheg file does not exist: {0}".format(powheg))

    if not os.path.exists(madgraph):
        raise Exception("madgraph file does not exist: {0}".format(madgraph))

    powheg_in = TFile(powheg)
    if powheg_in.IsZombie():
        raise Exception("failed to open powheg file: {0}".format(powheg))

    madgraph_in = TFile(madgraph)
    if madgraph_in.IsZombie():
        raise Exception("failed to open madgraph file: {0}".format(madgraph))

    plot_name = "el_mttbar__ttbar"
    powheg_h = powheg_in.Get(plot_name)
    if not powheg_h:
        raise Exception(("failed to extract {0} "
                         "from powheg file: {1}").format(plot_name, powheg))

    madgraph_h = madgraph_in.Get(plot_name)
    if not madgraph_h:
        raise Exception(("failed to extract {0} "
                         "from madgraph file: {1}").format(plot_name, madgraph))

    legend = TLegend(.5, .65, .85, .88)
    legend.SetMargin(0.12)  
    legend.SetTextSize(0.03)
    legend.SetFillColor(10) 
    legend.SetBorderSize(0) 

    legend.SetHeader("TTjets")
    legend.AddEntry(powheg_h, "POWHEG", "l")
    legend.AddEntry(madgraph_h, "MADGRAPH", "l")

    stack = THStack()
    for h in powheg_h, madgraph_h:
        h.Rebin(100)
        h.SetLineWidth(2)
        h.Scale(1 / h.Integral())
        h.SetLineColor((ROOT.kGreen if madgraph_h == h else ROOT.kRed) + 1)
        stack.Add(h)

    ratio = powheg_h.Clone()
    ratio.GetYaxis().SetRangeUser(0, 4)
    ratio.GetYaxis().SetTitle("#frac{powheg}{madgraph}")
    ratio.GetYaxis().SetTitleSize(0.08)
    ratio.GetYaxis().SetTitleOffset(0.6)
    ratio.GetYaxis().SetLabelSize(0.09)
    ratio.GetYaxis().SetNdivisions(4)
    ratio.GetXaxis().SetLabelSize(0.09)
    ratio.GetXaxis().SetTitleSize(0.1)
    ratio.SetMarkerSize(0.5)
    ratio.SetLineWidth(1)
    ratio.SetLineColor(ROOT.kGray + 2)
    for bin in range(1, ratio.GetXaxis().GetNbins() + 1):
        madgraph_bin = madgraph_h.GetBinContent(bin)
        powheg_bin = powheg_h.GetBinContent(bin)

        madgraph_bin_error = madgraph_h.GetBinError(bin)
        powheg_bin_error = powheg_h.GetBinError(bin)

        bin_ratio = powheg_bin / madgraph_bin if madgraph_bin else 0
        ratio.SetBinContent(bin, bin_ratio)
        ratio.SetBinError(bin, sqrt((bin_ratio ** 2 * madgraph_bin_error ** 2 + powheg_bin_error ** 2) / madgraph_bin_error ** 2) if madgraph_bin else 0)

    canvas = TCanvas()

    canvas.SetTitle("TTjets: Madgraph vs Powheg")
    canvas.SetWindowSize(480, 640)
    canvas.Divide(1, 2)

    pad = canvas.cd(1)
    pad.SetPad(0, 0.3, 1, 1)
    pad.SetLeftMargin(0.15)

    stack.Draw("hist nostack 9")
    stack.GetHistogram().GetXaxis().SetTitle(madgraph_h.GetXaxis().GetTitle())
    stack.GetHistogram().GetYaxis().SetTitle("a.u.")
    stack.GetYaxis().SetTitleSize(0.08)
    stack.GetYaxis().SetTitleOffset(0.8)
    stack.GetYaxis().SetLabelSize(0.04)
    stack.GetXaxis().SetLabelSize(0.04)
    stack.GetXaxis().SetTitleSize(0.04)

    legend.Draw("9")

    pad = canvas.cd(2)
    pad.SetPad(0, 0, 1, 0.3)
    pad.SetLeftMargin(0.15)
    pad.SetBottomMargin(0.25)
    pad.SetGrid()

    ratio.Draw("e 9")

    canvas.SaveAs("shape.pdf")
    canvas.SaveAs("shape.png")

    canvas.Update()

def main(argv = sys.argv):
    try:
        if 3 > len(argv):
            raise Exception(usage(argv))

        rootStyle("tdrstyle.C")

        compare(**dict(x.split(':') for x in argv[1:3]))

    except Exception as error:
        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
