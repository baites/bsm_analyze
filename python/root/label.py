#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 14, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function, division

import ROOT

class Label(object):
    def __init__(self):
        self._label = None

    @property
    def label(self):
        return self._label

    @label.setter
    def label(self, header):
        if self:
            self.label.SetHeader(header)
        else:
            self._label = ROOT.TLegend(0, 0.9, 1, 1);

            self.label.SetFillColor(10)
            self.label.SetMargin(0.12);
            self.label.SetTextAlign(32)
            self.label.SetTextSize(0.12)

            self.label = header

    @label.deleter
    def label(self):
        del self._label

    def __nonzero__(self):
        return self.__bool__()

    def __bool__(self):
        return bool(self.label)

    def __str__(self):
        return "<{klass} {header!r} at 0x{id:x}>".format(
                klass = self.__class__.__name__,
                header = self.label.GetHeader() if self else "",
                id = id(self))

class CMSLabel(Label):
    def __init__(self):
        Label.__init__(self)
        self.label = "CMS Preliminary #sqrt{s} = 7 TeV"

        self.label.SetX1(.53)
        self.label.SetY1(.91)

        self.label.SetX2(.88)
        self.label.SetY2(.96)

class LuminosityLabel(Label):
    def __init__(self, luminosity):
        Label.__init__(self)
        self.label = ("L = {0:.2f}".format(luminosity / 1000) +
                        " fb^{-1}, e+jets")

        self.label.SetX1(.22)
        self.label.SetY1(.91)

        self.label.SetX2(.5)
        self.label.SetY2(.96)

if "__main__" == __name__:
    cms = CMSLabel()
    print(cms)
    print()

    luminosity = LuminosityLabel(4330)
    print(luminosity)
    print()

    label = Label()
    print(label)
    print()

    label.label = "Test label"
    print(label)
    print()
