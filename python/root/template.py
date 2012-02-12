from __future__ import print_function, division

import os
import sys

import ROOT

class Templates(object):
    '''
    Manage templates/plots saved in ROOT file

    managing includes generic tasks:

        load()  find recursively all plots in the ROOT file
        draw()  draw all the plots separately
    '''
    luminosity = 0
    experiment_labels = {}

    def __init__(self, filename):
        '''
        Start managing templates in specific file
        '''

        self.__filename = filename
        self.__plots = []

        self.use_folders = []
        self.skip_folders = []

    def plots(self):
        '''
        Access loaded plots. Call load() first. Plots is a collection of
        dictionary items with structure:

            "path": path to plot in file
            "name": name of the plot
              "2d": True if plot is 2D otherwise False
            "hist": plot object
        '''

        return self.__plots

    def load(self):
        '''
        Load plots from input file. Skip loading if plots were loaded before.
        An Exception will be raised if input file does not exist or there
        were ploblems opening it
        '''

        # check if plots are already loaded
        if self.__plots:
            return

        if not os.path.exists(self.__filename):
            raise Exception("input file does not exist: " + self.__filename)

        in_file = ROOT.TFile.Open(self.__filename)
        if not in_file:
            raise Exception("failed to open input file: " + self.__filename)

        # Scan file recursively for plots
        self.__find_plots(in_file)

    def draw(self):
        '''
        Draw all found plots and return collection of created canvases. Method
        will automatically load plots if these are not loaded yet
        '''
        self.load()

        self.create_experiment_labels()

        canvases = []
        for plot in self.__plots:
            c = ROOT.TCanvas()
            canvases.append(c)

            if plot["2d"]:
                plot["hist"].GetZaxis().SetLabelSize(0)

            plot["hist"].Draw("colz 9" if plot["2d"] else "hist 9")

            map(lambda x: x.Draw('9'), self.experiment_labels.values())

            if plot["label"]:
                plot["label"].Draw('9')

        return canvases

    def __find_plots(self, in_file, path = ""):
        '''
        Recursively scan folders for templates. Method is not for public usage
        '''
        folder = in_file.GetDirectory(
                "{filename}:{path}".format(filename = self.__filename,
                                            path = path))

        # scan through all available objects in current folder
        names = [x.GetName() for x in folder.GetListOfKeys()]
        for name in names:
            h = folder.Get(name)
            if not h:
                print("failed to exract object: " + name, file = sys.stderr)

                continue

            if isinstance(h, ROOT.TH1):
                h = h.Clone()
                h.SetDirectory(0)

                if not h:
                    print("failed to clone object")

                plot = self.process_plot({
                    "path": path,
                    "name": name,
                    "label": None,
                    "2d": True if isinstance(h, ROOT.TH2) else False,
                    "hist": h
                }) 

                if plot:
                    self.__plots.append(plot)

            elif isinstance(h, ROOT.TDirectory):
                if ((not self.use_folders or name in self.use_folders)
                        and name not in self.skip_folders):

                    self.__find_plots(in_file, "{0}/{1}".format(path, name))
            else:
                print("unsupported object: " + name, file = sys.stderr)

    def __str__(self):
        '''
        List loaded plots
        '''

        return "{0:-<80}\n{1}".format(
                "-- loaded {0} plots ".format(len(self.__plots)),
                "\n".join(x["path"] + "/" + x["name"] for x in self.__plots))

    def __nonzero__(self):
        '''
        bool for Python 2.x
        '''
        return self.__bool__()

    def __bool__(self):
        '''
        bool for Python 3.x
        '''
        return bool(self.__plots)

    def process_plot(self, plot):
        return plot

    def create_experiment_labels(self):
        if self.experiment_labels:
            return

        label = ROOT.TLegend(.22, .91, .50, .96)

        label.SetFillColor(10)
        label.SetMargin(0.12);
        label.SetTextSize(0.03);

        label.SetHeader("L = {0:.2f}".format(self.luminosity / 1000) +
                        " fb^{-1}, e+jets")

        self.experiment_labels["luminosity"] = label

        label = ROOT.TLegend(.53, .91, .88, .96)

        label.SetTextAlign(32)
        label.SetTextSize(0.12)

        label.SetHeader("CMS Preliminary #sqrt{s} = 7 TeV")

        label.SetFillColor(10)
        label.SetMargin(0.12);
        label.SetTextSize(0.03);

        self.experiment_labels["experiment"] = label
