// Scale cutflow values per sample
//
// Created by Samvel Khalatyan, Dec 14, 2011
// Copyright 2011, All rights reserved

#include <fstream>
#include <iostream>
#include <ostream>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "interface/Cutflow.h"
#include "interface/Input.h"
#include "interface/ROOTUtil.h"

using namespace std;
using namespace boost;

namespace fs = boost::filesystem;

void Cutflow::loadScales(const string &filename)
{
    _scales.load(filename);
}

void Cutflow::load(const string &filename)
{
    if (filename.empty())
        return;

    if (!fs::exists(filename))
    {
        cerr << "cutflow file does not exist: " << filename << endl;

        return;
    }
    
    ifstream in(filename.c_str());
    if (!in.is_open())
    {
        cerr << "failed to open cutflow file: " << filename
            << endl;

        return;
    }

    char buf[1024];
    while(in.getline(buf, 1024))
    {
        string input(buf);

        smatch matches;
        regex pattern("^((?:zprime_[^\\s]+)|(?:w|z|tt)jets|(?:s|sa)top_(?:s|t|tw)|data|qcd_data)((?:\\s+\\d+)+)\\s+$",
                regex_constants::icase | regex_constants::perl);
        if (!regex_match(input, matches, pattern))
        {
            cerr << "didn't understand line: " << input << endl;

            continue;
        }

        bool use_luminosity = true;
        float scale = 1;
        if ("wjets" == matches[1])
        {
            scale = getScale(Input::WJETS) * _scales.wjets;
        }
        else if ("zjets" == matches[1])
        {
            scale = getScale(Input::ZJETS) * _scales.zjets;
        }
        else if ("ttjets" == matches[1])
        {
            scale = getScale(Input::TTJETS) * _scales.ttjets;
        }
        else if ("stop_s" == matches[1])
        {
            scale = getScale(Input::STOP_S) * _scales.stop;
        }
        else if ("stop_t" == matches[1])
        {
            scale = getScale(Input::STOP_T) * _scales.stop;
        }
        else if ("stop_tw" == matches[1])
        {
            scale = getScale(Input::STOP_TW) * _scales.stop;
        }
        else if ("satop_s" == matches[1])
        {
            scale = getScale(Input::SATOP_S) * _scales.stop;
        }
        else if ("satop_t" == matches[1])
        {
            scale = getScale(Input::SATOP_T) * _scales.stop;
        }
        else if ("satop_tw" == matches[1])
        {
            scale = getScale(Input::SATOP_TW) * _scales.stop;
        }
        else if ("zprime_m1000_w10" == matches[1])
        {
            scale = getScale(Input::ZPRIME1000);
        }
        else if ("zprime_m1500_w15" == matches[1])
        {
            scale = getScale(Input::ZPRIME1500);
        }
        else if ("zprime_m2000_w20" == matches[1])
        {
            scale = getScale(Input::ZPRIME2000);
        }
        else if ("zprime_m3000_w30" == matches[1])
        {
            scale = getScale(Input::ZPRIME3000);
        }
        else if ("qcd_data" ==  matches[1])
        {
            scale = _scales.qcd;
            use_luminosity = false;
        }
        else
        {
            use_luminosity = false;
        }

        if (use_luminosity)
            scale *= luminosity();

        istringstream in(matches[2]);

        cout << matches[1] << " ";
        for(float events; in >> events; cout << fixed << events * scale << " ");

        cout << endl;
    }
}
