// Filter events
//
// Apply pre-selection and filter events
//
// Created by Samvel Khalatyan, May 23, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include "interface/AppController.h"
#include "bsm_input/interface/Event.pb.h"
#include "interface/FilterAnalyzer.h"

using namespace std;

using boost::shared_ptr;

using bsm::AppController;
using bsm::FilterAnalyzer;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        shared_ptr<FilterAnalyzer> analyzer(new FilterAnalyzer());
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
