// Load and plot Scaling systematic
//
// Created by Samvel Khalatyan, Nov 18, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_ROOT_SCALING_SYSTEMATIC
#define BSM_ROOT_SCALING_SYSTEMATIC

#include <map>
#include <string>

#include "interface/Input.h"
#include "interface/ROOTUtil.h"
#include "interface/Systematic.h"

class TH1;

class ScalingSystematic: public Systematic
{
    public:
        ScalingSystematic(const std::string &file_mask):
            Systematic(file_mask),
            _plotname("mttbar_after_htlep")
        {
        }

        std::string plotname() const
        {
            return _plotname;
        }

        void load()
        {
            load(_systematic_none, "");
            load(_systematic_plus, "_scaling_plus", 1);
            load(_systematic_minus, "_scaling_minus", -1);
        }

        void draw();

    private:
        typedef std::map<Input::Type, TH1 *> Plots;

        void load(Plots &plots,
                const std::string &suffix,
                const int &systematic = 0);
        TH1 *loadPlot(const Input &input, const std::string &suffix, Plots &plots);

        void plot(const Input::Type &);

        void style(TH1 *, const int &);

        Plots _systematic_none;
        Plots _systematic_plus;
        Plots _systematic_minus;

        std::string _plotname;
};

#endif
