#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 28, 2012
Copyright 2011, All rights reserved
'''

from channel_template import ChannelTemplate, MCChannelTemplate
from channel_type import ChannelType
from input_template import InputTemplate
from input_type import InputType
from root.template import TemplateLoader

from util.timer import Timer

class InputTemplateLoader(InputType, TemplateLoader):
    def __init__(self, input_type):
        InputType.__init__(self, input_type)
        TemplateLoader.__init__(self)

        self.templates = {}

        self.use_folders = []
        self.ban_folders = []

        self.use_plots = []
        self.ban_plots = []

    @Timer(label = "[load templates from file]", verbose = True)
    def load(self, filename):
        self.templates = {}

        TemplateLoader.load(self, filename)

    def process_plot(self, template):
        if ((self.use_plots
                and template.name in self.use_plots
                and template.name not in self.ban_plots)

            or (not self.use_plots and template.name not in self.ban_plots)):

            self.templates[template.path + '/' +
                           template.name] = InputTemplate(self.type, template)

    def process_folder(self, folder, path):
        if ((self.use_folders
                and path in self.use_folders
                and path not in self.ban_folders)

            or (not self.use_folders and path not in self.ban_folders)):

            self.load_plots(folder, path)



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

        # all the cahnnels are loaded, combine MC
        for plot, channels in self.plots.items():
            mc = MCChannelTemplate("mc")
            for channel in channels:
                if channel.type in mc.allowed_inputs:
                    mc.add(channel)

            channels.append(mc)

    def load_channel(self, channel_type):
        channel_plots = {}

        for input_type in ChannelType(channel_type).allowed_inputs:
            templates = self.load_input(input_type)

            # merge loaded templates into channels
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

    def __str__(self):
        result = []
        for k, v in self.plots.items():
            result.append("{0:-<80}".format("-- {0} ".format(k)))
            
            for c in v:
                result.append("{0:>20}: {1}".format(c.type, [x.type for x in c.input_templates]))

        return '\n'.join(result)
