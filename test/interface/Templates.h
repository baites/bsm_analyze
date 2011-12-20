// Plot templates
//
// Created by Samvel Khalatyan, Dec 1, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_TEMPLATES
#define BSM_TEMPLATES

#include <iosfwd>
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
        struct ThetaScale
        {
            ThetaScale():
                wjets(0),
                zjets(0),
                stop(0),
                ttjets(0),
                qcd(0)
            {
            }

            float wjets;
            float zjets;
            float stop;
            float ttjets;
            float qcd;
        };

        Templates(const std::string &input_file, const std::string &theta_scale);
        ~Templates();

        std::string input_file() const;

        void load();
        void draw();

        enum QCDType {QCD_NONE = 0, QCD_FROM_MC, QCD_FROM_DATA};

        void setQCDType(QCDType value)
        {
            _qcd_type = value;
        }

        void setMCError(bool value = true)
        {
            _mc_error = value;
        }

        void setPullPlots(float value = 1.0)
        {
            _pull_plots = value;
        }

        void setKSChi2(bool value = true)
        {
            _ks_chi2 = value;
        }

        void setLogScale(const bool &value)
        {
            _log_scale = value;
        }

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

        TLegend *createLegend(const std::string & = "", bool = false);

        void cmsLegend();
        void histTestLegend(const float &kolmogorov_smirnov,
                const float &chi2);

        int rebin(const Template &) const;
        Rebin rebin2D(const Template &) const;

        void rebin(TH1 *, const Template &) const;
        void rebin2D(TH1 *, const Template &) const;

        TCanvas *draw(const Template &, Channels &);
        TCanvas *draw2D(const Template &, Channels &);
        void style(TH1 *, const Input &);

        void setYaxisTitle(TH1 *h, const Template &plot);

        Heap _heap;
        Plots _plots;

        const std::string _input_file;

        ThetaScale _theta_scale;

        // QCD and MC normalization from data
        //
        double _mc_fraction;
        double _mc_fraction_error;
        bool _mc_error;

        double _qcd_fraction;
        double _qcd_fraction_error;

        void normalize(); 
        TCanvas *normalize(const Template &, Channels &, Channels &);

        QCDType _qcd_type;
        float _pull_plots;
        bool _ks_chi2;

        bool _log_scale;
};

std::ostream &operator <<(std::ostream &, const Templates::ThetaScale &);

#endif
