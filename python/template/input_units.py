#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Mar 01, 2012
Copyright 2011, All rights reserved
'''

class InputUnits(object):
    __angle_units = "rad"
    __mass_units = "TeV/c^{2}"
    __momentum_units = "GeV/c"

    input_units = {
        "d0": ["cm"],
        "htlep": [__momentum_units],
        "htall": [__momentum_units],
        "htlep_after_htlep": [__momentum_units],
        "htlep_before_htlep": [__momentum_units],
        "htlep_before_htlep_qcd_noweight": [__momentum_units],
        "mttbar_before_htlep": [__mass_units],
        "mttbar_after_htlep": [__mass_units],
        "dr_vs_ptrel": ["", __momentum_units],
        "ttbar_pt": [__momentum_units],
        "wlep_mt": [__mass_units],
        "whad_mt": [__mass_units],
        "wlep_mass": [__mass_units],
        "whad_mass": [__mass_units],
        "met": [__momentum_units],
        "met_noweight": [__momentum_units],
        "ljet_met_dphi_vs_met_before_tricut": [__momentum_units, __angle_units],
        "lepton_met_dphi_vs_met_before_tricut": [__momentum_units, __angle_units],
        "ljet_met_dphi_vs_met": [__momentum_units, __angle_units],
        "lepton_met_dphi_vs_met": [__momentum_units, __angle_units],
        "htop_njet_vs_m": ["", __mass_units],
        "htop_pt_vs_m": [__momentum_units, __mass_units],
        "htop_pt_vs_njets": [__momentum_units, ""],
        "htop_pt_vs_ltop_pt": [__momentum_units, __momentum_units],
        "/ltop/mass": [__mass_units],
        "/htop/mass": [__mass_units],
        "/ltop/pt": [__momentum_units],
        "/htop/pt": [__momentum_units],
            }

    @property
    def units(self):
        if self.path:
            plotname = self.path + "/" + self.name
        else:
            plotname = self.name

        return self.input_units.get(plotname, None)
