#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 15, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function

import sys

import root.template

import ROOT

class HadronicTopTemplates(root.template.Templates):
    axis_titles = {
            "njets": "N_{jets}",
            "mass": "M",
            "mt": "M_{T}",
            "energy": "E",
            "et": "E_{T}",
            "px": "p_{x}",
            "py": "p_{y}",
            "pz": "p_{z}",
            "pt": "p_{T}",
            "eta": "#eta",
            "phi": "#phi",
            "pdg_id": "ID_{PDG}",
            "status": "status",
            "dr": "#Delta R",
            "ptrel": "p^{rel}_{T}",
            "deta": "#Delta #eta",
            "dphi": "#Delta #phi",
            "angle": "#alpha"
            }

    units = {
            "mass": "[GeV/c^{2}]",
            "mt": "[GeV/c^{2}]",
            "energy": "[GeV/c^{2}]",
            "et": "[GeV/c^{2}]",
            "px": "[GeV/c]",
            "py": "[GeV/c]",
            "pz": "[GeV/c]",
            "pt": "[GeV/c]",
            "phi": "[rad]",
            "ptrel": "[GeV/c]",
            "dphi": "[rad]"
            }

    top_rebin = {
            "energy": 5,
            "px": 5,
            "py": 5,
            "pz": 5,
            "pt": 10,
            "eta": 50,
            "phi": 10,
            "mass": 10,
            "mt": 10,
            "et": 5
            }

    jet_rebin = {
            "energy": 5,
            "px": 5,
            "py": 5,
            "pz": 5,
            "pt": 25,
            "eta": 50,
            "phi": 10,
            "mass": 10,
            "mt": 10,
            "et": 5
            }

    parton_rebin = {
            "energy": 5,
            "px": 5,
            "py": 5,
            "pz": 5,
            "pt": 25,
            "eta": 50,
            "phi": 10,
            "mass": 10,
            "mt": 10,
            "et": 5
            }

    jet_vs_jet_rebin = {
            "ptrel": 5,
            "dr": 5,
            "deta": 5,
            "dphi": 5
            }

    def __init__(self):
        root.template.Templates.__init__(self)

        self.use_folders = []
        self.ban_folders = []

        self.use_plots = []
        self.ban_plots = {}

    def process_plot(self, template):
        if ((self.use_plots
                and template.name in self.use_plots
                and template.name not in self.ban_plots)

            or (not self.use_plots and template.name not in self.ban_plots)):

            {
                "/top": self.process_plot_top,
                "/jet1": self.process_plot_jet,
                "/jet2": self.process_plot_jet,
                "/jet3": self.process_plot_jet,
                "/jet1_parton": self.process_plot_parton,
                "/jet2_parton": self.process_plot_parton,
                "/jet1_vs_jet2": self.process_plot_jet_vs_jet
            }.get(template.path.split(':', 1)[1],
                  lambda plot: None)(template)

    def process_folder(self, folder, path, callback):
        if ((self.use_folders
                and path in self.use_folders
                and path not in self.ban_folders)

            or (not self.use_folders and path not in self.ban_folders)):

            root.template.find_plots(folder, path, callback)

    def process_plot_top(self, plot):
        self.set_title_all(plot, "htop")
        self.rebin_all(plot, self.top_rebin)

        if "njets" == plot.name:
            integral = plot.hist.Integral()

            print("{0:->80}".format(" Hadronic top Njets composition --"))
            print(" {0:>3}    {1}".format("bin", "percent"))
            print("-" * 80)
            for b, percent in dict(
                    (x, plot.hist.GetBinContent(plot.hist.FindBin(x)) / integral)
                        for x in range(1, 10)).items():
                print(" {0:>3} .. {1:.2f} %".format(b, percent))
            print("-" * 80)
            print()

        self.plots.append(plot)

    def process_plot_jet(self, plot):
        self.set_title_all(plot, plot.path.split("/")[-1])
        self.rebin_all(plot, self.jet_rebin)

        self.plots.append(plot)

    def process_plot_parton(self, plot):
        self.set_title_all(plot, plot.path.split("/")[-1])
        self.rebin_all(plot, self.parton_rebin)

        self.plots.append(plot)

    def process_plot_jet_vs_jet(self, plot):
        self.set_title_all(plot, plot.path.split('/')[-1])
        self.rebin_all(plot, self.jet_vs_jet_rebin)

        self.plots.append(plot)

    def set_title_all(self, plot, axis_subtitle = None):
        if 2 == plot.dim:
            self.set_title(plot,
                           str_match = str.endswith,
                           get_axis = ROOT.TH1.GetYaxis,
                           axis_subtitle = axis_subtitle)

        self.set_title(plot,
                       str_match = str.startswith,
                       get_axis = ROOT.TH1.GetXaxis,
                       axis_subtitle = axis_subtitle)

    def set_title(self, plot, str_match, get_axis, axis_subtitle = None):
        for axis_name, axis_title in self.axis_titles.items():
            if str_match(plot.name, axis_name):
                if axis_subtitle:
                    axis_title += "^{" + axis_subtitle + "}"

                units = self.units.get(axis_name)
                if units:
                    axis_title += " " + units

                get_axis(plot.hist).SetTitle(axis_title)

                break

    def rebin_all(self, plot, axis_binning):
        if 2 == plot.dim:
            self.rebin(plot,
                       str_match = str.endswith,
                       axis_rebin = ROOT.TH2.RebinY,
                       axis_binning = axis_binning)

        self.rebin(plot,
                   str_match = str.startswith,
                   axis_rebin = ROOT.TH1.Rebin if 1 == plot.dim else ROOT.TH2.RebinX,
                   axis_binning = axis_binning)

    def rebin(self, plot, str_match, axis_rebin, axis_binning):
        for axis_name, binning in axis_binning.items():
            if str_match(plot.name, axis_name):
                axis_rebin(plot.hist, binning)
                
                break

if "__main__" == __name__:
    templates = HadronicTopTemplates()
    templates.use_folders = ["jet{0}".format(x) for x in range(5)] + [ "", "top"]
    templates.ban_folders = ["jet{0}".format(x) for x in range(2, 5, 2)]
    templates.ban_plots = [ "px", "py", "pz", "phi" ]
    templates.load(sys.argv[1])

    print(templates)

    canvases = templates.draw()

    raw_input("enter")
