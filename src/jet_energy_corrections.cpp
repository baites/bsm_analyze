// Apply jet energy corrections to good jets
//
// Created by Samvel Khalatyan, Jul 14, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include "interface/AppController.h"
#include "bsm_input/interface/Event.pb.h"
#include "interface/JetEnergyCorrections.h"
#include "interface/JetEnergyCorrectionsAnalyzer.h"

using namespace std;

using boost::shared_ptr;

using bsm::JetEnergyCorrectionsAnalyzer;
using bsm::JetEnergyCorrectionOptions;
using bsm::AppController;

typedef shared_ptr<JetEnergyCorrectionsAnalyzer> AnalyzerPtr;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        boost::shared_ptr<AppController> app(new AppController());
        boost::shared_ptr<JetEnergyCorrectionsAnalyzer> analyzer(new JetEnergyCorrectionsAnalyzer());
        boost::shared_ptr<JetEnergyCorrectionOptions> jec_options(new JetEnergyCorrectionOptions());

        jec_options->setDelegate(analyzer->getJetEnergyCorrectionDelegate());

        app->addOptions(*jec_options);

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
