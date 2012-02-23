#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 21, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function

import sys

from input_template import InputTemplatesLoader
from channel_type import ChannelType
from channel_template import ChannelTemplate

import ROOT

def usage(argv):
    return "usage: {0} templates.cfg".format(argv[0])

def main(argv = sys.argv):
    try:
        if 2 > len(argv):
            raise RuntimeError(usage(argv))

        #folders_to_use = ["", "htop", "ltop"]
        #plots_to_load = ["mttbar_after_htlep", "mass", "mt"]
        folders_to_use = [""]
        plots_to_load = ["mttbar_after_htlep"]

        # key: template.name    value: list of channels
        plots = {}
        for channel_type, input_types in {
                k: v for k, v in ChannelType.channel_types.items()
                     if k in ["ttbar", "zjets", "wjets", "stop"]}.items():

            loaded_channel_templates = []

            # load each file (input templates)
            for input_type in input_types:
                loader = InputTemplatesLoader(input_type)

                loader.use_folders.extend(folders_to_use)
                loader.use_plots.extend(plots_to_load)

                loader.load("{0}/output_signal_p150_hlt.root".format(input_type))

                for template in loader.input_templates.values():
                    full_path = template.path + '/' + template.name
                    if full_path not in plots:
                        plots[full_path] = []

                    channels = plots[full_path]
                    
                    # find channel where template should go 
                    for channel in channels:
                        if channel.type == channel_type:
                            break
                    else:
                        channel = ChannelTemplate(channel_type)
                        channels.append(channel)

                    channel.add(template)

        for k, v in plots.items():
            print("{0:-<80}".format("-- {0} ".format(k)))
            
            for c in v:
                print("{0:>10}: {1}".format(c.type, [x.type for x in c.input_templates]))

        print("draw it")
        canvas = ROOT.TCanvas()

        stack = ROOT.THStack()

        for channel in plots["/mttbar_after_htlep"]:
            print(channel.type)
            stack.Add(channel.hist)

        stack.Draw("9 hist")

        canvas.Update()

        print("press input")

        raw_input("enter")

    except Exception as error:
        print(error, file = sys.stderr)

        return 1
    else:
        return 0

if "__main__" == __name__:
    sys.exit(main())
