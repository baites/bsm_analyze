#!/usr/bin/env python

'''
Created by Samvel Khalatyan, Feb 14, 2012
Copyright 2011, All rights reserved
'''

from __future__ import print_function

import os
import sys

import label
import root.tfile

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
    access/store template information:

        path    path in file to the template (string path w.r.t. file)
        name    template name (aka Key in TDirectory)
        dim     template dimension (is set automatically when histogram is set)
        hist    template object

    Note, only 1D, 2D and 3D plots are supported
    '''

    def __init__(self):
        self.path = ""
        self.name = ""
        self.dim = None
        self.hist = None

    @property
    def path(self):
        '''
        Full path to tempalte in file including filename, e.g.:

            input_file.root:/path/to/template
        '''

        return self._path

    @path.setter
    def path(self, value):
        self._path = value

    @path.deleter
    def path(self, value):
        del self._path


    @property
    def name(self):
        '''
        Tempalte name
        '''

        return self._name

    @name.setter
    def name(self, value):
        self._name = value

    @name.deleter
    def name(self, value):
        del self._name


    @property
    def dim(self):
        '''
        Template dimension
        '''

        return self._dim

    @dim.setter
    def dim(self, value):
        '''
        Set template dimension. Object will be invalidated if an attempt is
        made to set unsupported dimention
        '''

        if value in [1, 2, 3]:
            self._dim = value
        else:
            # invalidate object
            self._dim = None
            self._hist = None

    @dim.deleter
    def dim(self):
        '''
        Remove dimension property
        '''

        del seld._dim


    @property
    def hist(self):
        '''
        Template histogram object
        '''

        return self._hist

    @hist.setter
    def hist(self, obj):
        '''
        Set hist object only in case it has supported Dimension, othewise
        set hist to None
        '''

        if obj:
            self.dim = obj.GetDimension()
            if self.dim:
                self._hist = obj
                self.name = obj.GetName()
        else:
            self._dim = None
            self._hist = None

    @hist.deleter
    def hist(self):
        '''
        Remove hist property
        '''

        del self._hist

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
            return "<{dim}D {name!r} in {path!r} at 0x{id:x}>".format(
                    dim = self.dim,
                    name = self.name,
                    path = self.path,
                    id = id(self))
        else:
            return "<invalid template at 0x{0:x}>".format(id(self))

    # restrict properties of the template
    __slots__ = [
            "_path",
            "_name",
            "_dim",
            "_hist"
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
    a = Template()
    print(a)
    print()

    h = ROOT.TH1F("hist_1d", "hist_1d", 10, 0, 10)
    a.hist = h
    print(a)
    print()

    h = ROOT.TH2F("hist_2d", "hist_2d", 10, 0, 10, 10, 0, 10)
    a.hist = h
    print(a)
    print()

    templates = Templates()
    templates.load("output_signal_p150_hlt.root")
    print(templates)
