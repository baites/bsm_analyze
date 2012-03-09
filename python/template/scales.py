#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Mar 08, 2012
Copyright 2011, All rights reserved
'''

class Scales(object):
    def __init__(self):
        self.__scales = {}
        
    def load(self, filename):
        with open(filename) as in_file:
            for line in in_file:
                line = line.strip()

                if line.startswith("#"):
                    continue

                channel, scale = line.split()

                self.__scales[channel] = float(scale)

    @property
    def scales(self):
        return self.__scales
