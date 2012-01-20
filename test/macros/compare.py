#!/usr/bin/env python
#
# Created by Samvel Khalatyan, Jan 20, 2012
# Copyright 2012, All rights reserved

from __future__ import division, print_function

import sys

from ROOT import *

class Compare():
    def __init__(self):
        self.numerator = None
        self.denominator = None
        self.canvas = None
        self.stack = None
        self.legend = None
        self.ratio = None

    def __call__(self, numerator, denominator, title = ""):
        '''
        Draw plots and their ratio below
        '''
        self.numerator = numerator
        self.denominator = denominator

        # Calculate ratio plot
        #
        self.ratio = self.numerator.Clone()
        self.ratio.SetDirectory(0)
        self.ratio.Reset()
        self.ratio.Divide(self.numerator, self.denominator)

        axis = self.ratio.GetYaxis()
        axis.SetNdivisions(4)

        axis = self.ratio.GetXaxis()
        axis.SetTitleSize(0.1)
        axis.SetTitleOffset(1.2)

        for axis in self.ratio.GetYaxis(), self.ratio.GetXaxis():
            axis.SetLabelSize(0.09)

        self.ratio.SetMarkerSize(0.5)
        self.ratio.SetLineWidth(1)
        self.ratio.SetLineColor(ROOT.kGray + 1)

        self.legend = TLegend(.8, .3, .9, .5)
        self.legend.SetMargin(0.12)  
        self.legend.SetTextSize(0.03)
        self.legend.SetFillColor(10) 
        self.legend.SetBorderSize(0) 
        if title:
            self.legend.SetHeader(title)

        for h in self.numerator, self.denominator:
            self.legend.AddEntry(h, h.GetTitle(), "l")

        # Draw Canvas
        #
        self.canvas = TCanvas()
        self.canvas.SetWindowSize(640, 640)
        self.canvas.Divide(1, 2)

        # Draw plots
        #
        pad = self.canvas.cd(1)
        pad.SetPad(0, 0.3, 1, 1)
        pad.SetRightMargin(5)

        self.stack = THStack()
        self.stack.Add(self.numerator)
        self.stack.Add(self.denominator)

        self.stack.Draw("nostack hist 9")
        self.legend.Draw("9")

        # Draw ratio
        #
        pad = self.canvas.cd(2)
        pad.SetPad(0, 0, 1, 0.3)
        pad.SetBottomMargin(0.3)
        pad.SetRightMargin(5)
        pad.SetGrid()

        self.ratio.Draw("e 9")

        self.canvas.Update()

if "__main__" == __name__:
    print("module is not designed for execution", file = sys.stderr)

    sys.exit(1)
