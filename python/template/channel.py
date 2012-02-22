#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 22, 2012
Copyright 2011, All rights reserved
'''

from base_type import BaseType

class ChannelType(BaseType):
    '''
    Specify channel types for plots. These are merged input types in some
    cases, for example: stop_s, stop_t, stop_tw, etc. are merged into stop
    (single top)
    '''

    # Generate allowed channel types from input types and merge single tops
    # into one channel
    channel_types = set([
            "ttbar",
            "zjets",
            "wjets",
            "stop",
            "data",
            "zprime_m1000_w10",
            "zprime_m1500_w15",
            "zprime_m2000_w20",
            "zprime_m3000_w30",
            "zprime_m4000_w40"
            ])

    def __init__(self, channel_type):
        BaseType.__init__(self, channel_type, "channel_type")

    def __contains__(self, value):
        return (value in self.channel_types or
                BaseType.__contains__(self, value))

if "__main__" == __name__:
    import unittest

    class TestChannelType(unittest.TestCase):
        def test_ttbar(self):
            input_type = ChannelType("ttbar")
            self.assertEqual(input_type.type, "ttbar")

        def test_zjets(self):
            input_type = ChannelType("zjets")
            self.assertEqual(input_type.type, "zjets")

        def test_wjets(self):
            input_type = ChannelType("wjets")
            self.assertEqual(input_type.type, "wjets")

        def test_stop(self):
            input_type = ChannelType("stop")
            self.assertEqual(input_type.type, "stop")

        def test_data(self):
            input_type = ChannelType("data")
            self.assertEqual(input_type.type, "data")

        def test_zprime_m1000_w10(self):
            input_type = ChannelType("zprime_m1000_w10")
            self.assertEqual(input_type.type, "zprime_m1000_w10")

        def test_zprime_m1500_w15(self):
            input_type = ChannelType("zprime_m1500_w15")
            self.assertEqual(input_type.type, "zprime_m1500_w15")

        def test_zprime_m2000_w20(self):
            input_type = ChannelType("zprime_m2000_w20")
            self.assertEqual(input_type.type, "zprime_m2000_w20")

        def test_zprime_m3000_w30(self):
            input_type = ChannelType("zprime_m3000_w30")
            self.assertEqual(input_type.type, "zprime_m3000_w30")

        def test_zprime_m4000_w40(self):
            input_type = ChannelType("zprime_m4000_w40")
            self.assertEqual(input_type.type, "zprime_m4000_w40")

        def test_qcd(self):
            self.assertRaises(AttributeError, ChannelType, "qcd")

    unittest.main()
