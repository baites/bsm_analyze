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

    def __init__(self, obj_type, attribute_name = None):
        if attribute_name:
            self.__attribute_name = "__{0}".format(attribute_name)
        else:
            self.__attribute_name = "__{0:x}".format(random.getrandbits(128))

        self.type = obj_type

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

        if value not in self:
            raise AttributeError("unsupported type {0}".format(value))
        else:
            setattr(self, self.__attribute_name, value)

    @type.deleter
    def type(self):
        delattr(self, self.__attribute_name)

    def __contains__(self, value):
        '''
        Do not allow any type by default
        '''

        return false

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
        genders = set(["male", "female"])

        def __init__(self, gender):
            BaseType.__init__(self, gender)

        def __contains__(self, value):
            return value in self.genders

    try:
        male = Human("male")
        print(male)
    except AttributeError as error:
        print(error)
    finally:
        print("-" * 80)

    try:
        female = Human("female")
        print(female)
    except AttributeError as error:
        print(error)
    finally:
        print("-" * 80)
    
    try:
        ape = Human("ape")
        print(ape)
    except AttributeError as error:
        print(error)
    finally:
        print("-" * 80)
