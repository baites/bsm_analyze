// Scale cutflow values per sample
//
// Created by Samvel Khalatyan, Dec 14, 2011
// Copyright 2011, All rights reserved

#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>

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

    cout << "# Loaded scales" << endl;

    ostringstream out;
    out << _scales;

    istringstream in(out.str());
    for(char line[512]; in.getline(line, 512);)
    {
        cout << "#   " << line << endl;
    }
    cout << endl;
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

        Input input_type(Input::UNKNOWN);
        if ("wjets" == matches[1])
            input_type = Input::WJETS;
        else if ("zjets" == matches[1])
            input_type = Input::ZJETS;
        else if ("ttjets" == matches[1])
            input_type = Input::TTJETS;
        else if ("stop_s" == matches[1])
            input_type = Input::STOP_S;
        else if ("stop_t" == matches[1])
            input_type = Input::STOP_T;
        else if ("stop_tw" == matches[1])
            input_type = Input::STOP_TW;
        else if ("satop_s" == matches[1])
            input_type = Input::SATOP_S;
        else if ("satop_t" == matches[1])
            input_type = Input::SATOP_T;
        else if ("satop_tw" == matches[1])
            input_type = Input::SATOP_TW;
        else if ("zprime_m1000_w10" == matches[1])
            input_type = Input::ZPRIME1000;
        else if ("zprime_m1500_w15" == matches[1])
            input_type = Input::ZPRIME1500;
        else if ("zprime_m2000_w20" == matches[1])
            input_type = Input::ZPRIME2000;
        else if ("zprime_m3000_w30" == matches[1])
            input_type = Input::ZPRIME3000;
        else if ("qcd_data" ==  matches[1])
            input_type = Input::QCD;
        else
            input_type = Input::RERECO_2011A_MAY10;

        float lumi_scale = getLuminosityScale(input_type);
        float norm_scale = getNormalizationScale(input_type);

        cout << "# " << matches[1] << " lumi_scale: " << lumi_scale
            << " norm_scale: " << norm_scale << endl;

        float scale = lumi_scale * norm_scale;

        istringstream in(matches[2]);

        cout << matches[1] << " ";
        for(float events; in >> events; )
            cout << fixed << events * scale << "+" << sqrt(events) * scale << " ";

        cout << endl;
    }
}

float Cutflow::getLuminosityScale(const Input &input_type)
{
    switch(input_type.type())
    {
        case Input::TTJETS: // fall through
        case Input::ZJETS: // fall through
        case Input::WJETS: // fall through
        case Input::STOP_S: // fall through
        case Input::STOP_T: // fall through
        case Input::STOP_TW: // fall through
        case Input::SATOP_S: // fall through
        case Input::SATOP_T: // fall through
        case Input::SATOP_TW:
        case Input::ZPRIME1000: // fall through
        case Input::ZPRIME1500: // fall through
        case Input::ZPRIME2000: // fall through
        case Input::ZPRIME3000: // fall through
        case Input::ZPRIME4000: break;

        // Skip other channels
        default: return 1;
    }

    return getScale(input_type) * luminosity();
}

float Cutflow::getNormalizationScale(const Input &input_type)
{
    switch(input_type.type())
    {
        case Input::WJETS: return _scales.wjets;
        case Input::ZJETS: return _scales.zjets;
        case Input::TTJETS: return _scales.ttjets;
        case Input::STOP_S: // fall through
        case Input::STOP_T: // fall through
        case Input::STOP_TW: // fall through
        case Input::SATOP_S: // fall through
        case Input::SATOP_T: // fall through
        case Input::SATOP_TW: return _scales.stop;
        case Input::QCD: return _scales.qcd;
        default: return 1;
    }
}
