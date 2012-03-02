#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Mar 01, 2012
Copyright 2011, All rights reserved
'''

class InputData(object):
    def __init__(self, rebin = None, units = None):
        self.__rebin = rebin
        self.__units = units

    @property
    def rebin(self):
        return self.__rebin

    @property
    def units(self):
        return self.__units



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

    input_infos = {
        "/d0": InputData(units = "cm"),
        "/htlep": InputData(units = __momentum_units),
        "/htall": InputData(units = __momentum_units),
        "/htlep_after_htlep": InputData(units = __momentum_units, rebin = 25),
        "/htlep_before_htlep": InputData(units = __momentum_units, rebin = 25),
        "/htlep_before_htlep_qcd_noweight": InputData(units = __momentum_units, rebin = 25),
        "/mttbar_before_htlep": InputData(rebin = 100, units = __mass_units),
        "/mttbar_after_htlep": InputData(rebin = 100, units = __mass_units),
        "/dr_vs_ptrel": InputData(units = ["", __momentum_units]),
        "/ttbar_pt": InputData(units = __momentum_units),
        "/wlep_mt": InputData(units = __mass_units),
        "/whad_mt": InputData(units = __mass_units),
        "/wlep_mass": InputData(units = __mass_units),
        "/whad_mass": InputData(units = __mass_units),
        "/met": InputData(units = __mass_units, rebin = 25),
        "/met_noweight": InputData(units = __mass_units, rebin = 25),
        "/ltop/mass": InputData(units = __mass_units, rebin = 25),
        "/htop/mass": InputData(units = __mass_units, rebin = 25),
        "/ltop/pt": InputData(units = __momentum_units, rebin = 25),
        "/htop/pt": InputData(units = __momentum_units, rebin = 25),
            }

    @property
    def info(self):
        return self.input_infos.get(self.path + '/' + self.name, InputData())
