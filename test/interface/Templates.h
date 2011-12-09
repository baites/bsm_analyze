// Plot templates
//
// Created by Samvel Khalatyan, Dec 1, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_TEMPLATES
#define BSM_TEMPLATES

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "interface/Input.h"
#include "interface/Channel.h"
#include "interface/ROOTUtil.h"
#include "interface/Template.h"

class TCanvas;
class TFile;
class TLegend;
class TObject;

class Templates
{
    public:
        Templates(const std::string &input_file);
        ~Templates();

        std::string input_file() const;

        void load();
        void draw();

    private:
        // Histograms groupped in channels
        //
        typedef std::map<Channel, TH1 *> Channels;

        // Inputs organized in:
        //
        //      [type][input]
        //
        typedef std::map<Input, TH1 *> InputPlots;
        typedef std::map<Template, InputPlots> Plots;

        // Track all created objects on heap
        //
        typedef std::vector<TObject *> Heap;

        typedef std::pair<int, int> Rebin;

        // Load histograms from given file
        //
        void loadHistograms(TFile *, const Input &);

        void plot(const Template &type);
        void plot2D(const Template &type);

        TH1 *get(const InputPlots &plots,
                const Input::Type &from,
                const Input::Type &to = Input::UNKNOWN);

        TLegend *createLegend(const std::string & = "");

        void cmsLegend();
        void histTestLegend(const float &kolmogorov_smirnov,
                const float &chi2);

        int rebin(const Template &) const;
        Rebin rebin2D(const Template &) const;

        TCanvas *draw(const Template &, Channels &);
        TCanvas *draw2D(const Template &, Channels &);
        void style(TH1 *, const Input &);

        void setYaxisTitle(TH1 *h, const Template &plot);

        Heap _heap;
        Plots _plots;

        const std::string _input_file;
};

#endif
