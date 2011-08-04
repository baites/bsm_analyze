// Read events and plot closest jet kinematics
//
// Created by Samvel Khalatyan, Apr 25, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TRint.h>

#include "bsm_stat/interface/Utility.h"
#include "bsm_stat/interface/Axis.h"
#include "bsm_stat/interface/H1.h"
#include "bsm_stat/interface/H2.h"

#include "bsm_input/interface/Event.pb.h"
#include "interface/AppController.h"
#include "interface/ClosestJetAnalyzer.h"
#include "interface/Monitor.h"
#include "interface/MonitorCanvas.h"

using namespace std;

using boost::shared_ptr;

using bsm::AppController;
using bsm::ClosestJetAnalyzer;
using bsm::DeltaCanvas;
using bsm::stat::convert;
using bsm::stat::TH1Ptr;
using bsm::stat::TH2Ptr;
using bsm::LorentzVectorCanvas;

typedef boost::shared_ptr<ClosestJetAnalyzer> ClosestJetAnalyzerPtr;

void plot(const ClosestJetAnalyzerPtr &);

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        ClosestJetAnalyzerPtr analyzer(new ClosestJetAnalyzer());
        analyzer->monitorElectronDelta()->ptrel()->mutable_axis()->init(200, 0, 200);
        analyzer->monitorElectronDelta()->ptrel_vs_r()->mutable_xAxis()->init(200, 0, 200);
        analyzer->monitorMuonDelta()->ptrel()->mutable_axis()->init(200, 0, 200);
        analyzer->monitorMuonDelta()->ptrel_vs_r()->mutable_xAxis()->init(200, 0, 200);

        boost::shared_ptr<AppController> app(new AppController());

        app->setAnalyzer(analyzer);
        result = app->run(argc, argv);
        if (result)
            plot(analyzer);
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

void plot(const ClosestJetAnalyzerPtr &analyzer)
{
    typedef LorentzVectorCanvas P4Canvas;
    typedef shared_ptr<P4Canvas> P4CanvasPtr;

    // Cheat ROOT with empty args
    //
    int empty_argc = 1;
    char *empty_argv[] = { "root" };
    shared_ptr<TRint> app(new TRint("app", &empty_argc, empty_argv));

    P4CanvasPtr el_canvas(new P4Canvas("Electrons"));
    el_canvas->draw(*analyzer->monitorElectrons());

    P4CanvasPtr el_closest_jet_canvas(new P4Canvas("Electron Closest Jet"));
    el_closest_jet_canvas->draw(*analyzer->monitorElectronJets());

    shared_ptr<DeltaCanvas> el_delta_canvas(new DeltaCanvas("Electron Closest Jet Delta"));
    el_delta_canvas->draw(*analyzer->monitorElectronDelta());

    P4CanvasPtr mu_canvas(new P4Canvas("Muons"));
    mu_canvas->draw(*analyzer->monitorMuons());

    P4CanvasPtr mu_closest_jet_canvas(new P4Canvas("Muon Closest Jet"));
    mu_closest_jet_canvas->draw(*analyzer->monitorMuonJets());

    shared_ptr<DeltaCanvas>
        mu_delta_canvas(new DeltaCanvas("Muon Closest Jet Delta"));
    mu_delta_canvas->draw(*analyzer->monitorMuonDelta());

    app->Run();
}
