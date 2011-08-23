// Apply the Synchronization Exercise Analyzer to the input
//
// Created by Samvel Khalatyan, Aug 01, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include "interface/AppController.h"
#include "interface/ElectronIDAnalyzer.h"
#include "interface/JetEnergyCorrections.h"

using namespace std;

using boost::shared_ptr;
using bsm::AppController;
using bsm::ElectronIDAnalyzer;
using bsm::JetEnergyCorrectionOptions;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        // Define all the needed objects
        boost::shared_ptr<JetEnergyCorrectionOptions> jec_options(new JetEnergyCorrectionOptions());
        boost::shared_ptr<ElectronIDAnalyzer> analyzer(new ElectronIDAnalyzer());
        boost::shared_ptr<AppController> app(new AppController());

        // Collect interdependent options
        jec_options->setDelegate(analyzer->getJetEnergyCorrectionDelegate());

        // Set the options into the controller
        app->addOptions(*jec_options);

        // Set the analyzer into the controller
        app->setAnalyzer(analyzer);
        result = app->run(argc, argv);

        // Save the histograms once the job is done
        analyzer->bookkeeper()->write("histograms.root");
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

    return result ? 0 : 1;
}
