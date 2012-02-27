#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 24, 2012
Copyright 2011, All rights reserved
'''

from __future__ import division

import math

class StatError(object):
    '''
    Wrapper around histogram property that acts like property but allows to
    intercept get/set operations and alter histogram errors on fly, e.g.:

        ...

        @CustomStatError(10)
        @property
        def hist(self, *parg, **karg):
            return SuperClass.hist.__get__(self, *parg, **karg)

        @hist.setter
        def hist(self, instance, value)
            SuperClass.hist.__set__(self, instance, value)

        ...

    In the above example each histogram error is modified by adding 10% of the
    bin content.

    By default, StatError does not change histogram the histogram, and should
    be inherited to add errors, e.g.:

        class CustomStatError(StatError):
            def __init__(self, percent):
                SuperClass.__init__(self, percent)

            def __set__(self, instance, value):
                SuperClass.__set__(self, instance, value)

                # Modify errors in the histogram
                self.add_error(instance.hist)

    The CustomStatError class will automatically adjust the errors of each bin
    in the histogram whenever a new plot is assigned to hist property.

    More than one error contributions can be applied to the same histogram,
    e.g.:

        ...

        @CustomStatError(4.5)
        @CustomStatError(5)
        @CustomStatError(10)
        @property
        def hist(self, *parg, **karg):
            return SuperClass.hist.__get__(self, *parg, **karg)

        @hist.setter
        def hist(self, instance, value)
            SuperClass.hist.__set__(self, instance, value)

        ...

    This way 4.5% + 5% + 10% error will be added.
    '''

    def __init__(self, percent):
        '''
        Initialize erorr descriptor
        '''

        self.__percent = percent / 100
        self.__wrapped = None   # property object to be wrapped

    def __call__(self, wrapped):
        '''
        Wrap property object, e.g.:

            class Klass(object):
                ...

                @StatError(10)
                @property
                def hist(self):
                    ...

                ...

        which is equivalent to:

            class Klass(object):
                ...

                def hist(self):
                    ...

                hist = @StatError(10)(property(Klass.hist))

                ...
        '''
        self.__wrapped = wrapped

        return self

    # Property interface
    def __get__(self, *parg, **karg):
        '''
        Route property get operation to property
        '''

        return self.__wrapped.__get__(*parg, **karg)

    def __set__(self, *parg, **karg):
        '''
        Route property set operation to property
        '''

        self.__wrapped.__set__(*parg, **karg)

    def __delete__(self, *parg, **karg):
        '''
        Route property delete operation to property
        '''

        self.__wrapped.__delete__(*parg, **karg)

    def getter(self, *parg, **karg):
        '''
        Set property getter function
        '''

        self.__wrapped = self.__wrapped.getter(*parg, **karg)

        return self

    def setter(self, *parg, **karg):
        '''
        Set property setter function
        '''

        self.__wrapped = self.__wrapped.setter(*parg, **karg)

        return self

    def deleter(self, *parg, **karg):
        '''
        Set property remove function
        '''

        self.__wrapped = self.__wrapped.deleter(*parg, **karg)

        return self

    def add_error(self, hist):
        '''
        Add error (custom percent of the bin content). Child classes should
        explicitly call method to adjust histogram errors
        '''

        # adjust plot errors
        for bin in range(1, hist.GetNbinsX() + 1):
            hist.SetBinError(bin,
                    math.sqrt(hist.GetBinError(bin) ** 2 +
                             (hist.GetBinContent(bin) * self.__percent) ** 2))



if "__main__" == __name__:
    import unittest
    import ROOT

    class CustomStatError(StatError):
        '''
        Example of how to extend StatError and adjust histogram errors when
        new plot is assigned
        '''

        def __init__(self, percent):
            StatError.__init__(self, percent)

        def __set__(self, instance, value):
            StatError.__set__(self, instance, value)

            # Adjust errors
            self.add_error(instance.hist)

    class HistNoErrorChange(object):
        '''
        Example of the histogram continaer whith StatErorr. By default, the
        Base error class does not change the plot errors
        '''

        # Custom gaussian, which is used to randomly fill histograms
        __gaus = ROOT.TF1("custom_gaus", "gaus(0)", 0, 10)
        __gaus.SetParameters(1, 5, 1) # random normalization, mean at 5, sigma 1

        def __init__(self):
            '''
            Create random histogram and store in instance
            '''

            h = ROOT.TH1F()
            h.SetBins(10, 0, 10);
            h.FillRandom("custom_gaus", 100)

            # for simplicity make each error to be equal to One
            for bin in range(1, h.GetNbinsX() + 1):
                h.SetBinError(bin, 1)

            self.hist = h

        @StatError(10)
        @property
        def hist(self):
            return self.__hist

        @hist.setter
        def hist(self, obj):
            self.__hist = obj

    class HistWithErrorChange(HistNoErrorChange):
        '''
        Example of how to add Errors to histogram:

            - Override property
            - pass property methods to super class
        '''

        def __init__(self):
            HistNoErrorChange.__init__(self)

        @CustomStatError(10)
        @property
        def hist(self, *parg, **karg):
            '''
            Route property access to super class
            '''

            return HistNoErrorChange.hist.__get__(self, *parg, **karg)

        @hist.setter
        def hist(self, *parg, **karg):
            '''
            Forward property set to super class
            '''

            HistNoErrorChange.hist.__set__(self, *parg, **karg)

    class HistWithManyErrorChange(HistNoErrorChange):
        '''
        Example of how to apply many errors to the same histogram. As in
        the example with one source of the errors, the idea is:

            - Override property
            - Pass all accessors to super class

        '''

        def __init__(self):
            HistNoErrorChange.__init__(self)

        @CustomStatError(4.5)
        @CustomStatError(4)
        @CustomStatError(10)
        @property
        def hist(self, *parg, **karg):
            return HistNoErrorChange.hist.__get__(self, *parg, **karg)

        @hist.setter
        def hist(self, *parg, **karg):
            HistNoErrorChange.hist.__set__(self, *parg, **karg)

    # Unit tests
    class TestHistNoErrorChange(unittest.TestCase):
        def setUp(self):
            self.__hist = HistNoErrorChange()

        def test_error(self):
            h = self.__hist.hist
            for bin in range(1, h.GetNbinsX() + 1):
                self.assertEqual(h.GetBinError(bin), 1)

    class TestHistWithErrorChange(unittest.TestCase):
        def setUp(self):
            self.__hist = HistWithErrorChange()

        def test_error(self):
            h = self.__hist.hist
            for bin in range(1, h.GetNbinsX() + 1):
                self.assertEqual("{0:.4f}".format(h.GetBinError(bin)),
                                 "{0:.4f}".format(
                                     math.sqrt(1 + (.1 * h.GetBinContent(bin)) ** 2)))

    class TestHistWithManyErrorChange(unittest.TestCase):
        def setUp(self):
            self.__hist = HistWithManyErrorChange()

        def test_error(self):
            h = self.__hist.hist
            for bin in range(1, h.GetNbinsX() + 1):
                self.assertEqual("{0:.4f}".format(h.GetBinError(bin)),
                                 "{0:.4f}".format(
                                     math.sqrt(
                                         1 + h.GetBinContent(bin) ** 2 *
                                            (0.04 ** 2 +
                                             0.045 ** 2 +
                                             0.1 ** 2))))

    unittest.main()
