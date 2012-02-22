#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 14, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function

import os
import sys

import label
import tfile

import ROOT

def find_plots(directory, path, callback):
    '''
    Find path in directory and scan it for histogrmas and subfolder.
    User-defined  callback function(s) are called depending on the object type:

        plot    is called for every 1..3 Dimension(s) plot found
        folder  is called for every sub-TDirectory
    '''

    plot_callback = callback.get("plot")
    folder_callback = callback.get("folder")

    # do nothing if none of the callback functions is defined
    if not plot_callback and not folder_callback:
        return

    # make sure path can be found in the directory
    folder = directory.GetDirectory(path)
    if not folder:
        print("sub-dir {0!r} is not found in {1!r}".format(
                path,
                directory.GetPath()),
            file = sys.stderr)

        return

    # scan through all available objects in current folder
    for name in (x.GetName() for x in folder.GetListOfKeys()):
        obj = folder.Get(name)
        if not obj:
            print("failed to exract object {0!r} in {1!r}".format(
                    name,
                    folder.GetPath()),
                file = sys.stderr)

            continue

        if isinstance(obj, ROOT.TH1):
            # process template
            if not plot_callback:
                continue

            h = obj.Clone()
            if not h:
                print("failed to clone object", file = sys.stderr)

                continue

            h.SetDirectory(0)

            plot = Template()
            plot.path = folder.GetPath()
            plot.hist = h

            plot_callback(plot)

        elif isinstance(obj, ROOT.TDirectory):
            # process sub-foler
            if folder_callback:
                folder_callback(folder, name, callback)

        else:
            print("unsupported object {0!r} of type {1}".format(
                    name,
                    obj.__class__.__name__),
                file = sys.stderr)

class Template(object):
    '''
    Container for template. User should be using next properties to
    access information:

        path    path in file to the template (string path w.r.t. file)
        name    template name
        dim     template dimension (is set automatically when histogram is set)
        hist    template object

    Only 1D, 2D and 3D plots are supported. Only several properties have
    write access:

        path
        hist

    The others are automatically extracted from histogram object
    '''

    def __init__(self, hist = None, path = "", clone = False):
        self.__clone = clone

        self.hist = hist # name and dimension are automatically set
        self.path = path

    @property
    def path(self):
        '''
        Full path to template in file including filename, e.g.:

            input_file.root:/path/to/template
        '''

        return self.__path

    @path.setter
    def path(self, value):
        self.__path = value

    @path.deleter
    def path(self, value):
        del self.__path


    @property
    def name(self):
        '''
        Template name
        '''

        return self.__name


    @property
    def dim(self):
        '''
        Template dimension
        '''

        return self.__dim


    @property
    def hist(self):
        '''
        Template histogram object
        '''

        return self.__hist

    @hist.setter
    def hist(self, obj):
        '''
        Set hist object only in case it has a supported Dimension value.
        Othewise, set hist to None
        '''

        if obj:
            dim = obj.GetDimension()
            if dim not in [1, 2, 3]:
                raise ValueError(
                        "unsupported template dimension {0}".format(dim))

            self.__dim = dim
            self.__name = obj.GetName()

            if self.__clone:
                obj = obj.Clone()
                obj.SetDirectory(0)

            self.__hist = obj
        else:
            self.__name = ""
            self.__dim = None
            self.__hist = None

    @hist.deleter
    def hist(self):
        '''
        Remove hist property
        '''

        del self.__hist

    def __nonzero__(self):
        '''
        Python 2.x hook
        '''

        return self.__bool__()

    def __bool__(self):
        '''
        Template is assumed to be valid only in case its dimension
        and histogram are set
        '''

        return bool(self.hist and self.dim)

    def __str__(self):
        '''
        Nice-print of the template
        '''

        if self:
            format_string = "<{Class} {name!r} {dim}D in {path!r} at 0x{ID:x}>"
        else:
            format_string = "<{Class} invalid template at 0x{ID:x}>"

        return format_string.format(
                    Class = self.__class__.__name__,
                    dim = self.dim,
                    name = self.name,
                    path = self.path,
                    ID = id(self))

    # restrict properties of the template
    __slots__ = [
            "__path",
            "__name",
            "__dim",
            "__hist",
            "__clone"
            ]

class Templates(object):
    '''
    Manage templates/plots saved in ROOT file

    managing includes generic tasks:

        load()  find recursively all plots in the ROOT file
        draw()  draw all the plots separately
    '''
    def __init__(self):
        self.plots = []

    def load(self, filename):
        '''
        Load plots from input file. Skip loading if plots were loaded before.
        An Exception will be raised if input file does not exist or there
        were ploblems opening it
        '''

        if not os.path.exists(filename):
            raise Exception("input file does not exist: " + filename)

        with root.tfile.topen(filename) as in_file:
            # Scan file recursively for plots
            find_plots(in_file,
                       "",
                       callback = {
                           "plot": self.process_plot,
                           "folder": self.process_folder
                           }
                       )

    def draw(self):
        '''
        Draw all found plots and return collection of created canvases. Method
        will automatically load plots if these are not loaded yet
        '''

        canvases = []
        for plot in self.plots:
            c = self.draw_plot(plot)

            if c:
                canvases.append(c)

        return canvases

    def process_plot(self, template):
        '''
        Callback for every template loaded
        '''

        self.plots.append(template)

    def process_folder(self, folder, path, callback):
        '''
        Callback for subfolders
        '''

        find_plots(folder, path, callback)

    def draw_plot(self, template):
        '''
        Called when new template is about to be plotted
        '''

        c = ROOT.TCanvas()

        if template.dim == 2:
            template.hist.GetZaxis().SetLabelSize(0)

            template.hist.Draw("colz 9")
       
        elif template.dim == 1:
            template.hist.Draw("hist 9")

        else:
            template.hist.Draw()

        return c

    def __nonzero__(self):
        '''
        Hook for Python 2.x
        '''

        return self.__bool__()

    def __bool__(self):
        return bool(self.plots)

    def __str__(self):
        '''
        Nice print of the object: list all loaded plots
        '''

        return "{0:-<80}\n{1}\n{2:-<80}".format(
                    "-- found {0} plots ".format(len(self.plots)),
                    "\n".join(map(str, self.plots)),
                    "-")

if "__main__" == __name__:
    import unittest
    import ROOT

    class TestTemplate(unittest.TestCase):
        def test_template_path(self):
            template = Template()
            self.assertEqual(template.path, "")

        def test_template_name(self):
            template = Template()
            self.assertEqual(template.name, "")

        def test_template_dim(self):
            template = Template()
            self.assertEqual(template.dim, None)

        def test_template_hist(self):
            template = Template()
            self.assertEqual(template.hist, None)

        def test_template_hist_no_clone(self):
            template = Template()
            hist = ROOT.TH1F("hist_1d", "hist_1d", 10, 0, 10)
            template.hist = hist

            self.assertEqual(template.hist, hist)

        def test_template_hist_clone(self):
            template = Template(clone = True)
            hist = ROOT.TH1F("hist_1d", "hist_1d", 10, 0, 10)
            template.hist = hist

            self.assertNotEqual(template.hist, hist)

        def test_hist_1d_path(self):
            template = Template()
            template.hist = ROOT.TH1F("hist_1d", "hist_1d", 10, 0, 10)

            self.assertEqual(template.path, "")

        def test_hist_1d_name(self):
            template = Template()
            template.hist = ROOT.TH1F("hist_1d", "hist_1d", 10, 0, 10)

            self.assertEqual(template.name, "hist_1d")

        def test_hist_1d_dim(self):
            template = Template()
            template.hist = ROOT.TH1F("hist_1d", "hist_1d", 10, 0, 10)

            self.assertEqual(template.dim, 1)

        def test_hist_1d_hist(self):
            template = Template()
            hist = ROOT.TH1F("hist_1d", "hist_1d", 10, 0, 10)
            template.hist = hist

            self.assertEqual(template.hist, hist)

        def test_hist_2d_path(self):
            template = Template()
            template.hist = ROOT.TH2F("hist_2d", "hist_2d", 10, 0, 10, 10, 0, 10)

            self.assertEqual(template.path, "")

        def test_hist_2d_name(self):
            template = Template()
            template.hist = ROOT.TH2F("hist_2d", "hist_2d", 10, 0, 10, 10, 0, 10)

            self.assertEqual(template.name, "hist_2d")

        def test_hist_2d_dim(self):
            template = Template()
            template.hist = ROOT.TH2F("hist_2d", "hist_2d", 10, 0, 10, 10, 0, 10)

            self.assertEqual(template.dim, 2)

        def test_hist_2d_hist(self):
            template = Template()
            hist = ROOT.TH2F("hist_2d", "hist_2d", 10, 0, 10, 10, 0, 10)
            template.hist = hist

            self.assertEqual(template.hist, hist)

        def test_hist_3d_path(self):
            template = Template()
            template.hist = ROOT.TH3F("hist_3d", "hist_3d",
                    10, 0, 10, 10, 0, 10, 10, 0, 10)

            self.assertEqual(template.path, "")

        def test_hist_3d_name(self):
            template = Template()
            template.hist = ROOT.TH3F("hist_3d", "hist_3d",
                    10, 0, 10, 10, 0, 10, 10, 0, 10)

            self.assertEqual(template.name, "hist_3d")

        def test_hist_3d_dim(self):
            template = Template()
            template.hist = ROOT.TH3F("hist_3d", "hist_3d",
                    10, 0, 10, 10, 0, 10, 10, 0, 10)

            self.assertEqual(template.dim, 3)

        def test_hist_3d_hist(self):
            template = Template()
            hist = ROOT.TH3F("hist_3d", "hist_3d",
                    10, 0, 10, 10, 0, 10, 10, 0, 10)
            template.hist = hist

            self.assertEqual(template.hist, hist)

    unittest.main()

    '''
    templates = Templates()
    templates.load("output_signal_p150_hlt.root")
    print(templates)
    '''
