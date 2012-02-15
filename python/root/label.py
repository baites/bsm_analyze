#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 14, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function, division

import ROOT

class Label(object):
    '''
    The most generic legend by managing label proprty, e.g.:

        my_label = Label()
        my_label.label = "This is a generic label"
        ...
        my_label.Draw()
    '''

    def __init__(self):
        self._label = None

    @property
    def label(self):
        return self._label

    @label.setter
    def label(self, header):
        '''
        Set label header. Label object will be automatically created if it
        does not exist yet
        '''

        if self:
            self.label.SetTitle(header)
        else:
            self._label = ROOT.TLatex()

            self.label.SetTextSize(0.10)

            '''
            recursion to absorb Set Header procedure if it is changed in future,
            for example if some logic or setup is added
            '''

            self.label = header

    @label.deleter
    def label(self):
        del self._label

    def draw(self):
        self.label.DrawLatex(self.label.GetX(),
                             self.label.GetY(),
                             self.label.GetTitle())

    def __nonzero__(self):
        '''
        Python 2.x hook
        '''

        return self.__bool__()

    def __bool__(self):
        return bool(self.label)

    def __str__(self):
        '''
        Nice print for label with class name and header if any is set
        '''

        return "<{klass} {header!r} at 0x{id:x}>".format(
                klass = self.__class__.__name__,
                header = self.label.GetHeader() if self else "",
                id = id(self))

class CMSLabel(Label):
    '''
    CMS Label with experiment energy, specific size and location, etc.
    '''

    def __init__(self):
        Label.__init__(self)
        self.label = "CMS Preliminary #sqrt{s} = 7 TeV"

        self.label.SetX(.53)
        self.label.SetY(.91)

class CMSSimulationLabel(CMSLabel):
    '''
    CMS Simulation Label with experiment energy, specific size and location,
    etc.
    '''

    def __init__(self):
        CMSLabel.__init__(self)

        self.label = "CMS Simulation Preliminary"

class LuminosityLabel(Label):
    '''
    Luminosity label
    '''

    def __init__(self, luminosity):
        Label.__init__(self)
        self.label = ("L = {0:.2f}".format(luminosity / 1000) +
                        " fb^{-1}, e+jets")

        self.label.SetX(.02)
        self.label.SetY(.56)

if "__main__" == __name__:
    label = CMSLabel()
    print(label)
    print()

    label = CMSSimulationLabel()
    print(label)
    print()

    label = LuminosityLabel(4330)
    print(label)
    print()

    label = Label()
    print(label)
    print()

    label.label = "Test label"
    print(label)
    print()
