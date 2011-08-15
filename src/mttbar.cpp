// Mttbar reconstructions
//
// Created by Samvel Khalatyan, Jun 07, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <TCanvas.h>
#include <TH1.h>
#include <TRint.h>

#include "bsm_input/interface/Event.pb.h"
#include "bsm_stat/interface/bsm_stat_fwd.h"
#include "bsm_stat/interface/Utility.h"
#include "interface/AppController.h"
#include "interface/JetEnergyCorrections.h"
#include "interface/MonitorCanvas.h"
#include "interface/MttbarAnalyzer.h"
#include "interface/SynchSelector.h"

using namespace std;

using boost::shared_ptr;

using bsm::AppController;
using bsm::JetEnergyCorrectionOptions;
using bsm::DeltaCanvas;
using bsm::LorentzVectorCanvas;
using bsm::MttbarAnalyzer;
using bsm::SynchSelectorOptions;

typedef shared_ptr<MttbarAnalyzer> AnalyzerPtr;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        AnalyzerPtr analyzer(new MttbarAnalyzer());
        boost::shared_ptr<AppController> app(new AppController());
        boost::shared_ptr<JetEnergyCorrectionOptions> jec_options(new JetEnergyCorrectionOptions());
        boost::shared_ptr<SynchSelectorOptions> synch_selector_options(new SynchSelectorOptions());

        jec_options->setDelegate(analyzer->getJetEnergyCorrectionDelegate());
        synch_selector_options->setDelegate(analyzer->getSynchSelectorDelegate());

        app->addOptions(*jec_options);
        app->addOptions(*synch_selector_options);

        app->setAnalyzer(analyzer);

        result = app->run(argc, argv);
        if (result)
        {
            typedef bsm::stat::TH1Ptr TH1Ptr;

            int empty_argc = 1;
            char *empty_argv[] = { argv[0] };

            boost::shared_ptr<TRint>
                root(new TRint("app", &empty_argc, empty_argv));

            TH1Ptr mttbar = convert(*analyzer->mttbar());
            mttbar->SetName("mttbar");
            mttbar->GetXaxis()->SetTitle("m_{t#bar{t}} [GeV/c^{2}]");
            mttbar->Draw();

            mttbar->SaveAs("mttbar.root");

            boost::shared_ptr<LorentzVectorCanvas> ltop_p4(
                    new LorentzVectorCanvas("Leptonic Top"));
            ltop_p4->draw(*analyzer->ltopMonitor());

            boost::shared_ptr<LorentzVectorCanvas> htop_p4(
                    new LorentzVectorCanvas("Hadronic Top"));
            htop_p4->draw(*analyzer->htopMonitor());

            boost::shared_ptr<DeltaCanvas> top_delta(
                    new DeltaCanvas("Delta between Leptonic and Hadronic tops"));
            top_delta->draw(*analyzer->topDeltaMonitor());

            root->Run();
        }
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
