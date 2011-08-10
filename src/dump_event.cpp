// Dump Event content given the run/lumi and event numbers
//
// Created by Samvel Khalatyan, Jul 07, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include "bsm_input/interface/Event.pb.h"
#include "interface/AppController.h"
#include "interface/EventDump.h"

using namespace std;

using boost::shared_ptr;

using bsm::AppController;
using bsm::EventDumpAnalyzer;
using bsm::EventDumpOptions;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        shared_ptr<EventDumpAnalyzer> analyzer(new EventDumpAnalyzer());
        shared_ptr<AppController> app(new AppController());
        shared_ptr<EventDumpOptions> event_options(new EventDumpOptions());

        event_options->setDelegate(analyzer.get());

        app->addOptions(*event_options);

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
