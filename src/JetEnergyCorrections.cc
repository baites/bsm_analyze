// Jet Energy Corrections Package
//
// Wrapper around CMSSW Jet Energy Corrections with options
//
// Created by Samvel Khalatyan, Jul 31, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <ostream>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

#include "interface/JetEnergyCorrections.h"

using namespace std;

namespace fs = boost::filesystem;

using bsm::JetEnergyCorrectionDelegate;
using bsm::JetEnergyCorrectionOptions;

JetEnergyCorrectionOptions::JetEnergyCorrectionOptions()
{
    _delegate = 0;

    _options.reset(new po::options_description("Jet Energy Correction Options"));
    _options->add_options()
        ("l1",
         po::value<string>()->notifier(
             boost::bind(&JetEnergyCorrectionOptions::setCorrections, this, L1, _1)),
         "Level 1 corrections")

        ("l2",
         po::value<string>()->notifier(
             boost::bind(&JetEnergyCorrectionOptions::setCorrections, this, L2, _1)),
         "Level 2 corrections")

        ("l3",
         po::value<string>()->notifier(
             boost::bind(&JetEnergyCorrectionOptions::setCorrections, this, L3, _1)),
         "Level 3 corrections")
    ;
}

JetEnergyCorrectionOptions::~JetEnergyCorrectionOptions()
{
}

JetEnergyCorrectionOptions::OptionsPtr JetEnergyCorrectionOptions::options() const
{
    return _options;
}

void JetEnergyCorrectionOptions::setCorrections(const JetEnergyCorrectionLevel &jec_level,
        const std::string &file_name)
{
    // Check validity of flag
    //
    switch(jec_level)
    {
        case L1: // Fall through
        case L2: // Fall through
        case L3: 
            {
                if (!fs::exists(file_name))
                    cerr << "Jet Energy Corrections " << jec_level
                        << " file does not exist: " << file_name << endl;
                else
                {
                }
            }
        default: cerr << "unsupported Jet Energy Corrections level" << endl;
    }
}

void JetEnergyCorrectionOptions::setDelegate(JetEnergyCorrectionDelegate *delegate)
{
    if (_delegate != delegate)
        _delegate = delegate;
}

JetEnergyCorrectionDelegate *JetEnergyCorrectionOptions::delegate() const
{
    return _delegate;
}



// Helpers
//
std::ostream &bsm::operator <<(std::ostream &out,
        const JetEnergyCorrectionLevel &jec_level)
{
    switch(jec_level)
    {
        case L1: out << "L1";
                 break;

        case L2: out << "L2";
                 break;

        case L3: out << "L3";
                 break;

        default: out << "unknown";
                 break;
    }

    return out;
}
