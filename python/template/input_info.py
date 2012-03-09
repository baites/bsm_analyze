#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Mar 01, 2012
Copyright 2011, All rights reserved
'''

class InputData(object):
    def __init__(self, rebin = None, units = None, title = None):
        self.__rebin = rebin
        self.__units = units
        self.__title = title

    @property
    def rebin(self):
        return self.__rebin

    @property
    def units(self):
        return self.__units

    @property
    def title(self):
        return self.__title



class InputInfo(object):
    '''
    Mix-in module that would work only in class with superclass:
    
        Template

    e.g.:

        class UserTemplate(Template, InputInfo):
            ...
    '''

    # cache strings
    __momentum_units = "GeV/c"
    __mass_units = "GeV/c^{2}"
    __mass_tev_units =  "TeV/c^{2}"

    input_infos = {
        "/d0": InputData(units = "cm", title = "d0"),
        "/htlep": InputData(units = __momentum_units,
            rebin = 25,
            title = "H_{T}^{lep}"),

        "/htall": InputData(units = __momentum_units,
            rebin = 25,
            title = "H_{T}^{all}"),

        "/htlep_after_htlep": InputData(units = __momentum_units,
            rebin = 25,
            title = "H_{T}^{lep}"),

        "/htlep_before_htlep": InputData(units = __momentum_units,
            rebin = 25,
            title = "H_{T}^{lep}"),

        "/htlep_before_htlep_qcd_noweight": InputData(units = __momentum_units,
            rebin = 25,
            title = "H_{T}^{lep}"),

        "/mttbar_before_htlep": InputData(rebin = 100,
            units = __mass_tev_units,
            title = "M_{t#bart}"),

        "/mttbar_after_htlep": InputData(rebin = 100,
            units = __mass_tev_units,
            title = "M_{t#bart}"),

        "/dr_vs_ptrel": InputData(units = ["", __momentum_units]),
        "/ttbar_pt": InputData(units = __momentum_units, title = "p_{T}^{t#bart}"),
        "/wlep_mt": InputData(units = __mass_units),
        "/whad_mt": InputData(units = __mass_units),
        "/wlep_mass": InputData(units = __mass_units),
        "/whad_mass": InputData(units = __mass_units),
        "/met": InputData(units = __mass_units, rebin = 25, title = "#slash{E}_{T}"),
        "/met_noweight": InputData(units = __mass_units, rebin = 25, title = "#slash{E}_{T}^{no weight}"),
        "/ltop_drsum": InputData(title = "#DeltaR_{sum}^{ltop}"),
        "/htop_drsum": InputData(title = "#DeltaR_{sum}^{htop}"),
        "/htop_dphi": InputData(title = "#Delta#phi(ltop,htop)"),
        "/ltop/mass": InputData(units = __mass_units,
            rebin = 25,
            title = "M^{ltop}"),

        "/ltop/eta": InputData(rebin = 50, title = "#eta^{ltop}"),

        "/htop/mass": InputData(units = __mass_units,
            rebin = 25,
            title = "M^{htop}"),

        "/htop/eta": InputData(rebin = 50, title = "#eta^{htop}"),

        "/ltop/pt": InputData(units = __momentum_units,
            rebin = 25,
            title = "p_{T}^{ltop}"),

        "/htop/pt": InputData(units = __momentum_units,
            rebin = 25,
            title = "p_{T}^{htop}"),

        "/Electron/pt": InputData(units = __momentum_units,
            rebin = 25,
            title = "p_{T}^{e}"),

        "/Electron/mass": InputData(units = __mass_units, title = "M^{e}"),
        "/Electron/eta": InputData(rebin = 50, title = "#eta^{e}"),

        "/First_jet/pt": InputData(units = __momentum_units,
            rebin = 25,
            title = "p_{T}^{jet1}"),

        "/First_jet/mass": InputData(rebin = 25,
            units = __mass_units,
            title = "M^{jet1}"),

        "/First_jet/eta": InputData(rebin = 50, title = "#eta^{jet1}"),

        "/Second_jet/pt": InputData(units = __momentum_units,
            rebin = 25,
            title = "p_{T}^{jet2}"),

        "/Second_jet/mass": InputData(rebin = 25,
            units = __mass_units,
            title = "M^{jet2}"),

        "/Second_jet/eta": InputData(rebin = 50, title = "#eta^{jet2}"),

        "/chi2": InputData(rebin = 10, title = "#Chi^{2}_{t#bar{t}}"),
        "/ltop_chi2": InputData(rebin = 10, title = "#Chi^{2}_{ltop}"),
        "/htop_chi2": InputData(rebin = 10, title = "#Chi^{2}_{htop}"),
            }

    @property
    def info(self):
        return self.input_infos.get(self.path + '/' + self.name, InputData())
