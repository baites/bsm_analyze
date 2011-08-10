// Apply the Synchronization Exercise Analyzer to the input
//
// Created by Samvel Khalatyan, Aug 01, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include "interface/AppController.h"
#include "interface/ElectronIDAnalyzer.h"

using namespace std;

using boost::shared_ptr;

using bsm::AppController;
using bsm::ElectronIDAnalyzer;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        boost::shared_ptr<ElectronIDAnalyzer> analyzer(new ElectronIDAnalyzer());
        boost::shared_ptr<AppController> app(new AppController());

        app->setAnalyzer(analyzer);
        result = app->run(argc, argv);
        
        analyzer->write("histograms.root");
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
