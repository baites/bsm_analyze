// Test Jet Energy Corrections Options
//
// Use JEC Options with Application Controller
//
// Created by Samvel Khalatyan, Jul 31, 2011
// Copyright 2011, All rights reserved

#include <boost/shared_ptr.hpp>

#include "interface/AppController.h"
#include "interface/JetEnergyCorrections.h"

using bsm::AppController;
using bsm::JetEnergyCorrectionOptions;

int main(int argc, char *argv[])
{
    boost::shared_ptr<AppController> app(new AppController());

    boost::shared_ptr<JetEnergyCorrectionOptions> jec_options(new JetEnergyCorrectionOptions());
    app->addOptions(*jec_options);

    return app->run(argc, argv);
}
