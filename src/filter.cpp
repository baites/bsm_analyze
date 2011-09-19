// Apply selectors and filter events
//
// Created by Samvel Khalatyan, May 20, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include "interface/AppController.h"
#include "bsm_input/interface/Event.pb.h"
#include "interface/FilterAnalyzer.h"
#include "interface/JetEnergyCorrections.h"
#include "interface/SynchSelector.h"

using namespace std;

using boost::shared_ptr;

using bsm::AppController;
using bsm::FilterAnalyzer;
using bsm::FilterOptions;
using bsm::JetEnergyCorrectionOptions;
using bsm::SynchSelectorOptions;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        shared_ptr<FilterAnalyzer> analyzer(new FilterAnalyzer());
        shared_ptr<AppController> app(new AppController());

        boost::shared_ptr<JetEnergyCorrectionOptions> jec_options(new JetEnergyCorrectionOptions());
        boost::shared_ptr<SynchSelectorOptions> synch_selector_options(new SynchSelectorOptions());
        boost::shared_ptr<FilterOptions> filter_options(new FilterOptions());

        jec_options->setDelegate(analyzer->getJetEnergyCorrectionDelegate());
        synch_selector_options->setDelegate(analyzer->getSynchSelectorDelegate());
        filter_options->setDelegate(analyzer.get());

        app->addOptions(*jec_options);
        app->addOptions(*synch_selector_options);
        app->addOptions(*filter_options);

        app->setAnalyzer(analyzer, true);

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
