// Apply SynchSelector, use events with matched reconstructed jets with
// gen partons.
//
// Created by Samvel Khalatyan, Mar 29, 2012
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <TCanvas.h>
#include <TFile.h>
#include <TGaxis.h>
#include <TH1.h>
#include <TH2.h>
#include <TRint.h>

#include "bsm_stat/interface/Utility.h"
#include "interface/AppController.h"
#include "interface/Btag.h"
#include "interface/Cut2DSelector.h"
#include "interface/JetEnergyCorrections.h"
#include "interface/MonitorCanvas.h"
#include "interface/Pileup.h"
#include "interface/GenMatchingAnalyzer.h"
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
        boost::shared_ptr<GenMatchingAnalyzer> analyzer(new GenMatchingAnalyzer());
        boost::shared_ptr<AppController> app(new AppController());

        boost::shared_ptr<JetEnergyCorrectionOptions> jec_options(new JetEnergyCorrectionOptions());
        boost::shared_ptr<SynchSelectorOptions> synch_selector_options(new SynchSelectorOptions());
        boost::shared_ptr<TriggerOptions> trigger_options(new TriggerOptions());
        boost::shared_ptr<BtagOptions> btag_options(new BtagOptions());

        jec_options->setDelegate(analyzer->getJetEnergyCorrectionDelegate());
        synch_selector_options->setDelegate(analyzer->getSynchSelectorDelegate());
        trigger_options->setDelegate(analyzer->getTriggerDelegate());
        btag_options->setDelegate(analyzer->getBtagDelegate());

        app->addOptions(*jec_options);
        app->addOptions(*synch_selector_options);
        app->addOptions(*trigger_options);
        app->addOptions(*btag_options);

        app->setAnalyzer(analyzer);

        result = app->run(argc, argv);
        if (result)
        {
            typedef bsm::stat::TH1Ptr TH1Ptr;
            typedef bsm::stat::TH2Ptr TH2Ptr;

            int empty_argc = 1;
            char *empty_argv[] = { argv[0] };

            boost::shared_ptr<TRint>
                root(new TRint("app", &empty_argc, empty_argv));

            TGaxis::SetMaxDigits(3);

            TH1Ptr cutflow = convert(*analyzer->cutflow());
            cutflow->SetName("cutflow");

            TH1Ptr ltop_drsum = convert(*analyzer->ltop_drsum());
            ltop_drsum->SetName("ltop_drsum");

            TH1Ptr htop_drsum = convert(*analyzer->htop_drsum());
            htop_drsum->SetName("htop_drsum");

            TH1Ptr htop_dphi = convert(*analyzer->htop_dphi());
            htop_dphi->SetName("htop_dphi");

            shared_ptr<P4Canvas> ltop(new P4Canvas("ltop", "ltop"));
            shared_ptr<P4Canvas> htop(new P4Canvas("htop", "htop"));

            shared_ptr<P4Canvas> ttbar(new P4Canvas("ttbar", "ttbar"));

            if (app->output())
            {
                cutflow->Write();

                ltop_drsum->Write();
                htop_drsum->Write();
                htop_dphi->Write();

                ltop->write(*analyzer->ltop(), app->output().get());
                ltop->write(*analyzer->ltop(), app->output().get());

                ttbar->write(*analyzer->ttbar(), app->output().get());
            }
        }
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
