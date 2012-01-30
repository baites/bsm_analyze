#!/usr/bin/env python
#
# Created by Samvel Khalatyan, Jan 04, 2012
# Copyright 2012, All rights reserved

import os

from ROOT import *

def expandHistograms(histograms, channels, suffix):
    for channel in channels:
        histograms.append("el_mttbar__{0}{1}".format(channel, suffix))

def histograms():
    mc = ("ttbar", "zjets", "wjets", "singletop")
    signal = ("zp1000", "zp1500", "zp2000", "zp3000", "zp4000")
    data = ("DATA", "eleqcd")

    histograms = []
    for suffix in ("", ):
        expandHistograms(histograms, (x for x in mc + signal + data if x != "wjets"), suffix)

    for suffix in ("__jes__minus", "__jes__plus", "__pileup__plus", "__pileup__minus"):
        expandHistograms(histograms, mc + signal, suffix)

    for suffix in ("__matching__plus", "__matching__minus"):
        expandHistograms(histograms, ("wjets", "ttbar"), suffix)

    for suffix in ("__scaling__plus", "__scaling__minus"):
        expandHistograms(histograms, ("wjets", "ttbar"), suffix)

    return histograms

def rootStyle(filename):
    if os.path.isfile(filename):
        gROOT.ProcessLine(".L {0}".format(filename))
        ROOT.setTDRStyle()

        print("Loaded ROOT style from: {0}".format(filename))

def removeScaling(file_in, file_out):
    if os.path.isfile(file_in):
        output = TFile(file_out, "recreate")
        input = TFile(file_in)

        # Save everyting in output file except wjets
        #
        hists = histograms()

        for hist in hists:
            h = input.Get(hist)

            output.WriteObject(h, hist)

        # process separately wjets
        #
        hist_name = "el_mttbar__wjets"
        nominal = input.Get(hist_name)
        scale_up = input.Get("{0}__scaling__plus".format(hist_name))

        clone = nominal.Clone()
        bins = clone.GetNbinsX()
        scale = 0.37
        for bin in range(bins + 2):
            content, variance = clone.GetBinContent(bin) * (1 - scale) + scale * scale_up.GetBinContent(bin), (clone.GetBinError(bin) * (1 - scale)) ** 2 + (scale * scale_up.GetBinContent(bin)) ** 2

            clone.SetBinContent(bin, content)
            clone.SetBinError(bin, sqrt(variance))

        output.WriteObject(clone, hist_name)

    else:
        print("input file does not exist: {0}".format(file_in))

if "__main__" == __name__:
    rootStyle("tdrstyle.C")

    removeScaling("theta_input.root", "theta_input_no_scaling.root")
