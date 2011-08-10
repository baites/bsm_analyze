// Read inputs, apply triggers and produce triggers table
//
// Created by Samvel Khalatyan, May 26, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include "bsm_input/interface/Event.pb.h"
#include "interface/AppController.h"
#include "interface/TriggerAnalyzer.h"
#include "interface/Thread.h"

using namespace std;

using boost::shared_ptr;

using bsm::AppController;
using bsm::TriggerAnalyzer;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        shared_ptr<TriggerAnalyzer> analyzer(new TriggerAnalyzer());
        shared_ptr<AppController> app(new AppController());

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
