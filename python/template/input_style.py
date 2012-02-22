#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 21, 2012
Copyright 2011, All rights reserved
'''

import ROOT
import channel

class Style(object):
    def __init__(self,
                 color = ROOT.kBlack,
                 marker_size = 0.5,
                 line_width = 2,
                 line_style = None):

        self.__color = color
        self.__marker_size = marker_size
        self.__line_width = line_width
        self.__line_style = line_style

    @property
    def color(self):
        return self.__color

    @property
    def marker_size(self):
        return self.__marker_size

    @property
    def line_width(self):
        return self.__line_width

    @property
    def line_style(self):
        return self.__line_style

    def __str__(self):
        return ("<{Class} color {color} marker size {marker_size} "
                "line width {line_width} at 0x{ID:x}>").format(
                        Class = self.__class__.__name__,
                        ID = id(self),
                        color = self.color,
                        marker_size = self.marker_size,
                        line_width = self.line_width)

class ChannelStyle(object):
    styles = {
        "ttbar": Style(color = ROOT.kRed + 1),
        "zjets": Style(color = ROOT.kBlue + 1),
        "wjets": Style(color = ROOT.kGreen + 1),
        "stop": Style(color = ROOT.kMagenta + 1),

        "zprime_m1000_w10": Style(line_style = 2),
        "zprime_m1500_w15": Style(line_style = 3),
        "zprime_m2000_w20": Style(line_style = 4),
        "zprime_m3000_w30": Style(line_style = 5)
    }

if "__main__" == __name__:
    style = ChannelStyle.styles[channel.ChannelType("ttbar").type]
    print(style)
