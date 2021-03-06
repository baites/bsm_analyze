// Apply the Synchronization Exercise Analyzer to the input
//
// Created by Samvel Khalatyan, Aug 01, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include "interface/AppController.h"
#include "interface/Cut2DSelector.h"
#include "interface/EventDump.h"
#include "interface/JetEnergyCorrections.h"
#include "interface/SynchAnalyzer.h"
#include "interface/SynchSelector.h"

using namespace std;

using boost::shared_ptr;

using bsm::AppController;
using bsm::Cut2DSelectorOptions;
using bsm::EventDumpOptions;
using bsm::JetEnergyCorrectionOptions;
using bsm::SynchAnalyzer;
using bsm::SynchAnalyzerOptions;
using bsm::SynchSelectorOptions;
using bsm::TriggerOptions;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        boost::shared_ptr<SynchAnalyzer> analyzer(new SynchAnalyzer());
        boost::shared_ptr<AppController> app(new AppController());
        boost::shared_ptr<JetEnergyCorrectionOptions> jec_options(new JetEnergyCorrectionOptions());
        boost::shared_ptr<SynchSelectorOptions> synch_selector_options(new SynchSelectorOptions());
        boost::shared_ptr<SynchAnalyzerOptions> synch_analyzer_options(new SynchAnalyzerOptions());
        boost::shared_ptr<EventDumpOptions> event_dump_options(new EventDumpOptions());
        boost::shared_ptr<Cut2DSelectorOptions> cut_2d_selector_options(new Cut2DSelectorOptions());
        boost::shared_ptr<TriggerOptions> trigger_options(new TriggerOptions());

        jec_options->setDelegate(analyzer->getJetEnergyCorrectionDelegate());
        synch_selector_options->setDelegate(analyzer->getSynchSelectorDelegate());
        synch_analyzer_options->setDelegate(analyzer.get());
        event_dump_options->setDelegate(analyzer.get());
        cut_2d_selector_options->setDelegate(analyzer->getCut2DSelectorDelegate());
        trigger_options->setDelegate(analyzer.get());

        app->addOptions(*jec_options);
        app->addOptions(*synch_selector_options);
        app->addOptions(*synch_analyzer_options);
        app->addOptions(*event_dump_options);
        app->addOptions(*cut_2d_selector_options);
        app->addOptions(*trigger_options);

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
