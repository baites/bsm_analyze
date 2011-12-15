// Scales for each sample from Theta
//
// Created by Samvel Khalatyan, Dec 14, 2011
// Copyright 2011, All rights reserved

#include <fstream>
#include <iostream>
#include <ostream>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "interface/ThetaScale.h"

using namespace std;
using namespace boost;

namespace fs = boost::filesystem;

void ThetaScale::load(const string &filename)
{
    if (filename.empty())
        return;

    if (!fs::exists(filename))
    {
        cerr << "theta scale file does not exist: " << filename << endl;

        return;
    }
    
    ifstream in(filename.c_str());
    if (!in.is_open())
    {
        cerr << "failed to open theta scale file: " << filename
            << endl;

        return;
    }

    char buf[512];
    while(in.getline(buf, 512))
    {
        string input(buf);

        smatch matches;
        regex pattern("^(wjets|zjets|singletop|ttbar|eleqcd):\\s+(\\d+\\.\\d+)$",
                regex_constants::icase | regex_constants::perl);
        if (!regex_match(input, matches, pattern))
        {
            cerr << "didn't understand line: " << buf << endl;

            continue;
        }

        float scale = lexical_cast<float>(matches[2]);
        if ("wjets" == matches[1])
        {
            wjets = scale;
        }
        else if ("zjets" == matches[1])
        {
            zjets = scale;
        }
        else if ("singletop" == matches[1])
        {
            stop = scale;
        }
        else if ("ttbar" == matches[1])
        {
            ttjets = scale;
        }
        else if ("eleqcd" == matches[1])
        {
            qcd = scale;
        }
    }
}

ostream &operator <<(ostream &out, const ThetaScale &scale)
{
    return out << "wjets: " << scale.wjets << endl
        << "zjets: " << scale.zjets << endl
        << "stop: " << scale.stop << endl
        << "ttjets: " << scale.ttjets << endl
        << "qcd: " << scale.qcd;
}
