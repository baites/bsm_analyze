#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 21, 2012
Copyright 2011, All rights reserved
'''

import random

class BaseType(object):
    '''
    Base for managable types. It's type can only be set if allowed by
    __contains__ method. Otherwise AttributeError exception is raised.

    Child classes should define policy by overloading __contains__ operator,
    e.g.:

        class Human(BaseType):
            genders = set(["male", "female"])

            def __init__(self, gender):
                BaseType.__init__(self, gender)

            def __contains__(self, value):
                return value in self.genders

    Child class may define a name for the attribute where type value will
    be saved. If not, then random variable is used for this
    '''

    def __init__(self, obj_type, attribute_name = None, fixed = True):
        if attribute_name:
            self.__attribute_name = "__{0}".format(attribute_name)
        else:
            self.__attribute_name = "__{0:x}".format(random.getrandbits(128))

        # allow type to be set first
        self.__fixed = False

        self.type = obj_type

        # fix type if asked to
        self.__fixed = fixed

    @property
    def type(self):
        '''
        Get type value
        '''

        return getattr(self, self.__attribute_name)

    @type.setter
    def type(self, value):
        '''
        Type attribute is only set if allowed by __contains__ method
        '''

        if self.__fixed:
            raise AttributeError("can not change fixed type")

        if value not in self:
            raise AttributeError("unsupported type {0}".format(value))
        else:
            setattr(self, self.__attribute_name, value)

    @type.deleter
    def type(self):
        '''
        Remove type attribute
        '''

        delattr(self, self.__attribute_name)

    def __contains__(self, value):
        '''
        Do not allow any type by default
        '''

        return False

    def __str__(self):
        '''
        Nice print
        '''

        return "<{Class} {Type} at 0x{ID:x}>".format(
                        Class = self.__class__.__name__,
                        ID = id(self),
                        Type = self.type
                    )

if "__main__" == __name__:
    class Human(BaseType):
        '''
        Example of BaseType inheritance and type policy definition
        '''

        genders = set(["male", "female"])

        def __init__(self, gender, fixed = True):
            '''
            Initialize type with gender value and make type dynamically
            fixable. Note, the actual type value will be saved in random
            variable
            '''

            BaseType.__init__(self, gender, fixed = fixed)

        def __contains__(self, value):
            '''
            Look up for defined type in the list of allowed values
            '''

            return value in self.genders

    import unittest

    class TestBase(unittest.TestCase):
        '''
        Test BaseType functionality
        '''

        def test_empty(self):
            '''
            By default BaseType does not allow any type to be set
            '''

            self.assertRaises(AttributeError, BaseType, "hello")

    class TestHuman(unittest.TestCase):
        def test_male(self):
            '''
            Try to create male (allowed) Human type
            '''

            human = Human("male")
            self.assertEqual(human.type, "male")

        def test_female(self):
            '''
            Try to create female (allowed) Human type
            '''

            human = Human("female")
            self.assertEqual(human.type, "female")

        def test_ape(self):
            '''
            Try to create non-existing ape Human type
            '''

            self.assertRaises(AttributeError, Human, "ape")

        def test_fixed(self):
            '''
            Try to change fixed Human type
            '''

            human = Human("male")
            try:
                human.type = "female"
            except AttributeError:
                pass

            self.assertEqual(human.type, "male")

        def test_not_fixed(self):
            '''
            Try to change non-fixed Human type
            '''

            human = Human("male", fixed = False)
            try:
                human.type = "female"
            except AttributeError:
                pass

            self.assertEqual(human.type, "female")


    unittest.main()
