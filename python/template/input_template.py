#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 21, 2012
Copyright 2011, All rights reserved
'''

from __future__ import division

from root.template import Template, Templates, find_plots

from input_type import InputType

class InputTemplate(InputType, Template):
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
                          hist = template.hist,
                          filename = template.filename,
                          path = template.path)

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
            self.hist.Scale(self.scale)

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

class InputTemplatesLoader(InputType, Templates):
    rebin = {
        "cutflow": 1,
        "npv": 1,
        "npv_with_pileup": 1,
        "njets": 1,
        "d0": 1,
        "htlep": 1,
        "htall": 1,
        "htlep_after_htlep": 1,
        "htlep_before_htlep": 1,
        "htlep_before_htlep_qcd_noweight": 1,
        "solutions": 1,
        "mttbar_before_htlep": 1,
        "mttbar_after_htlep": 50,
        "dr_vs_ptrel": 0,
        "ttbar_pt": 1,
        "wlep_mt": 1,
        "whad_mt": 1,
        "wlep_mass": 1,
        "whad_mass": 1,
        "met": 1,
        "met_noweight": 1,
        "ljet_met_dphi_vs_met_before_tricut": 0,
        "lepton_met_dphi_vs_met_before_tricut": 0,
        "ljet_met_dphi_vs_met": 0,
        "lepton_met_dphi_vs_met": 0,
        "htop_njets": 1,
        "htop_delta_r": 1,
        "htop_njet_vs_m": 0,
        "htop_pt_vs_m": 0,
        "htop_pt_vs_njets": 0,
        "htop_pt_vs_ltop_pt": 0,
        "njets_before_reconstruction": 1,
        "njet2_dr_lepton_jet1_before_reconstruction": 1,
        "njet2_dr_lepton_jet2_before_reconstruction": 1,
        "njets_after_reconstruction": 1,
        "njet2_dr_lepton_jet1_after_reconstruction": 1,
        "njet2_dr_lepton_jet2_after_reconstruction": 1,
    }

    def __init__(self, input_type):
        InputType.__init__(self, input_type)
        Templates.__init__(self)

        self.use_folders = []
        self.ban_folders = []

        self.use_plots = []
        self.ban_plots = []

        self.__input_templates = {}

    @property
    def input_templates(self):
        return self.__input_templates

    def process_plot(self, template):
        if ((self.use_plots
                and template.name in self.use_plots
                and template.name not in self.ban_plots)

            or (not self.use_plots and template.name not in self.ban_plots)):

            rebin = self.rebin.get(template.name, 0)
            if rebin and 1 != rebin:
                template.hist.Rebin(rebin)

            self.input_templates[template.path.rstrip('/') + '/' +
                                 template.name] = InputTemplate(self.type,
                                                                template)

    def process_folder(self, folder, path, callback):
        if ((self.use_folders
                and path in self.use_folders
                and path not in self.ban_folders)

            or (not self.use_folders and path not in self.ban_folders)):

            find_plots(folder, path, callback)

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
            self.assertEqual(template.dim, None)

        def test_empty_template_hist(self):
            input_type = random.choice(InputType.input_types.keys())
            template = InputTemplate(input_type, Template())
            self.assertEqual(template.hist, None)

        def test_template_path(self):
            input_type = random.choice(InputType.input_types.keys())
            template = InputTemplate(input_type,
                                     Template(plot, "path", clone = True))
            self.assertEqual(template.path, "path")

        def test_template_name(self):
            input_type = random.choice(InputType.input_types.keys())
            template = InputTemplate(input_type, Template(plot, clone = True))
            self.assertEqual(template.name, "plot")

        def test_template_dim(self):
            input_type = random.choice(InputType.input_types.keys())
            template = InputTemplate(input_type, Template(plot, clone = True))
            self.assertEqual(template.dim, 1)

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
            template = InputTemplateWithQCD("qcd", Template(plot, clone = True))
            self.assertEqual(template.type, "qcd")

        def test_template_ttbar(self):
            template = InputTemplateWithQCD("ttbar",
                                            Template(plot, clone = True))
            self.assertEqual(template.type, "ttbar")

    unittest.main()
