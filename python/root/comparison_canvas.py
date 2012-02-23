#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 23, 2012
Copyright 2011, All rights reserved
'''

import ROOT

def compare(function):
    '''
    function decorator: it will call custom ratio calculation function and
    adjust styles for ratio plot to look nice
    '''

    def compare_decorator(*parg, **karg):
        '''
        wrapper around custom ratio calculator: adjust ratio plot style
        '''

        # let custom function calcaule ratio and set all titles
        ratio = function(*parg, **karg)

        # adjust style of ratio plot
        axis = ratio.GetXaxis()
        axis.SetTitleSize(0.1)
        axis.SetTitleOffset(1.2)

        axis = ratio.GetYaxis()
        axis.SetTitleSize(0.08)
        axis.SetTitleOffset(0.5)
        axis.SetNdivisions(4)

        for axis in ratio.GetYaxis(), ratio.GetXaxis():
            axis.SetLabelSize(0.09)

        ratio.SetMarkerSize(0.5)
        ratio.SetLineWidth(2)
        ratio.SetLineColor(ROOT.kGray + 2)
        ratio.SetMarkerColor(ROOT.kBlack)

        return ratio

    return compare_decorator

class ComparisonCanvas(object):
    '''
    Canvas with two pads:

        1   (top)       original plots should be drawn here
        2   (bottom)    comparison of the plots should be drawn in this pad

    It is recommended to use ComparisonCanvas class in conjuction with compare
    decorator, e.g.:

        class DataMcComparisonCanvas(ComparisonCanvas):
            ...

            @compare
            def ratio(self, data, mc):
                ratio = data.Clone()
                ratio.SetDirectory(0)

                ratio.Add(mc, -1)
                ratio.Divide(mc)
                ratio.GetYaxis().SetTitle("#frac{Data - MC}{MC}")

                return ratio

            def draw(self, data, mc):
                canvas = self.canvas
                canvas.cd(1)

                stack = ROOT.THStack()
                stack.Add(data)
                stack.Add(mc)
                stack.Draw("nostack hist 9")

                canvas.cd(2)
                self.ratio(data, mc).Draw("e 9")
            
            ...

    in the above example ratio method will be wrapped into compare decorator
    and comparison plot (ratio) style will be automatically adjusted

    Canvas is automatically created on acesss
    '''

    def __init__(self):
        '''
        Initialize with empty canvas
        '''

        self.__canvas = None

    @property
    def canvas(self):
        '''
        Create canvas if one does not exist and split into 70% and 30%
        vertical Pads for comparison plot
        '''

        if not self.__canvas:
            # create canvas
            canvas = ROOT.TCanvas()
            canvas.SetWindowSize(640, 800)
            canvas.Divide(1, 2)

            # prepare top pad for original plots to be drawn overlayed
            pad = canvas.cd(1)
            pad.SetPad(0, 0.3, 1, 1)
            pad.SetRightMargin(5)

            # prepare bottom pad for comparison/ratio draw
            pad = canvas.cd(2)
            pad.SetPad(0, 0, 1, 0.3)
            pad.SetBottomMargin(0.3)
            pad.SetRightMargin(5)
            pad.SetGrid()

            self.__canvas = canvas

        return self.__canvas



if "__main__" == __name__:
    import root.style

    style = root.style.tdr()
    style.cd()

    # Prepare function for later random fill
    my_gaus1 = ROOT.TF1("my_gaus1", "gaus(0)", 0, 100)
    my_gaus1.SetParameters(1, 50, 10)

    my_gaus2 = ROOT.TF1("my_gaus2", "gaus(0)", 0, 100)
    my_gaus2.SetParameters(1, 40, 10)

    # Create plot and randomly fill with above function
    plot1 = ROOT.TH1F("plot1", "plot1", 50, 0, 100);
    plot1.FillRandom("my_gaus1", 10000)
    plot1.SetLineColor(ROOT.kRed + 1)

    plot2 = ROOT.TH1F("plot2", "plot2", 50, 0, 100);
    plot2.FillRandom("my_gaus2", 10000)

    class ComparePlots(ComparisonCanvas):
        @ratio
        def ratio(self, first, second):
            ratio = first.Clone()
            ratio.SetDirectory(0)
            ratio.Reset()

            ratio.Divide(first, second)
            ratio.GetYaxis().SetTitle("{0}/{1}".format(first.GetName(), second.GetName()))

            return ratio

        def __call__(self, first, second):
            canvas = self.canvas
            canvas.cd(1)

            stack = ROOT.THStack()
            stack.Add(first)
            stack.Add(second)
            stack.Draw("nostack hist 9")

            canvas.cd(2)
            self.ratio(first, second).Draw("e 9")

            canvas.Update()
            
            raw_input("enter")

    compare = ComparePlots()
    compare(plot1, plot2)
