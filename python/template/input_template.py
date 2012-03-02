#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 21, 2012
Copyright 2011, All rights reserved
'''

from __future__ import division

from root.template import Template

from input_type import InputType
from input_info import InputInfo

class InputTemplate(InputType, Template, InputInfo):
    '''
    Container for input plot and type. Each input plot is cloned and
    automatically scaled to cross-section, luminosity and Monte-Carlo
    number of processed events
    '''

    def __init__(self, input_type, template):
        '''
        Initialize object with input type and histogram. Histogram will be
        cloned and automatically scaled to x-section, luminosity and 
        number of Monte-Carlo processed events
        '''

        InputType.__init__(self, input_type)

        # cache scale factor for future fast access
        self.__scale = (self.xsection * self.luminosity() /
                        self.events) if self.events else 1

        # histogram will be scaled upon set and scale factor need to be set
        Template.__init__(self,
                          template = template)

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
    def hist(self):
        '''
        Redefine hist property to make it scalable when set
        '''

        # route to superclass
        return Template.hist.__get__(self, self.__class__)

    @hist.setter
    def hist(self, obj):
        '''
        Automatically scale histogram when new plot is set
        '''

        # let super-class work on the histogram
        Template.hist.__set__(self, obj)

        # auto-scale plot if it was set
        if self.hist:
            template_scale = self.scale
            if template_scale:
                self.hist.Scale(template_scale)

            info = self.info
            if info.rebin:
                if 2 < self.dimension:
                    self.hist.RebinZ(info.rebin[-1])
                
                if 1 < self.dimension:
                    self.hist.RebinY(info.rebin[1])
                    self.hist.RebinX(info.rebin[0])
                else:
                    self.hist.Rebin(info.rebin)

            if info.title:
                if 1 == self.dimension:
                    self.hist.GetXaxis().SetTitle(info.title +
                            ((" [" + info.units + "]") if info.units else ""))

            if info.units:
                if 1 == self.dimension:
                    self.hist.GetYaxis().SetTitle(
                            "events yield / {bin_width:.1f} {units}".format(
                                bin_width = self.hist.GetBinWidth(1),
                                units = info.units))
            
    def __str__(self):
        '''
        Add lumi and scale numbers to InputType pretty print
        '''

        return ("<{Class} lumi {lumi:.1f} fb-1 scale {scale:.2f} at 0x{ID:x}>\n"
                "  + {InputTypeStr}\n"
                "  + {TemplateStr}").format(
                    Class = self.__class__.__name__,
                    lumi = self.luminosity() / 1000,
                    scale = self.scale,
                    InputTypeStr = InputType.__str__(self),
                    TemplateStr = Template.__str__(self),
                    ID = id(self))

if "__main__" == __name__:
    import unittest
    import ROOT
    import random

    # Prepare function for later random fill
    my_gaus = ROOT.TF1("my_gaus", "gaus(0)", 0, 100)
    my_gaus.SetParameters(1, 50, 10)

    # Create plot and randomly fill with above function
    plot = ROOT.TH1F("plot", "plot", 100, 0, 100);
    plot.FillRandom("my_gaus", 10000)

    class TestInputTemplate(unittest.TestCase):
        def test_empty_template_type(self):
            input_type = random.choice(InputType.input_types.keys())
            template = InputTemplate(input_type, Template())
            self.assertEqual(template.type, input_type)

        def test_empty_template_path(self):
            input_type = random.choice(InputType.input_types.keys())
            template = InputTemplate(input_type, Template())
            self.assertEqual(template.path, "")

        def test_empty_template_name(self):
            input_type = random.choice(InputType.input_types.keys())
            template = InputTemplate(input_type, Template())
            self.assertEqual(template.name, "")

        def test_empty_template_dim(self):
            input_type = random.choice(InputType.input_types.keys())
            template = InputTemplate(input_type, Template())
            self.assertEqual(template.dimension, None)

        def test_empty_template_hist(self):
            input_type = random.choice(InputType.input_types.keys())
            template = InputTemplate(input_type, Template())
            self.assertEqual(template.hist, None)

        def test_template_path(self):
            input_type = random.choice(InputType.input_types.keys())
            template = InputTemplate(input_type,
                                     template = Template(plot,
                                                         clone = True))
            self.assertEqual(template.path, "")

        def test_template_name(self):
            input_type = random.choice(InputType.input_types.keys())
            template = InputTemplate(input_type, Template(plot, clone = True))
            self.assertEqual(template.name, "plot")

        def test_template_dim(self):
            input_type = random.choice(InputType.input_types.keys())
            template = InputTemplate(input_type, Template(plot, clone = True))
            self.assertEqual(template.dimension, 1)

        def test_template_hist(self):
            input_type = random.choice(InputType.input_types.keys())
            template = InputTemplate(input_type, Template(plot, clone = True))
            self.assertEqual(template.hist.GetTitle(), "plot")

        def test_template_hist_object(self):
            input_type = random.choice(InputType.input_types.keys())
            template = InputTemplate(input_type, Template(plot, clone = True))
            self.assertNotEqual(template.hist, plot)

        def test_template_analytical_scale(self):
            input_type = random.choice(InputType.input_types.keys())
            template = InputTemplate(input_type, Template(plot, clone = True))
            self.assertEqual(template.xsection * template.luminosity() /
                                 template.events,
                             template.scale)

        def test_template_graphical_scale(self):
            input_type = random.choice(InputType.input_types.keys())
            template = InputTemplate(input_type, Template(plot, clone = True))
            self.assertEqual(int(1000 *
                                 template.hist.Integral() / plot.Integral()),
                             int(1000 * template.scale))

    from input_type import InputData
    class InputTemplateWithQCD(InputTemplate):
        input_types = InputTemplate.input_types.copy()
        input_types.update({
            "qcd": InputData(2.361e8 * 1.06e-2, 35729669)
        })

    class TestInputTemplateWithQCD(unittest.TestCase):
        def test_template_qcd(self):
            template = InputTemplateWithQCD("qcd",
                                            template = Template(plot,
                                                                clone = True))
            self.assertEqual(template.type, "qcd")

        def test_template_ttbar(self):
            template = InputTemplateWithQCD("ttbar",
                                            template = Template(plot,
                                                                clone = True))
            self.assertEqual(template.type, "ttbar")

    unittest.main()
