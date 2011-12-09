// Draw jet energy scale systematics
//
// Created by Samvel Khalatyan, Nov 18, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_ROOT_JES_SYSTEMATIC
#define BSM_ROOT_JES_SYSTEMATIC

#include <map>
#include <string>

#include "interface/Input.h"
#include "interface/ROOTUtil.h"
#include "interface/Systematic.h"

class TH1;

class JESSystematic: public Systematic
{
    public:
        JESSystematic(const std::string &file_mask):
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
            load(_jes_none, "");
            load(_jes_plus, "_jes_plus", 1);
            load(_jes_minus, "_jes_minus", -1);
        }

        void draw();

    private:
        typedef std::map<Input::Type, TH1 *> Plots;

        void load(Plots &plots,
                const std::string &suffix,
                const int &systematic = 0);

        void plot(const Input::Type &);

        void style(TH1 *, const int &);

        Plots _jes_none;
        Plots _jes_plus;
        Plots _jes_minus;

        std::string _plotname;
};

#endif
