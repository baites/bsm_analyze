// Monitor BSM Input
//
// Read events and plot basic distributions
//
// Created by Samvel Khalatyan, Apr 22, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <TCanvas.h>
#include <TH1.h>
#include <TRint.h>

#include "bsm_stat/interface/Utility.h"
#include "interface/AppController.h"
#include "interface/Monitor.h"
#include "interface/MonitorCanvas.h"
#include "interface/MonitorAnalyzer.h"

using namespace std;

using boost::shared_ptr;

using bsm::AppController;
using bsm::MonitorAnalyzer;

using namespace bsm;

typedef shared_ptr<MonitorAnalyzer> MonitorAnalyzerPtr;

void plot(const MonitorAnalyzerPtr &, char *[]);

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        MonitorAnalyzerPtr analyzer(new MonitorAnalyzer());
        boost::shared_ptr<AppController> app(new AppController());

        app->setAnalyzer(analyzer);

        result = app->run(argc, argv);

        if (result)
            plot(analyzer, argv);
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

void plot(const MonitorAnalyzerPtr &analyzer, char *argv[])
{
    // Cheat ROOT with empty args
    //
    int empty_argc = 1;
    char *empty_argv[] = { argv[0] };
    shared_ptr<TRint> app(new TRint("app", &empty_argc, empty_argv));

    shared_ptr<JetCanvas> jet_canvas(new JetCanvas("Jets"));
    jet_canvas->draw(*analyzer->jets());

    shared_ptr<MuonCanvas> mu_pf_canvas(new MuonCanvas("Particle Flow Muons"));
    mu_pf_canvas->draw(*analyzer->pfMuons());

    shared_ptr<ElectronCanvas> el_pf_canvas(new ElectronCanvas("Particle Flow Electrons"));
    el_pf_canvas->draw(*analyzer->pfElectrons());

    shared_ptr<PrimaryVertexCanvas> pv_canvas(new PrimaryVertexCanvas("Primary Vertex"));
    pv_canvas->draw(*analyzer->primaryVertices());

    shared_ptr<MissingEnergyCanvas> met_canvas(new MissingEnergyCanvas("Missing Energy"));
    met_canvas->draw(*analyzer->missingEnergy());

    app->Run();
}
