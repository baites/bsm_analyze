#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 23, 2012
Copyright 2011, All rights reserved
'''

import random

class Style(object):
    def __init__(self,
                 color = 1, # black by default
                 marker_size = 0.5,
                 line_width = 2,
                 line_style = None):

        self.__color = color
        self.__marker_size = marker_size
        self.__line_width = line_width
        self.__line_style = line_style

    @property
    def color(self):
        return self.__color

    @property
    def marker_size(self):
        return self.__marker_size

    @property
    def line_width(self):
        return self.__line_width

    @property
    def line_style(self):
        return self.__line_style

    def __str__(self):
        return ("<{Class} color {color} marker size {marker_size} "
                "line width {line_width} at 0x{ID:x}>").format(
                        Class = self.__class__.__name__,
                        ID = id(self),
                        color = self.color,
                        marker_size = self.marker_size,
                        line_width = self.line_width)

class BaseStyle(object):
    def __init__(self, obj_style, attribute_name = None, fixed = True):
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
        Get style value
        '''

        return getattr(self, self.__attribute_name)

    @style.setter
    def style(self, value):
        '''
        Style attribute is only set if allowed by __contains__ method
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
