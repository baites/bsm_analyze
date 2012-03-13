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
    }

    def __init__(self, options, args):
        template.templates.Templates.__init__(self, options, args)

        if options.systematic:
            self.suffix = "__{0}__{1}".format(
                    options.systematic[:-1],
                    "plus" if options.systematic.endswith("+") else "minus")
        else:
            self.suffix = None

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
        self._fraction_fitter()
        self._apply_scales()

        self._save_templates()

    @Timer(label="[save templates]", verbose=True)
    def _save_templates(self):
        channels = self.loader.plots.get("/mttbar_after_htlep")
        if not channels:
            raise RuntimeError("mttbar_after_htlep is not loaded")

        format_string = "el_mttbar__{channel}"
        if self.suffix:
            format_string += "{suffix}"

        with topen(self.output_filename, "update"):
            for channel_type, channel in channels.items():
                if (channel_type not in self.channel_names or
                    (   self.save_channels and
                        channel_type not in self.save_channels)):

                    continue

                # All Zprimes are originally normalized to 5pb. Scale to 1pb
                if channel_type.startswith("zprime"):
                    channel.hist.Scale(1. / 5)

                channel.hist.Write(format_string.format(
                            channel=self.channel_names[channel_type],
                            suffix=self.suffix))
