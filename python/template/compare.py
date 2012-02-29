#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 28, 2012
Copyright 2011, All rights reserved
'''

'''
Collection of different histogram comparison methods:

    ratio
        simple ratio of plots: S/N

    data_mins_bg_over_bg
        difference between S and N with respect to N: (S - N) / N
'''

from root.comparison import compare

@compare
def ratio(data, background, title = None):
    h = data.Clone()
    h.SetDirectory(0)
    h.Reset()

    h.Divide(data, background)

    h.GetYaxis().SetTitle(title if title else "#frac{Data}{BKGD}")

    return h

@compare
def data_mins_bg_over_bg(data, background, title = None):
    h = data.Clone()
    h.SetDirectory(0)

    h.Add(background, -1)
    h.Divide(background)

    h.GetYaxis().SetTitle(title if title else "#frac{Data - BKGD}{BKGD}")

    return h
