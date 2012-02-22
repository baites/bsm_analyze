#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 21, 2012
Copyright 2011, All rights reserved
'''

import base_type
import input_type

class ChannelType(base_type.BaseType):
    '''
    Specify channel types for plots. These are merged input types in some
    cases, for example: stop_s, stop_t, stop_tw, etc. are merged into stop
    (single top)
    '''

    # Generate allowed channel types from input types and merge single tops
    # into one channel
    channel_types = set()
    for channel in input_type.InputType.inputs.keys():
        if channel.startswith("stop") or channel.startswith("satop"):
            channel_types.add("stop")
        else:
            channel_types.add(channel)

    def __init__(self, channel_type):
        base_type.BaseType.__init__(self, channel_type, "channel_type")

    def __contains__(self, value):
        '''
        Let type to be set only for pre-defined channel_types
        '''

        return value in self.channel_types

class Channel(object):
    def __init__(self):
        self.inputs = []
        self._plot = None

if "__main__" == __name__:
    try:
        ttbar = ChannelType("ttbar")
        print(ttbar)
    except AttributeError as error:
        print(error)
    finally:
        print("-" * 80)

    try:
        stop = ChannelType("stop")
        print(stop)
    except AttributeError as error:
        print(error)
    finally:
        print("-" * 80)

    try:
        stop_t = ChannelType("stop_t")
        print(stop_t)
    except AttributeError as error:
        print(error)
    finally:
        print("-" * 80)
