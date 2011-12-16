// Read input file generated by cutflow.sh and apply scales
//
// Created by Samvel Khalatyan, Dec 14, 2011
// Copyright 2011, All rights reserved

#include <iostream>

#include <boost/lexical_cast.hpp>

#include "interface/Cutflow.h"

using namespace boost;
using namespace std;

int main(int argc, char *argv[])
{
    if (3 > argc)
    {
        cerr << "Usage: " << argv[0] << " cutflow.txt qcd_scale" << endl;

        return 0;
    }

    Cutflow cutflow;
    cutflow.load(argv[1], lexical_cast<float>(argv[2]));

    return 0;
}
