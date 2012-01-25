#!/usr/bin/env python
#
# Created by Samvel Khalatyan, Jan 20, 2012
# Copyright 2012, All rights reserved

from __future__ import division, print_function

import os
import sys

import ROOT

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
        self.ratio.GetYaxis().SetTitle("#frac{" + numerator.GetTitle() + "}{"
                + denominator.GetTitle() + "}")
        self.ratio.GetYaxis().SetTitleSize(0.08)
        self.ratio.GetYaxis().SetTitleOffset(0.5)

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
        self.ratio.SetMarkerColor(ROOT.kBlack)

        self.legend = ROOT.TLegend(.6, .3, .9, .5)
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
        self.canvas = ROOT.TCanvas()
        self.canvas.SetWindowSize(640, 640)
        self.canvas.Divide(1, 2)

        # Draw plots
        #
        pad = self.canvas.cd(1)
        pad.SetPad(0, 0.3, 1, 1)
        pad.SetRightMargin(5)

        self.stack = ROOT.THStack()
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

class CompareFiles():
    def __init__(self):
        self.input = None
        self.compare = []

    def __call__(self, left_input, right_input):
        if not left_input:
            raise Exception("left input is not specified")

        if not right_input:
            raise Exception("right input is not specified")

        if not os.path.lexists(left_input):
            raise Exception("left input does not exist: " + left_input)

        if not os.path.lexists(right_input):
            raise Exception("right input does not exist: " + right_input)

        self.input = {
                "left": ROOT.TFile(left_input),
                "right": ROOT.TFile(right_input)
                }

        names = {}
        for key, file in self.input.items():
            if file.IsZombie():
                raise Exception("failed to open file: " + file.GetName())

            names[key] = set([x.GetName() for x in file.GetListOfKeys()])

        common = names["left"] & names["right"]
        left_only = names["left"] - names["right"]
        right_only = names["right"] - names["left"]

        if left_only:
            print("-- Templates found in the file only: {0} ".format(left_input).ljust(80))
            print(left_only)
            print()

        if right_only:
            print("-- Templates found in the file only: {0} ".format(right_input).ljust(80))
            print(right_only)
            print()

        for name in common:
            lh = self.input["left"].Get(name)
            if not lh:
                print("faile to read template: " + name, file = sys.stderr)

                continue

            lh = lh.Clone()
            lh.SetLineColor(ROOT.kRed + 1)
            lh.SetFillColor(0)
            lh.SetTitle(os.path.basename(left_input).rsplit(".", 1)[0])

            rh = self.input["right"].Get(name)
            if not rh:
                print("faile to read template: " + name, file = sys.stderr)

                continue

            rh = rh.Clone()
            rh.SetLineColor(ROOT.kGreen + 1)
            rh.SetFillColor(0)
            rh.SetTitle(os.path.basename(right_input).rsplit(".", 1)[0])

            compare = Compare()
            compare(numerator = lh, denominator = rh, title = name)
            self.compare.append(compare)

if "__main__" == __name__:
    print("module is not designed for execution", file = sys.stderr)

    sys.exit(1)
