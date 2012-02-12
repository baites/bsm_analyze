from __future__ import print_function

import ROOT

import root.template

class HadronicTopTemplates(root.template.Templates):
    '''
    Add appropriate binning to Hadronic top tempalates
    '''

    luminosity = 4328.472

    __top_mass_rebin = 10
    __top_pt_rebin = 10

    __labels = {
            "njets": "N_{jets}^{LABEL}",
            "mass": "M^{LABEL}",
            "mt": "M_{T}^{LABEL}",
            "energy": "E^{LABEL}",
            "et": "E_{T}^{LABEL}",
            "px": "p_{x}^{LABEL}",
            "py": "p_{y}^{LABEL}",
            "pz": "p_{z}^{LABEL}",
            "pt": "p_{T}^{LABEL}",
            "eta": "#eta^{LABEL}",
            "phi": "#phi^{LABEL}"
            }

    __units = {
            "mass": "[GeV/c^{2}]",
            "mt": "[GeV/c^{2}]",
            "energy": "[GeV/c^{2}]",
            "et": "[GeV/c^{2}]",
            "px": "[GeV/c]",
            "py": "[GeV/c]",
            "pz": "[GeV/c]",
            "pt": "[GeV/c]",
            "phi": "[rad]"
            }

    jets_rebin = {
            "energy": 5,
            "px": 5,
            "py": 5,
            "pz": 5,
            "pt": 25,
            "eta": 50,
            "phi": 10,
            "mass": 10,
            "mt": 10,
            "et": 5
            }

    top_rebin = {
            "energy": 5,
            "px": 5,
            "py": 5,
            "pz": 5,
            "pt": 25,
            "eta": 50,
            "phi": 10,
            "mass": 10,
            "mt": 10,
            "et": 5
            }

    def __init__(self, filename):
        root.template.Templates.__init__(self, filename)

    def process_plot(self, plot):
        '''
        Add appropriate binning to Hadronic top tempalates
        '''
        return {
            "": self.__process_top_level_plot,
            "/top": self.__process_top_folder,
            "/jet1": self.__process_jet_folder,
            "/jet2": self.__process_jet_folder,
            "/jet3": self.__process_jet_folder,
            "/jet4": self.__process_jet_folder
        }.get(plot["path"], lambda plot: None)(plot)

    def __process_top_level_plot(self, plot):
        if "njets" == plot["name"]:
            h = plot["hist"]
            integral = h.Integral()

            print("Hadronic top Njets compisition")
            for k, v in dict((x, h.GetBinContent(h.FindBin(x)) / integral)
                    for x in range(1, 10)).items():
                print("{0:>2} {1:.2f}%".format(k, v))
            print()

        name = plot["name"]
        plot_2d = plot["2d"]

        for k, v in self.__labels.items():
            if name.startswith(k):
                units = self.__units.get(k, "")
                label = v.replace("LABEL", "top")
                if units:
                    label += " " + units

                plot["hist"].GetXaxis().SetTitle(label)

                if not plot_2d:
                    h.GetYaxis().SetTitle("event yield / {0} {1}".format(plot["hist"].GetBinWidth(1), units))

            if plot_2d and name.endswith(k):
                units = self.__units.get(k, "")
                label = v.replace("LABEL", "top")
                if units:
                    label += " " + units

                plot["hist"].GetYaxis().SetTitle(label)

        rebin_x = 1
        rebin_y = 1

        if name.startswith("mass"):
            rebin_x = self.__top_mass_rebin
        elif name.startswith("pt"):
            rebin_x = self.__top_pt_rebin

        if plot_2d:
            if name.endswith("mass"):
                rebin_y = self.__top_mass_rebin
            elif name.endswith("pt"):
                rebin_y = self.__top_pt_rebin

        return self.__rebin(plot, rebin_x, rebin_y)

    def __process_top_folder(self, plot):
        if plot["name"] in ("px", "py", "pz", "phi"):
            return None

        plot["hist"].GetXaxis().SetTitle(
                self.__labels.get(
                    plot["name"], "").replace("LABEL", "top"))

        return self.__rebin(plot, self.top_rebin.get(plot["name"], 1))

    def __process_jet_folder(self, plot):
        # skip px, py, pz
        if plot["name"] in ("px", "py", "pz", "phi"):
            return None
        
        plot["hist"].GetXaxis().SetTitle(
                self.__labels.get(plot["name"], "").replace(
                    "LABEL", plot["path"].lstrip('/').replace('/', ' ')))

        return self.__rebin(plot, self.jets_rebin.get(plot["name"], 1))

    def __rebin(self, plot, bins_x, bins_y = 1):
        if 1 < bins_x:
            (plot["hist"].RebinX if plot["2d"] else plot["hist"].Rebin)(bins_x)

        if 1 < bins_y:
            plot["hist"].RebinY(bins_y)

        return plot

    def create_experiment_labels(self):
        if self.experiment_labels:
            return

        label = ROOT.TLegend(.53, .91, .88, .96)

        label.SetTextAlign(32)
        label.SetTextSize(0.12)

        label.SetHeader("CMS Simulation Preliminary #sqrt{s} = 7 TeV")

        label.SetFillColor(10)
        label.SetMargin(0.12);
        label.SetTextSize(0.03);

        self.experiment_labels["experiment"] = label
