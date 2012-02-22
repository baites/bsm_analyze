#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 21, 2012
Copyright 2011, All rights reserved
'''

from __future__ import division, print_function

import ROOT

from input_type import InputType

class InputTemplate(InputType):
    '''
    Container for input plot and type. Each input plot is cloned and
    automatically scaled to cross-section, luminosity and Monte-Carlo
    number of processed events
    '''

    def __init__(self, input_type_, plot):
        '''
        Initialize object with input type and histogram. Histogram will be
        cloned and automatically scaled to x-section, luminosity and 
        number of Monte-Carlo processed events
        '''

        InputType.__init__(self, input_type_)

        # cache scale factor for future fast access
        if "data" == self.type:
            self.__scale = 1
        else:
            self.__scale = self.xsection * InputTemplate.luminosity() / self.events

        self.__plot = plot.Clone()
        self.plot.SetDirectory(0)

        self.plot.Scale(self.scale)

    @staticmethod
    def luminosity():
        '''
        Luminosity, aka amount of processed data
        '''

        return 4328.472

    @property
    def scale(self):
        '''
        InputTemplate scale factor
        '''

        return self.__scale

    @property
    def plot(self):
        '''
        Access plot
        '''

        return self.__plot

    def __str__(self):
        '''
        Add lumi and scale numbers to InputType pretty print
        '''

        front, back = InputType.__str__(self).split(" at ", 1)

        return (front +
                " lumi {lumi:.1f} fb-1 scale {scale:.2f} at ".format(
                        lumi = self.luminosity() / 1000,
                        scale = self.scale) +
                back)

if "__main__" == __name__:
    '''
    Test InputTemplate class with:

        - create random plot
        - create randomly picked input type with above plot
        - plot original histogram and input
        - print input with scale factors, etc.
    '''

    try:
        print("{0:-<80}".format("-- Create plot, random fill, scale, draw "))

        # Prepare function for later random fill
        my_gaus = ROOT.TF1("my_gaus", "gaus(0)", 0, 100)
        my_gaus.SetParameters(1, 50, 10)

        # Create plot and randomly fill with above function
        plot = ROOT.TH1F("plot", "plot", 100, 0, 100);
        plot.FillRandom("my_gaus", 10000)

        # Create random input
        import random
        input_ = InputTemplate(random.choice(InputType.input_types.keys()), plot)

        # Prepare canvas for drawing
        canvas = ROOT.TCanvas()
        canvas.SetWindowSize(800, 480)
        canvas.Divide(2)

        # Draw original plot
        canvas.cd(1)
        plot.Draw("hist 9")

        # Draw input plot (it should be scaled)
        canvas.cd(2)
        input_.plot.Draw("hist 9")

        canvas.Update()

        # print input
        print(input_)

        raw_input("enter")

    except AttributeError as error:
        print(error)
    finally:
        print('-' * 80)

    try:
        print("{0:-<80}".format("-- Expand InputTemplate types with QCD, Create plot, "
                                "random fill, scale, draw "))

        from input_type import InputData

        class InputTemplateWithQCD(InputTemplate):
            input_types = InputTemplate.input_types.copy()
            input_types.update({
                "qcd": InputData(2.361e8 * 1.06e-2, 35729669)
            })

        # Prepare canvas for drawing
        canvas2 = ROOT.TCanvas()
        canvas2.SetWindowSize(800, 480)
        canvas2.Divide(2)

        # Create and draw qcd
        canvas2.cd(1)
        qcd = InputTemplateWithQCD("qcd", plot)
        qcd.plot.Draw("hist 9")

        # Create and draw ttbar
        canvas2.cd(2)
        ttbar = InputTemplateWithQCD("ttbar", plot)
        ttbar.plot.Draw("hist 9")

        canvas2.Update()

        # print input
        print(qcd)
        print(ttbar)

        raw_input("enter")

    except AttributeError as error:
        print(error)
    finally:
        print('-' * 80)
