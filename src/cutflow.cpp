// Read inputs and generate cutflow tables
//
// Created by Samvel Khalatyan, May 18, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include "bsm_input/interface/Event.pb.h"
#include "interface/AppController.h"
#include "interface/CutflowAnalyzer.h"

using namespace std;

using boost::shared_ptr;

using bsm::AppController;
using bsm::CutflowAnalyzer;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    int result = 0;
    try
    {
        shared_ptr<CutflowAnalyzer> analyzer(new CutflowAnalyzer());
        boost::shared_ptr<AppController> app(new AppController());

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

        result = 1;
    }

    // Clean Up any memory allocated by libprotobuf
    //
    google::protobuf::ShutdownProtobufLibrary();

    return result;
}
