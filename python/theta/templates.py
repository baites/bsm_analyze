#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Mar 12, 2012
Copyright 2011, All rights reserved
'''

import template.templates

from template import channel_type

from root.tfile import topen
from util.timer import Timer
from util.arg import split_use_and_ban

class Templates(template.templates.Templates):
    channel_names = {
            # Map channel type to channel name to be used in plot name

            "ttbar": "ttbar",
            "zjets": "zjets",
            "wjets": "wjets",
            "stop": "singletop",
            "qcd": "eleqcd",
            "data": "DATA",

            "zprime_m1000_w10": "zp1000",
            "zprime_m1500_w15": "zp1500",
            "zprime_m2000_w20": "zp2000",
            "zprime_m3000_w30": "zp3000",
            "zprime_m4000_w40": "zp4000",

            "ttbar_matching_plus": "ttbar",
            "ttbar_matching_minus": "ttbar",

            "ttbar_scaling_plus": "ttbar",
            "ttbar_scaling_minus": "ttbar",

            "wjets_matching_plus": "wjets",
            "wjets_matching_minus": "wjets",

            "wjets_scaling_plus": "wjets",
            "wjets_scaling_minus": "wjets",
    }

    def __init__(self, options, args):
        template.templates.Templates.__init__(
                self,
                options, args,
                disable_systematics=False if (options.systematic and
                                              ("scaling" in options.systematic or
                                               "matching" in options.systematic))
                                          else True)

        if options.systematic:
            self.suffix = "__{0}__{1}".format(
                    options.systematic[:-1],
                    "plus" if options.systematic.endswith("+") else "minus")
        else:
            self.suffix = None

        self.theta_prefix = options.thetaprefix
        self.output_filename = options.output

        if options.savechannels:
            use_channels, ban_channels = split_use_and_ban(set(
                channel.strip() for channel in options.savechannels.split(',')))

            # use only allowed channels or all if None specified
            channels = set(channel_type.ChannelType.channel_types.keys())
            if use_channels:
                channels &= use_channels

            # remove banned channels
            if ban_channels:
                channels -= ban_channels

            self.save_channels = list(channels)
        else:
            self.save_channels = []

    def _process(self):
        if not self.use_channels:
            raise RuntimeError("all channels are turned off")

        self._load_channels()
        self._run_fraction_fitter()
        self._apply_fractions()
        self._apply_scales()

        self._save_templates()

    @Timer(label="[save templates]", verbose=True)
    def _save_templates(self):
        '''
        Save loaded channels in output ROOT file
        
        Output file will be updated. All channels will be saved or only those,
        that are specified with --savechannels option.

        The histogram naming convention is as follows:

            [analysis]_[plot]__[channel][__[systematics]]

        where
        
            analysis    analysis channel: mu, el
            plot        plot name, e.g. mttbar
            channel     channel name: ttbar, zjets, wjets, singletop, etc.
            systematics source of the systematic error, e.g. jes__plus

        '''

        # Make sure required plot is loaded
        channels = self.loader.plots.get("/mttbar_after_htlep")
        if not channels:
            raise RuntimeError("mttbar_after_htlep is not loaded")

        # format string has different format with(-out) systematics
        format_string = "{prefix}_mttbar__{channel}".format(self.theta_prefix)
        if self.suffix:
            format_string += self.suffix

        with topen(self.output_filename, "update"):
            # save only those channels that are supported or specified by user
            for channel_type, channel in channels.items():
                if (channel_type not in self.channel_names or
                        (self.save_channels and
                         channel_type not in self.save_channels)):

                    continue

                # All Zprimes are originally normalized to 5pb. Scale to 1pb
                if channel_type.startswith("zprime"):
                    channel.hist.Scale(1. / 5)

                name = format_string.format(
                        channel=self.channel_names[channel_type])

                hist = channel.hist.Clone(name)
                hist.SetTitle(channel.hist.GetTitle())

                hist.Write(name)

    def __str__(self):
        '''
        Print Templates object configuraiton
        '''

        result = []

        result.append(["systematic", self.suffix if self.suffix else ""])
        result.append(["output filename", self.output_filename])
        result.append(["save channels", self.save_channels])

        return "{0}\n{1}".format(
                template.templates.Templates.__str__(self),
                '\n'.join(self._str_format.format(name, value)
                          for name, value in result))
