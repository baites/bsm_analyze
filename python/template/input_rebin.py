#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 23, 2012
Copyright 2011, All rights reserved
'''

class InputRebin(object):
    '''
    Mix-in module that would work only in a class whose superclasses are:

        root.template.Temlate
        input_rebin.InputRebin

    e.g.:

        class MyInputTemplate(Template, InputRebin):
            ...
    '''
    input_rebins = {
        "cutflow": [1],
        "npv": [1],
        "npv_with_pileup": [1],
        "njets": [1],
        "d0": [1],
        "htlep": [1],
        "htall": [1],
        "htlep_after_htlep": [25],
        "htlep_before_htlep": [1],
        "htlep_before_htlep_qcd_noweight": [1],
        "solutions": [1],
        "mttbar_before_htlep": [1],
        "mttbar_after_htlep": [100],
        "dr_vs_ptrel": [1, 1],
        "ttbar_pt": [1],
        "wlep_mt": [1],
        "whad_mt": [1],
        "wlep_mass": [1],
        "whad_mass": [1],
        "met": [1],
        "met_noweight": [1],
        "ljet_met_dphi_vs_met_before_tricut": [1, 1],
        "lepton_met_dphi_vs_met_before_tricut": [1, 1],
        "ljet_met_dphi_vs_met": [1, 1],
        "lepton_met_dphi_vs_met": [1, 1],
        "htop_njets": [1],
        "htop_delta_r": [1],
        "htop_njet_vs_m": [1, 1],
        "htop_pt_vs_m": [1, 1],
        "htop_pt_vs_njets": [1, 1],
        "htop_pt_vs_ltop_pt": [1, 1],
        "njets_before_reconstruction": [1],
        "njet2_dr_lepton_jet[1]_before_reconstruction": 1,
        "njet2_dr_lepton_jet2_before_reconstruction": [1],
        "njets_after_reconstruction": [1],
        "njet2_dr_lepton_jet[1]_after_reconstruction": 1,
        "njet2_dr_lepton_jet2_after_reconstruction": [1],
            }

    @property
    def rebin(self):
        if self.path:
            plotname = self.path + "/" + self.name
        else:
            plotname = self.name

        return self.input_rebins.get(plotname, [1])
