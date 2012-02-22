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
        self.__xsection = xsection
        self.__events = events

    @property
    def xsection(self):
        return self.__xsection

    @property
    def events(self):
        return self.__events

class InputType(BaseType):
    '''
    Input with type. It holds information about allowed inputs, cross-section,
    and number of processed events.

    The supported channel is defined by inputs class dictionary keys. User may
    add more input type by expanding it, e.g.:

        InputType.input_types.update({
            "qcd_bctoe_pt20to30": InputData(123, 123456789)
        })
        qcd_bctoe_pt20to30 = InputType("qcd_bctoe_pt20to30")

    or disable/pop specific channels:

        InputType.input_types.pop("qcd_bctoe_pt20to30")
        try:
            qcd_bctoe_pt20to30 = InputType("qcd_bctoe_pt20to30")
        except AttributeError as error:
            print(error) # fill print error message as channel is not available

    however, the prefered way to add more channels is through inheritance:

        import InputType

        class InputTypeWithQCD(InputType):
            input_types = InputType.input_types.copy()
            inptu_types.update({
                "qcd": InputData(102030, 112233)
            })

    use read-only properties to automatically access data for given input:

        xsection    Monte-Carlo cross-section for given type
        events      Number of processed events in the Monte-Carlo sample

    type can be only set/changed if it is specified in the input_types
    '''

    input_types = {
        # Use NNLO x-section: 163 instead of NLO: 157.5 or LO: 94.76
        "ttbar": InputData(163 * 1.0, 3701947),

        # Use NLO x-section: 3048 instead of LO: 2475
        "zjets": InputData(3048 * 1.0, 36277961),

        # Use NLO x-section: 31314 instead of LO: 27770
        "wjets": InputData(31314 * 1.0, 77105816),

        "stop_s": InputData(3.19 * 1.0, 259971),
        "stop_t": InputData(41.92 * 1.0, 3900171),
        "stop_tw": InputData(7.87 * 1.0, 814390),
        "satop_s": InputData(1.44 * 1.0, 137980),
        "satop_t": InputData(22.65 * 1.0, 1944826),
        "satop_tw": InputData(7.87 * 1.0, 809984),

        "zprime_m1000_w10": InputData(1.0, 207992),
        "zprime_m1500_w15": InputData(1.0, 168383),
        "zprime_m2000_w20": InputData(1.0, 179315),
        "zprime_m3000_w30": InputData(1.0, 195410),

        "data": InputData(1, 1)
    }

    def __init__(self, input_type):
        BaseType.__init__(self, input_type, "input_type")

    @property
    def events(self):
        return self.data.events

    @property
    def xsection(self):
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

    @property
    def data(self):
        return self.input_types[self.type]

    def __contains__(self, value):
        '''
        Look up for type among supported channels. Child chlasses should
        overload this method to add new channels
        '''

        return (value in self.input_types or
                BaseType.__contains__(self, value))

# ------------------------------------------------------------------------------

if "__main__" == __name__:
    # create supported input and print on screen
    try:
        ttbar = InputType("ttbar")
        print(ttbar)
    finally:
        print("-" * 50)

    # attempt to create unsupported input
    try:
        qcd = InputType("qcd")
    except AttributeError as error:
        print(error)
    finally:
        print("-" * 50)

    # expand list of supported input_types and create type
    try:
        InputType.input_types.update({
            "qcd_bc": InputData(102030, 112233)
        })

        qcd_bc = InputType("qcd_bc")
        print(qcd_bc)
    except AttributeError as error:
        print(error)
    finally:
        print("-" * 50)

    # remove newly added input type and check if type can still be created
    InputType.input_types.pop("qcd_bc")

    try:
        qcd_bc = InputType("qcd_bc")
        print(qcd_bc)
    except AttributeError as error:
        print(error)
    finally:
        print("-" * 50)

    # add new types through inheritance and copy
    try:
        class InputTypeWithBCQCD(InputType):
            input_types = InputType.input_types.copy()
            input_types.update({
                "qcd_bc": InputData(102030, 112233)
            })

        qcd_bc = InputTypeWithBCQCD("qcd_bc")
        print(qcd_bc)
    except AttributeError as error:
        print(error)
    finally:
        print("-" * 50)

    # confirm that original list of input_types is not modified
    try:
        qcd_bc = InputType("qcd_bc")
        print(qcd_bc)
    except AttributeError as error:
        print(error)
    finally:
        print("-" * 50)
