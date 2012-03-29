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

                if not line or line.startswith("#"):
                    continue

                if ":" in line:
                    channel, scale = line.split(':')
                else:
                    channel, scale = line.split()

                self.__scales[channel.strip().lower()] = float(scale.strip())

    @property
    def scales(self):
        return self.__scales

    def __str__(self):
        return str(self.scales)
