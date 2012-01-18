#!/usr/bin/env python
#
# Read Theta calculated expected and observed limits, plot these and
# theoretical lines
#
# Created by Samvel Khalatyan, Dec 19, 2011
# Copyright 2011, All rights reserved

from ROOT import *
from array import array
import os
import re
import sys
from theoryxsecs import *

# Apply styles
#
def rootStyle(filename):
    if os.path.isfile(filename):
        gROOT.ProcessLine(".L {0}".format(filename))
        ROOT.setTDRStyle()

        print("Loaded style from: {0}".format(filename))

def loadLimits(filename):
    # Data convention:
    #   mass: (y, low_two_sigma, high_two_sigma, low_one_sigma, high_one_sigma)
    #
    data = {}
    if os.path.isfile(filename):
        with open(filename) as input:
            for line in input:
                if line.startswith('#'):
                    continue

                line_data = re.split('\s+', line.strip())

                data[float(line_data[0])] = tuple(float(x) for x in line_data[1:])
    else:
        print("limist file does not exist: {0}".format(filename))

    return data

def plot(expected_limits, observed_limits):
    canvas = TCanvas()
    canvas.SetWindowSize(640, 480)
    pad = canvas.cd(1)
    pad.SetRightMargin(10)
    pad.SetTopMargin(10)

    frame = TH2F("frame", "frame", 10, 1, 3, 1500, 0, 15)
    frame.GetXaxis().SetTitle("Z' mass [TeV/c^{2}]")
    frame.GetYaxis().SetTitle("#sigma(Z' #rightarrow t #bar{t}) #upoint BR [pb]")
    frame.SetNdivisions(505)
    frame.Draw("9")

    legend = TLegend(.5, .65, .85, .88)
    legend.SetMargin(0.12)  
    legend.SetTextSize(0.03)
    legend.SetFillColor(10) 
    legend.SetBorderSize(0) 

    expected = {
            "nominal": None,
            "one_sigma": None,
            "two_sigma": None
    }

    # nominal
    #
    xlist = sorted(expected_limits)
    xlist_reversed = xlist[:]
    xlist_reversed.reverse()

    ylist = list(expected_limits[x][0] for x in xlist)

    x = array('d')
    y = array('d')

    x.fromlist(list(x / 1000 for x in xlist))
    y.fromlist(ylist)
    graph = TGraph(len(x), x, y)
    graph.SetLineColor(kGray + 1)
    graph.SetFillColor(kGray + 1)
    graph.SetLineWidth(2)

    legend.AddEntry(graph, "expected (95% C.L)", "L")

    expected["nominal"] = graph

    # one sigma
    #
    y_one_sigma = array('d')
    ylist = list(expected_limits[x][3] for x in xlist)
    ylist = ylist + list(expected_limits[x][4] for x in xlist_reversed)

    x = array('d')
    y = array('d')
    x.fromlist(list(x / 1000 for x in xlist + xlist_reversed))
    y.fromlist(ylist)
    graph = TGraph(len(x), x, y)
    graph.SetLineColor(kGreen + 2)
    graph.SetFillColor(kGreen + 2)

    legend.AddEntry(graph, "central 1#sigma expected", "fe")
    
    expected["one_sigma"] = graph

    # two sigma
    #
    y_two_sigma = array('d')
    ylist = list(expected_limits[x][1] for x in xlist)
    ylist = ylist + list(expected_limits[x][2] for x in xlist_reversed)

    y = array('d')
    y.fromlist(ylist)
    graph = TGraph(len(x), x, y)
    graph.SetLineColor(kGreen)
    graph.SetFillColor(kGreen)

    legend.AddEntry(graph, "central 2#sigma expected", "fe")

    expected["two_sigma"] = graph

    for key in ("two_sigma", "one_sigma", "nominal"):
        if not key in expected:
            print("key {0} in not found in the expected graphs".format(key))
            continue

        graph = expected[key]
        if graph:
            graph.Draw("l 9" if "nominal" == key else "F 9")

    # observed
    #
    xlist = sorted(observed_limits)
    ylist = list(observed_limits[x][0] for x in xlist)

    x = array('d')
    y = array('d')

    x.fromlist(list(x / 1000 for x in xlist))
    y.fromlist(ylist)
    graph = TGraph(len(x), x, y)
    graph.SetLineWidth(2)

    legend.AddEntry(graph, "observed (95% C.L.)", "L")

    observed = {"nominal": graph}

    observed["nominal"].Draw("l 9")

    # theoretical x-sections
    #
    theory = {
            "1_2": None,
            "3_0": None
    }

    # Topcolor $\\rm Z^{\\prime}$, 1.2% width, Harris et al.
    #
    theory_x, theory_y = get_zp(1.2)
    low_x = min(xlist)
    xlist = list(x for x in theory_x if x >= low_x)
    ylist = list(theory_y[i] for i in range(len(theory_x)) if theory_x[i] >= low_x)

    x = array('d')
    y = array('d')

    x.fromlist(list(x / 1000 for x in xlist))
    y.fromlist(ylist)

    graph = TGraph(len(x), x, y)
    graph.SetLineColor(TColor.GetColor("#ff00ff"))
    graph.SetLineStyle(kDashed)
    graph.SetLineWidth(2)

    legend.AddEntry(graph, "Topcolor Z', 1.2% width, Harris et al.", "L")

    theory["1_2"] = graph

    # Topcolor $\\rm Z^{\\prime}$, 3.0% width, Harris et al.
    #
    theory_x, theory_y = get_zp(3.0)
    low_x = min(xlist)
    xlist = list(x for x in theory_x if x >= low_x)
    ylist = list(theory_y[i] for i in range(len(theory_x)) if theory_x[i] >= low_x)

    x = array('d')
    y = array('d')

    x.fromlist(list(x / 1000 for x in xlist))
    y.fromlist(ylist)

    graph = TGraph(len(x), x, y)
    graph.SetLineColor(TColor.GetColor("#0000ff"))
    graph.SetLineStyle(kDashed)
    graph.SetLineWidth(2)

    legend.AddEntry(graph, "Topcolor Z', 3.0% width, Harris et al.", "L")

    theory["3_0"] = graph

    for key, graph in theory.items():
        if graph:
            graph.Draw("l 9")

    cms_legend = TLatex(0.5,0.92, "CMS 2011 Preliminary, #sqrt{s} = 7 TeV")
    cms_legend.SetNDC()
    cms_legend.SetTextSize(0.035)
    cms_legend.Draw("9")

    lumi_legend = TLatex(0.2, 0.92, "L = 4.33 fb^{-1}, e+jets")
    lumi_legend.SetNDC()
    lumi_legend.SetTextSize(0.035)
    lumi_legend.Draw("9")

    method_legend = TLatex(0.2, 0.85, "{0} method".format("Bayesian" if 1 < len(sys.argv) else "CLs"))
    method_legend.SetNDC()
    method_legend.SetTextSize(0.035)
    method_legend.Draw("9")

    legend.Draw("9")

    frame.Draw("axis same 9")

    canvas.Update()

    canvas.SaveAs("theta_limits.pdf")
    canvas.SaveAs("theta_limits.png")

    canvas.SetLogy()
    canvas.Update()

    canvas.SaveAs("theta_limits_log.pdf")
    canvas.SaveAs("theta_limits_log.png")

if "__main__" == __name__:
    rootStyle("tdrstyle.C")

    expected_limits = loadLimits("exp_limit.txt")
    print("Expected limits")
    print(expected_limits)
    print("")

    observed_limits = loadLimits("obs_limit.txt")
    print("Observed limits")
    print(observed_limits)
    print("")

    plot(expected_limits, observed_limits)
