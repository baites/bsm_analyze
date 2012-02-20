// Dump resonance information
//
// Created by Samvel Khalatyan, Feb 17, 2012
// Copyright 2012, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TRint.h>

#include "bsm_stat/interface/Utility.h"
#include "interface/Algorithm.h"
#include "interface/AppController.h"
#include "interface/JetEnergyCorrections.h"
#include "interface/MonitorCanvas.h"
#include "interface/Pileup.h"
#include "interface/ResonanceDumpAnalyzer.h"
#include "interface/TriggerAnalyzer.h"
#include "interface/SynchSelector.h"

using namespace std;
using namespace boost;
using namespace bsm;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        shared_ptr<ResonanceDumpAnalyzer> analyzer(new ResonanceDumpAnalyzer());
        shared_ptr<AppController> app(new AppController());

        shared_ptr<JetEnergyCorrectionOptions> jec_options(new JetEnergyCorrectionOptions());
        shared_ptr<SynchSelectorOptions> synch_selector_options(new SynchSelectorOptions());
        shared_ptr<TriggerOptions> trigger_options(new TriggerOptions());
        shared_ptr<ResonanceDumpOptions> dump_options(new ResonanceDumpOptions());
        shared_ptr<EventDumpOptions> event_options(new EventDumpOptions());

        jec_options->setDelegate(analyzer->getJetEnergyCorrectionDelegate());
        synch_selector_options->setDelegate(analyzer->getSynchSelectorDelegate());
        trigger_options->setDelegate(analyzer->getTriggerDelegate());
        dump_options->setDelegate(analyzer.get());
        event_options->setDelegate(analyzer.get());

        app->addOptions(*jec_options);
        app->addOptions(*synch_selector_options);
        app->addOptions(*trigger_options);
        app->addOptions(*dump_options);
        app->addOptions(*event_options);

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
