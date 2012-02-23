#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 23, 2012
Copyright 2011, All rights reserved
'''

import random

class Style(object):
    '''
    Minimalistic style for plots
    '''

    def __init__(self,
                 color = 1, # black by default
                 marker_size = 0.5,
                 line_width = 2,
                 line_style = None,
                 fill = True):

        self.__color = color
        self.__marker_size = marker_size
        self.__line_width = line_width
        self.__line_style = line_style
        self.__fill = fill

    @property
    def color(self):
        '''
        Histogram color. It should be applied to:
            - marker
            - line
            - fill (if fill flag is set)
        '''

        return self.__color

    @property
    def marker_size(self):
        '''
        Marker size
        '''

        return self.__marker_size

    @property
    def line_width(self):
        '''
        Width of the bar lines
        '''

        return self.__line_width

    @property
    def line_style(self):
        '''
        Style of the bar lines
        '''

        return self.__line_style

    @property
    def fill(self):
        '''
        Indicate whether histogram should be filled with specified color
        '''

        return self.__fill

    def apply(self, plot):
        plot.SetLineColor(self.color)
        if self.fill:
            plot.SetFillColor(self.color)

        plot.SetMarkerSize(self.marker_size)
        plot.SetLineWidth(self.line_width)

        if self.line_style:
            plot.SetLineStyle(self.line_style)

    def __str__(self):
        return ("<{Class} color {color} marker size {marker_size} "
                "line width {line_width} at 0x{ID:x}>").format(
                        Class = self.__class__.__name__,
                        ID = id(self),
                        color = self.color,
                        marker_size = self.marker_size,
                        line_width = self.line_width)

class BaseStyle(object):
    '''
    Base for all histogram styles. It can be set to certain values that are
    allowed by __contains__ method. Child classes should overload above
    method, e.g.:

        class HumanStyle(BaseStyle):
            human_styles = set(["male", "female"])

            def __init__(self, style):
                BaseStyle.__init__(self, style)

            def __contains__(self, style):
                # Test if style is allowed
                return (style in self.human_styles or
                        BaseStyle.__contains__(self, style))

    By default, BaseStyle does not allow any style to be set. The value of
    current style can be fixed if instance was created with fixed parameter
    set to True. Otherwise, style can be later changed, e.g.:

        class HumanStyle(BaseStyle):
            ...

            def __init__(self, style):
                BaseStyle.__init__(self, style, fixed = False)

            ...

        human = HumanStyle("male")
        human.style = "female"

    Child classes may also define attribute name for stored current style,
    e.g.:

        class HumanStyle(BaseStyle):
            ...

            def __init__(self, style):
                BaseStyle.__init__(self, style, attribute_name = "human_style")

            ...

    Otherwise a random variable name is used.
    '''

    def __init__(self, obj_style, attribute_name = None, fixed = True):
        '''
        Initialize style with value if allowed by __contains__, store in
        a variable with attribute_name (or random name if not set), and
        fix style if fixed argument is set to True
        '''

        if attribute_name:
            self.__attribute_name = "__{0}".format(attribute_name)
        else:
            self.__attribute_name = "__{0:x}".format(random.getrandbits(128))

        # allow style to be set first
        self.__fixed = False

        self.style = obj_style

        # fix style if asked to
        self.__fixed = fixed

    @property
    def style(self):
        '''
        Get current style
        '''

        return getattr(self, self.__attribute_name)

    @style.setter
    def style(self, value):
        '''
        Style attribute is only set if allowed by __contains__ method and not
        fixed
        '''

        if self.__fixed:
            raise AttributeError("can not change fixed style")

        if value not in self:
            raise AttributeError("unsupported style {0}".format(value))
        else:
            setattr(self, self.__attribute_name, value)

    @style.deleter
    def style(self):
        '''
        Remove style attribute
        '''

        delattr(self, self.__attribute_name)

    def __contains__(self, value):
        '''
        Do not allow any style by default
        '''

        return False

    def __str__(self):
        '''
        Nice print
        '''

        return "<{Class} {Style} at 0x{ID:x}>".format(
                        Class = self.__class__.__name__,
                        ID = id(self),
                        Style = self.style
                    )

if "__main__" == __name__:
    import unittest

    class HumanStyle(BaseStyle):
        human_styles = set(["male", "female"])

        def __init__(self, style, fixed = True):
            BaseStyle.__init__(self, style, fixed = fixed)

        def __contains__(self, style):
            return (style in self.human_styles or
                    BaseStyle.__contains__(self, style))

    class TestBaseStyle(unittest.TestCase):
        def test_empty(self):
            self.assertRaises(AttributeError, BaseStyle, "hello")

    class TestHumanStyle(unittest.TestCase):
        def test_male(self):
            human = HumanStyle("male")
            self.assertEqual(human.style, "male")

        def test_female(self):
            human = HumanStyle("female")
            self.assertEqual(human.style, "female")

        def test_ape(self):
            self.assertRaises(AttributeError, HumanStyle, "ape")

        def test_fixed(self):
            human = HumanStyle("male")
            try:
                human.style = "female"
            except AttributeError:
                pass

            self.assertEqual(human.style, "male")

        def test_not_fixed(self):
            human = HumanStyle("male", fixed = False)
            try:
                human.style = "female"
            except AttributeError:
                pass

            self.assertEqual(human.style, "female")

    unittest.main()
