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

void Cutflow::load(const string &filename, const float &qcd_scale)
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

    char buf[512];
    while(in.getline(buf, 512))
    {
        string input(buf);

        smatch matches;
        regex pattern("^((?:zprime_[^\\s]+)|(?:w|z|tt)jets|(?:s|sa)top_(?:s|t|tw)|data|qcd_data)((?:\\s+\\d+)+)$",
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
            scale = getScale(Input::WJETS);
        }
        else if ("zjets" == matches[1])
        {
            scale = getScale(Input::ZJETS);
        }
        else if ("ttjets" == matches[1])
        {
            scale = getScale(Input::TTJETS);
        }
        else if ("stop_s" == matches[1])
        {
            scale = getScale(Input::STOP_S);
        }
        else if ("stop_t" == matches[1])
        {
            scale = getScale(Input::STOP_T);
        }
        else if ("stop_tw" == matches[1])
        {
            scale = getScale(Input::STOP_TW);
        }
        else if ("satop_s" == matches[1])
        {
            scale = getScale(Input::SATOP_S);
        }
        else if ("satop_t" == matches[1])
        {
            scale = getScale(Input::SATOP_T);
        }
        else if ("satop_tw" == matches[1])
        {
            scale = getScale(Input::SATOP_TW);
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
            scale = qcd_scale;
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
        int precision = cout.precision(2);
        for(float events; in >> events; cout << fixed << events * scale << " ");
        cout.precision(precision);

        cout << endl;
    }
}
