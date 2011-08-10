// Apply lepton removal to jets, Jet Energy Corrections, select events
// based on the corrected P4, leptons and then aaply 2D cut:
//      DeltaR > CUT
//      pT,rel > CUT
//
// between the muon/electron and closest jet
//
// Created by Samvel Khalatyan, Jul 19, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include "interface/AppController.h"
#include "interface/EfficiencyAnalyzer.h"
#include "interface/JetEnergyCorrections.h"
#include "interface/SynchSelector.h"

using namespace std;

using boost::shared_ptr;

using bsm::AppController;
using bsm::EfficiencyAnalyzer;
using bsm::JetEnergyCorrectionOptions;
using bsm::SynchSelectorOptions;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        boost::shared_ptr<EfficiencyAnalyzer> analyzer(new EfficiencyAnalyzer());
        boost::shared_ptr<AppController> app(new AppController());
        boost::shared_ptr<JetEnergyCorrectionOptions> jec_options(new JetEnergyCorrectionOptions());
        boost::shared_ptr<SynchSelectorOptions> synch_selector_options(new SynchSelectorOptions());

        jec_options->setDelegate(analyzer->getJetEnergyCorrectionDelegate());
        synch_selector_options->setDelegate(analyzer->getSynchSelectorDelegate());

        app->addOptions(*jec_options);
        app->addOptions(*synch_selector_options);

        app->setAnalyzer(analyzer);

        result = app->run(argc, argv);
    }
    catch(const exception &error)
    {
        cerr << error.what() << endl;

        result = false;
    }
    catch(...)
    {
        cerr << "Unknown error" << endl;

        result = false;
    }

    // Clean Up any memory allocated by libprotobuf
    //
    google::protobuf::ShutdownProtobufLibrary();

    return result
        ? 0
        : 1;
}
