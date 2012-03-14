#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 21, 2012
Copyright 2011, All rights reserved
'''

from __future__ import division

from base_type import BaseType

class InputData(object):
    '''
    Container for cross-section and number of events in the Monte-Carlo sample.
    Two properties are defined to prevent accidental change of values:
        
        xsection    Monte-Carlo process cross-section
        events      Number of processed events in the Monte-Carlo sample

    User still can alter these numbers by changing corresponding attributes
    with leading underscores
    '''

    def __init__(self, xsection, events):
        '''
        Initialialize Data with cross-section and number of processed events
        '''

        self.__xsection = xsection
        self.__events = events

    @property
    def xsection(self):
        '''
        Monte-Carlo cross-section for specific input
        '''

        return self.__xsection

    @property
    def events(self):
        '''
        Number of processed events for current input
        '''

        return self.__events



class InputType(BaseType):
    '''
    Basic Monte-Carlo input with type and corresponding cross-section and
    number of processed events.

    The supported type is defined by class input_types dictionary keys.
    User may add more types by expanding it, e.g.:

        InputType.input_types.update({
            "qcd_bctoe_pt20to30": InputData(123, 123456789)
        })
        qcd_bctoe_pt20to30 = InputType("qcd_bctoe_pt20to30")

    This way all instances of InputType will receive new type. In similar
    way specific type can be removed from all instances:

        InputType.input_types.pop("qcd_bctoe_pt20to30")
        try:
            qcd_bctoe_pt20to30 = InputType("qcd_bctoe_pt20to30")
        except AttributeError as error:
            print(error) # fill print error message as channel is not available

    Note: this is a dangerous, b/c instances of InputTypes will be affected
          by this change and will break the code. Make sure types are removed
          before any instances are created

    On the contrary, the prefered way to add more channels is through
    inheritance:

        from input_type import InputType

        class InputTypeWithQCD(InputType):
            # shallow-copy dictionary
            input_types = InputType.input_types.copy()

            # add new type(s)
            input_types.update({
                "qcd": InputData(102030, 112233)
            })

    Now, the original class InputType is not affected and all instances
    of INputTypeWithQCd may have newly added type: qcd

    Use read-only properties to access input data for given input:

        data        Pair of cross-sectiona and number of processed events
        xsection    Monte-Carlo cross-section for given type
        events      Number of processed events in the Monte-Carlo sample
        type        Input type

    Type can not be changed after it is set at object instantiation
    '''

    input_types = {
        # Use NNLO x-section: 163 instead of NLO: 157.5 or LO: 94.76
        "ttbar": InputData(163 * 1.0, 3701947),
        "ttbar_powheg": InputData(163 * 1.0, 16330372),

        "ttbar_scaling_up": InputData(163 * 1.0, 930483),
        "ttbar_scaling_down": InputData(163 * 1.0, 967055),

        "ttbar_matching_up": InputData(163 * 1.0, 1057479),
        "ttbar_matching_down": InputData(163 * 1.0, 1065323),

        # Use NLO x-section: 3048 instead of LO: 2475
        "zjets": InputData(3048 * 1.0, 36277961),

        # Use NLO x-section: 31314 instead of LO: 27770
        "wjets": InputData(31314 * 1.0, 77105816),

        "wjets_scaling_up": InputData(31314 * 1.0, 9784907),
        "wjets_scaling_down": InputData(31314 * 1.0, 10022324),

        "wjets_matching_up": InputData(31314 * 1.0, 10461655),
        "wjets_matching_down": InputData(31314 * 1.0, 9956679),

        "stop_s": InputData(3.19 * 1.0, 259971),
        "stop_t": InputData(41.92 * 1.0, 3900171),
        "stop_tw": InputData(7.87 * 1.0, 814390),
        "satop_s": InputData(1.44 * 1.0, 137980),
        "satop_t": InputData(22.65 * 1.0, 1944826),
        "satop_tw": InputData(7.87 * 1.0, 809984),

        "zprime_m1000_w10": InputData(5.0, 207992),
        "zprime_m1500_w15": InputData(5.0, 168383),
        "zprime_m2000_w20": InputData(5.0, 179315),
        "zprime_m3000_w30": InputData(5.0, 195410),
        "zprime_m4000_w40": InputData(5.0, 180381),

        # zero number of events will indicate: do-not scale
        "rereco_2011a_may10": InputData(1, 0),
        "rereco_2011a_aug05": InputData(1, 0),
        "prompt_2011a_v4": InputData(1, 0),
        "prompt_2011a_v6": InputData(1, 0),
        "prompt_2011b_v1": InputData(1, 0),

        # QCD From data
        "qcd_from_data": InputData(1, 0)
    }

    def __init__(self, input_type):
        '''
        Initialize with user defined input type
        '''

        BaseType.__init__(self, input_type, "input_type")

    @property
    def data(self):
        '''
        Access data container for current type
        '''

        return self.input_types[self.type]

    @property
    def events(self):
        '''
        Extract number of processed Monte-Carlo events
        '''

        return self.data.events

    @property
    def xsection(self):
        '''
        Exract Monte-Carlo cross-section for given type
        '''

        return self.data.xsection

    def __str__(self):
        '''
        Standard print plus input type, x-section and number of events
        '''

        # cache
        data = self.data
        return ("<{Class} {Type!r} xsec {XSection} events {Events:.0f} K "
                "at 0x{ID:x}>").format(
                        Class = self.__class__.__name__,
                        ID = id(self),
                        Type = self.type,
                        XSection = data.xsection,
                        Events = data.events / 1000
                    )

    def __contains__(self, value):
        '''
        Look up for type among supported channels. Child chlasses should
        overload this method to add new channels
        '''

        return (value in self.input_types or
                BaseType.__contains__(self, value))



if "__main__" == __name__:
    import unittest

    class InputTypeWithBCQCD(InputType):
        # shallow copy
        input_types = InputType.input_types.copy()

        # add new type
        input_types.update({
            "qcd_bc": InputData(102030, 112233)
        })

    class TestInputType(unittest.TestCase):
        '''
        Test InputType functionality
        '''

        def test_ttbar(self):
            input_type = InputType("ttbar")
            self.assertEqual(input_type.type, "ttbar")

        def test_ttbar_powheg(self):
            input_type = InputType("ttbar_powheg")
            self.assertEqual(input_type.type, "ttbar_powheg")

        def test_ttbar_scale_up(self):
            input_type = InputType("ttbar_scale_up")
            self.assertEqual(input_type.type, "ttbar_scale_up")

        def test_ttbar_scale_down(self):
            input_type = InputType("ttbar_scale_down")
            self.assertEqual(input_type.type, "ttbar_scale_down")

        def test_ttbar_matching_up(self):
            input_type = InputType("ttbar_matching_up")
            self.assertEqual(input_type.type, "ttbar_matching_up")

        def test_ttbar_matching_down(self):
            input_type = InputType("ttbar_matching_down")
            self.assertEqual(input_type.type, "ttbar_matching_down")

        def test_zjets(self):
            input_type = InputType("zjets")
            self.assertEqual(input_type.type, "zjets")

        def test_wjets(self):
            input_type = InputType("wjets")
            self.assertEqual(input_type.type, "wjets")

        def test_wjets_scale_up(self):
            input_type = InputType("wjets_scale_up")
            self.assertEqual(input_type.type, "wjets_scale_up")

        def test_wjets_scale_down(self):
            input_type = InputType("wjets_scale_down")
            self.assertEqual(input_type.type, "wjets_scale_down")

        def test_wjets_matching_up(self):
            input_type = InputType("wjets_matching_up")
            self.assertEqual(input_type.type, "wjets_matching_up")

        def test_wjets_matching_down(self):
            input_type = InputType("wjets_matching_down")
            self.assertEqual(input_type.type, "wjets_matching_down")

        def test_stop_s(self):
            input_type = InputType("stop_s")
            self.assertEqual(input_type.type, "stop_s")

        def test_stop_t(self):
            input_type = InputType("stop_t")
            self.assertEqual(input_type.type, "stop_t")

        def test_stop_tw(self):
            input_type = InputType("stop_tw")
            self.assertEqual(input_type.type, "stop_tw")

        def test_satop_s(self):
            input_type = InputType("satop_s")
            self.assertEqual(input_type.type, "satop_s")

        def test_satop_t(self):
            input_type = InputType("satop_t")
            self.assertEqual(input_type.type, "satop_t")

        def test_satop_tw(self):
            input_type = InputType("satop_tw")
            self.assertEqual(input_type.type, "satop_tw")

        def test_zprime_m1000_w10(self):
            input_type = InputType("zprime_m1000_w10")
            self.assertEqual(input_type.type, "zprime_m1000_w10")

        def test_zprime_m1500_w15(self):
            input_type = InputType("zprime_m1500_w15")
            self.assertEqual(input_type.type, "zprime_m1500_w15")

        def test_zprime_m2000_w20(self):
            input_type = InputType("zprime_m2000_w20")
            self.assertEqual(input_type.type, "zprime_m2000_w20")

        def test_zprime_m3000_w30(self):
            input_type = InputType("zprime_m3000_w30")
            self.assertEqual(input_type.type, "zprime_m3000_w30")

        def test_zprime_m4000_w40(self):
            input_type = InputType("zprime_m4000_w40")
            self.assertEqual(input_type.type, "zprime_m4000_w40")

        def test_rereco_2011a_may10(self):
            input_type = InputType("rereco_2011a_may10")
            self.assertEqual(input_type.type, "rereco_2011a_may10")

        def test_rereco_2011a_aug05(self):
            input_type = InputType("rereco_2011a_aug05")
            self.assertEqual(input_type.type, "rereco_2011a_aug05")

        def test_prompt_2011a_v4(self):
            input_type = InputType("prompt_2011a_v4")
            self.assertEqual(input_type.type, "prompt_2011a_v4")

        def test_prompt_2011a_v6(self):
            input_type = InputType("prompt_2011a_v6")
            self.assertEqual(input_type.type, "prompt_2011a_v6")

        def test_prompt_2011b_v1(self):
            input_type = InputType("prompt_2011b_v1")
            self.assertEqual(input_type.type, "prompt_2011b_v1")

        def test_qcd(self):
            self.assertRaises(AttributeError, InputType, "qcd")

        def test_expand_input_types(self):
            # Add new type
            InputType.input_types.update({
                "qcd_bc": InputData(102030, 112233)
            })

            # prove that new type exists
            input_type = InputType("qcd_bc")
            self.assertEqual(input_type.type, "qcd_bc")

            # remove newly added type
            InputType.input_types.pop("qcd_bc")

            # confirm that new type does not exist any more
            self.assertRaises(AttributeError, InputType, "qcd_bc")

    class TestInputTypeWithBCQCD(unittest.TestCase):
        '''
        Test InputTypeWithBCQCD functionality
        '''

        def test_create_new_type(self):
            # Create new type
            input_type = InputTypeWithBCQCD("qcd_bc")
            self.assertEqual(input_type.type, "qcd_bc")

        def test_input_type(self):
            # demonstrate that new type ditn't chane original InputType
            self.assertRaises(AttributeError, InputType, "qcd_bc")

        def test_ttbar_with_new_type(self):
            # Demonstrate that old types are still available
            input_type = InputTypeWithBCQCD("ttbar")
            self.assertEqual(input_type.type, "ttbar")

    unittest.main()
