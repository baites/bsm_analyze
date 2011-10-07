// Extract trigger producers from events
//
// Created by Samvel Khalatyan, Oct 07, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include "bsm_input/interface/Event.pb.h"
#include "interface/AppController.h"
#include "interface/TriggerFilterAnalyzer.h"
#include "interface/Thread.h"

using namespace boost;
using namespace bsm;
using namespace std;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        shared_ptr<TriggerFilterAnalyzer>
            analyzer(new TriggerFilterAnalyzer());

        shared_ptr<AppController> app(new AppController());

        app->setAnalyzer(analyzer);

        result = app->run(argc, argv);
    }
    catch(const std::exception &error)
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
