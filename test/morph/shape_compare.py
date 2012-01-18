#!/usr/bin/env python
#
# Created by Samvel Khalatyan, Jan 11, 2012
# Copyright 2012, All rights reserved

from __future__ import division, print_function

import os
import sys

from ROOT import *

def rootStyle(filename):
    if os.path.lexists(filename):
        gROOT.ProcessLine(".L {0}".format(filename))
        ROOT.setTDRStyle()

        print("Loaded ROOT style: {0}".format(filename))
    else:
        print(("ROOT style can not be loaded - "
               "file does not exist: {0}").format(filename))

class Compare():
    def __init__(self, file, low, morph, high):
        self.file = file
        self.low = low
        self.morph = morph
        self.high = high
        self.objects= []

    def run(self):
        for suffix in ["",
                       "__jes__minus",
                       "__jes__plus",
                       "__pileup__minus",
                       "__pileup__plus"]:
            self.compare(suffix)

        if len(self.objects):
            raw_input("press Enter")

        self.objects = []

    def compare(self, suffix):
        file = TFile(self.file)
        if file.IsZombie():
            print("failed to open input file: ", self.file)

            return 

        plots = []
        title = suffix.replace("__", " ")
        title.strip()
        legend = self.legend(title)

        h = file.Get(self.histogram(self.low, suffix))
        if not h:
            print("failed to extract: ", self.histogram(self.low, suffix))
        else:
            h = h.Clone()
            h.SetLineColor(ROOT.kRed)
            legend.AddEntry(h, self.low, "l")
            plots.append(h)

        for morph in self.morph:
            h = file.Get(self.histogram(morph, suffix))
            if not h:
                print("failed to extract: ", self.histogram(morph, suffix))
            else:
                h = h.Clone()
                legend.AddEntry(h, morph, "l")
                plots.append(h)

        h = file.Get(self.histogram(self.high, suffix))
        if not h:
            print("failed to extract: ", self.histogram(self.high, suffix))
        else:
            h = h.Clone()
            h.SetLineColor(ROOT.kGreen + 2)
            legend.AddEntry(h, self.high, "l")
            plots.append(h)

        stack = THStack()
        self.objects.append(stack)

        for h in plots:
            h.SetDirectory(0)
            h.Rebin(100)
            h.Scale(1 / h.Integral())

            stack.Add(h)

        canvas = TCanvas()
        self.objects.append(canvas)

        canvas.SetWindowSize(640, 480)

        stack.Draw("hist nostack 9")

        len(plots) and stack.GetHistogram().GetXaxis().SetTitle(plots[0].GetXaxis().GetTitle())
        stack.GetHistogram().GetYaxis().SetTitle("a.u.")

        legend.Draw("9")

        canvas.Update()

        if not suffix:
            canvas.SaveAs("_".join(self.morph) + ".pdf")

        file.Close()

    def legend(self, title):
        legend = TLegend(.8, .65, .9, .88)
        self.objects.append(legend)

        legend.SetHeader(title if title else "nominal")

        legend.SetMargin(0.12)  
        legend.SetTextSize(0.03)
        legend.SetFillColor(10) 
        legend.SetBorderSize(0) 

        return legend

    def histogram(self, mass, suffix = ""):
        return "el_mttbar__zp{0}{1}".format(mass, suffix)

def usage(argv):
    print(("usage: {0} theta_input.root "
           "point point point [point]").format(argv[0]))

def main(argv = sys.argv):
    if 5 > len(argv):
        usage(argv)

        return 1

    if not os.path.exists(argv[1]):
        print("input file does not exist: {0}".format(argv[1]))

        return 1

    rootStyle("tdrstyle.C")

    compare = Compare(file = argv[1],
                      low = argv[2],
                      morph = argv[3:-1],
                      high = argv[-1])
    compare.run()

if "__main__" == __name__:
    sys.exit(main())
