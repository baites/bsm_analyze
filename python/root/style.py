#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 09, 2012
Copyright 2011, All rights reserved

Collection of different ROOT styles
'''

import array
import ROOT

def tdr():
    style = ROOT.TStyle("tdr_style", "Style for P-TDR")

    # For the canvas:
    style.SetCanvasBorderMode(0)
    style.SetCanvasColor(ROOT.kWhite)
    style.SetCanvasDefH(600) #Height of canvas
    style.SetCanvasDefW(600) #Width of canvas
    style.SetCanvasDefX(0)   #POsition on screen
    style.SetCanvasDefY(0)

    # For the Pad:
    style.SetPadBorderMode(0)
    style.SetPadColor(ROOT.kWhite)
    style.SetPadGridX(False)
    style.SetPadGridY(False)
    style.SetGridColor(0)
    style.SetGridStyle(3)
    style.SetGridWidth(1)

    # For the frame:
    style.SetFrameBorderMode(0)
    style.SetFrameBorderSize(1)
    style.SetFrameFillColor(0)
    style.SetFrameFillStyle(0)
    style.SetFrameLineColor(1)
    style.SetFrameLineStyle(1)
    style.SetFrameLineWidth(1)

    # For the histo:
    style.SetHistLineColor(1)
    style.SetHistLineStyle(0)
    style.SetHistLineWidth(1)

    style.SetEndErrorSize(2)

    style.SetMarkerStyle(20)
    style.SetMarkerSize(1)

    #For the fit/function:
    style.SetOptFit(1)
    style.SetFitFormat("5.4g")
    style.SetFuncColor(2)
    style.SetFuncStyle(1)
    style.SetFuncWidth(1)

    #For the date:
    style.SetOptDate(0)

    # For the statistics box:
    style.SetOptFile(0)
    style.SetOptStat(0) # To display the mean and RMS:   SetOptStat("mr");
    style.SetOptStat(1111)
    style.SetOptStat(False)
    style.SetStatColor(ROOT.kWhite)
    style.SetStatFont(42)
    style.SetStatFontSize(0.035)
    style.SetStatTextColor(1)
    style.SetStatFormat("6.4g")
    style.SetStatBorderSize(1)
    style.SetStatH(0.5)
    style.SetStatW(0.4)

    # Margins:
    style.SetPadTopMargin(10)
    style.SetPadBottomMargin(0.15)
    style.SetPadLeftMargin(0.2)
    style.SetPadRightMargin(10)

    # For the Global title:

    style.SetOptTitle(0)
    style.SetTitleFont(42)
    style.SetTitleColor(1)
    style.SetTitleTextColor(1)
    style.SetTitleFillColor(10)
    style.SetTitleFontSize(0.05)

    # For the axis titles:

    style.SetTitleColor(1, "XYZ")
    style.SetTitleFont(42, "XYZ")
    style.SetTitleSize(0.04, "XYZ")
    style.SetTitleXOffset(10)
    style.SetTitleYOffset(1.7)

    # For the axis labels:

    style.SetLabelColor(1, "XYZ")
    style.SetLabelFont(42, "XYZ")
    style.SetLabelOffset(0.007, "XYZ")
    style.SetLabelSize(0.04, "XYZ")
    style.SetNdivisions(8)

    # For the axis:

    style.SetAxisColor(1, "XYZ")
    style.SetStripDecimals(True)
    style.SetTickLength(0.03, "XYZ")
    style.SetNdivisions(510, "XYZ")

    # Change for log plots:
    style.SetOptLogx(0)
    style.SetOptLogy(0)
    style.SetOptLogz(0)

    # Postscript options:
    style.SetPaperSize(20.,20.)

    # 2D Plots palette
    #
    red = array.array('d', [ 0.60, 0.00, 0.00, 0.00, 1.00, 1.00, 1.00])
    green = array.array('d', [ 0.00, 0.00, 0.60, 1.00, 1.00, 0.60, 0.00])
    blue = array.array('d', [ 1.00, 1.00, 1.00, 0.00, 0.00, 0.00, 0.00])
    length = array.array('d', [ 0.00, 0.17, 0.34, 0.51, 0.68, 0.85, 1.00])

    ROOT.TColor.CreateGradientColorTable(len(red), length, red, green, blue, 50)

    # Legends
    style.SetLegendBorderSize(0)

    style.cd()
