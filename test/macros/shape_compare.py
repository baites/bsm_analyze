#!/usr/bin/env python
#
# Created by Samvel Khalatyan, Jan 09, 2012
# Copyright 2012, All rights reserved

from __future__ import division, print_function

import os
import sys

from compare import Compare

from ROOT import *

def rootStyle(filename):
    if os.path.isfile(filename):
        gROOT.ProcessLine(".L {0}".format(filename))
        ROOT.setTDRStyle()

        print("Loaded ROOT style from: {0}".format(filename))

class CompareOld():
    def __init__(self, left_folder, right_folder):
        self.left_folder = left_folder
        self.right_folder = right_folder
        self.objects= []

    def run(self):
        for channel in [x + "jets" for x in ["tt", "w", "z"]]:
            '''
        for channel in [x + "jets" for x in ["tt", "w", "z"]] \
                + ["zprime_m" + x + "00_w" + x for x in \
                    [str(x) for x in [10, 15, 20, 30, 40]]] \
                + [x + "_" + y for y in ["s", "t", "tw"] \
                    for x in ["stop", "satop"]]:
            '''

            filename = "output_signal_p150_hlt.root"
            left_input = "{0}/{1}/{2}".format(self.left_folder,
                    channel,
                    filename).replace("//", "/")

            right_input = "{0}/{1}/{2}".format(self.right_folder,
                    channel,
                    filename).replace("//", "/")

            if not os.path.exists(left_input):
                print("skip channel {0} - input is not available: {1}".format(channel, left_input))
                continue

            if not os.path.exists(right_input):
                print("skip channel {0} - input is not available: {1}".format(channel, right_input))
                continue

            self.compare(channel, left_input, right_input)

        if len(self.objects):
            raw_input("press Enter")

    def compare(self, channel, left_input, right_input):
        lin = TFile(left_input)
        if lin.IsZombie():
            print("failed to open input: {0}".format(left_input))
            return

        rin = TFile(right_input)
        if rin.IsZombie():
            print("failed to open input: {0}".format(right_input))
            return

        plot_name = "mttbar_after_htlep"

        lh = lin.Get(plot_name)
        if type(lh) == TObject:
            print("failed to extract {0} from {1}".format(plot_name, left_input))

        lh = lh.Clone()
        lh.SetDirectory(0)
        lh.SetTitle(channel + 

        rh = rin.Get(plot_name)
        if type(rh) == TObject:
            print("failed to extract {0} from {1}".format(plot_name, right_input))

        rh = rh.Clone()
        rh.SetDirectory(0)

        lh.Scale(1 / lh.Integral())
        rh.Scale(1 / rh.Integral())

        lh.Rebin(100)
        rh.Rebin(100)

        lh.SetLineColor(ROOT.kRed)
        rh.SetLineColor(ROOT.kGreen)

        for h in [lh, rh]:
            h.SetLineWidth(2)
            h.SetFillStyle(0)

        compare = Compare()
        compare(rh, lh)

        self.objects.append(compare)

        '''
        legend = TLegend(.5, .65, .85, .88)
        legend.SetMargin(0.12)  
        legend.SetTextSize(0.03)
        legend.SetFillColor(10) 
        legend.SetBorderSize(0) 

        legend.SetHeader(channel)
        legend.AddEntry(lh, left_input.split("/")[-3], "L")
        legend.AddEntry(rh, right_input.split("/")[-3], "L")

        legend.Draw()

        canvas.Update()

        canvas.SaveAs("{0}.pdf".format(channel))

        self.objects.append(legend)
        self.objects.append(lh)
        self.objects.append(rh)
        self.objects.append(canvas)
        '''

def usage(argv):
    return "usage: {0} numerator:folder denominator:folder".format(argv[0])

def main(argv = sys.argv):
    try:
        if 3 > len(sys.argv):
            raise Exception(usage(argv))

        rootStyle("tdrstyle.C")

        compare = CompareOld(*sys.argv[1:3])
        compare.run()
    except Exception as error:
        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
