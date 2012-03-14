#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Mar 13, 2012
Copyright 2011, All rights reserved
'''

from util.timer import Timer
from template.loader import ChannelTemplateLoader


class Systematics(object):
    def __init__(self):
        self.nominal = None
        self.plus = None
        self.minus = None


class SystematicLoader(object):
    def __init__(self,
                 input_filename,
                 channels,
                 systematic,
                 load_systematic=None):

        self._input_filename = input_filename
        self._channels = channels
        self._systematic = systematic
        self._load_systematic = load_systematic
        self.plots = {}

    @Timer(label="load systematics", verbose=True)
    def load(self):
        self.__load_nominal_channels()

        if not self._load_systematic or self._load_systematic["plus"]:
            self.__load_systematic_plus()

        if not self._load_systematic or self._load_systematic["minus"]:
            self.__load_systematic_minus()

    def __load_nominal_channels(self):
        loader = ChannelTemplateLoader(self._input_filename)
        loader.use_plots = ["mttbar_after_htlep"]
        loader.use_folders = [""]
        loader.load(self._channels)

        self.__add_plots(loader.plots, "nominal")

    def __load_systematic_plus(self):
        basename, extension = self._input_filename.rsplit(".")
        loader = ChannelTemplateLoader("{0}_{1}_plus.{2}".format(
            basename,
            self._systematic,
            extension))

        loader.use_plots = ["mttbar_after_htlep"]
        loader.use_folders = [""]
        loader.load(self._channels)

        self.__add_plots(loader.plots, "plus")

    def __load_systematic_minus(self):
        basename, extension = self._input_filename.rsplit(".")
        loader = ChannelTemplateLoader("{0}_{1}_minus.{2}".format(
            basename,
            self._systematic,
            extension))

        loader.use_plots = ["mttbar_after_htlep"]
        loader.use_folders = [""]
        loader.load(self._channels)

        self.__add_plots(loader.plots, "minus")

    def __add_plots(self, plots, systematic):
        for name, channels in plots.items():
            if name not in self.plots:
                channels_systematic = {}
                self.plots[name] = channels_systematic
            else:
                channels_systematic = self.plots[name]

            for channel_type, channel in channels.items():
                if channel_type not in channels_systematic:
                    systematics = Systematics()
                    channels_systematic[channel_type] = systematics
                else:
                    systematics = channels_systematic[channel_type]

                systematics.__dict__[systematic] = channel
