#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 28, 2012
Copyright 2011, All rights reserved
'''

from channel_template import ChannelTemplate
from channel_type import ChannelType
from input_template import InputTemplateLoader

class ChannelTemplateLoader(object):
    def __init__(self, filename):
        self.__plots = {}
        self.__filename = filename

        self.use_folders = []
        self.use_plots = []

    @property
    def plots(self):
        return self.__plots

    def load(self, channel_types):
        self.__plots = {}

        for channel_type in channel_types:
            channel_plots = self.load_channel(channel_type)

            for name, channel in channel_plots.items():
                channels = self.plots.get(name)
                if not channels:
                    channels = []
                    self.plots[name] = channels

                channels.append(channel)

    def load_channel(self, channel_type):
        channel_plots = {}

        for input_type in ChannelType(channel_type).allowed_inputs:
            templates = self.load_input(input_type)

            for name, template in templates.items():
                channel = channel_plots.get(name)
                if not channel:
                    channel = ChannelTemplate(channel_type)
                    channel_plots[name] = channel

                channel.add(template)

        return channel_plots

    def load_input(self, input_type):
        loader = InputTemplateLoader(input_type)
        loader.use_folders.extend(self.use_folders)
        loader.use_plots.extend(self.use_plots)
        loader.load("{0}/{1}".format(loader.type, self.__filename))

        return loader.templates
