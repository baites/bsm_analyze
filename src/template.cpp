// Generate template distributions of the hTlep, mTTbar for different 2D cut
// regions
//
// Created by Samvel Khalatyan, Aug 29, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <TCanvas.h>
#include <TGaxis.h>
#include <TH1.h>
#include <TH2.h>
#include <TRint.h>

#include "bsm_stat/interface/Utility.h"
#include "interface/AppController.h"
#include "interface/Cut2DSelector.h"
#include "interface/JetEnergyCorrections.h"
#include "interface/TemplateAnalyzer.h"
#include "interface/SynchSelector.h"

using namespace std;

using boost::shared_ptr;

using bsm::AppController;
using bsm::Cut2DSelectorOptions;
using bsm::JetEnergyCorrectionOptions;
using bsm::TemplateAnalyzer;
using bsm::SynchSelectorOptions;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        boost::shared_ptr<TemplateAnalyzer> analyzer(new TemplateAnalyzer());
        boost::shared_ptr<AppController> app(new AppController());

        boost::shared_ptr<JetEnergyCorrectionOptions> jec_options(new JetEnergyCorrectionOptions());
        boost::shared_ptr<SynchSelectorOptions> synch_selector_options(new SynchSelectorOptions());
        boost::shared_ptr<Cut2DSelectorOptions> cut_2d_selector_options(new Cut2DSelectorOptions());

        jec_options->setDelegate(analyzer->getJetEnergyCorrectionDelegate());
        synch_selector_options->setDelegate(analyzer->getSynchSelectorDelegate());
        cut_2d_selector_options->setDelegate(analyzer->getCut2DSelectorDelegate());

        app->addOptions(*jec_options);
        app->addOptions(*synch_selector_options);
        app->addOptions(*cut_2d_selector_options);

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

            shared_ptr<TCanvas> canvas(new TCanvas());
            canvas->SetTitle("Mass/Htlep");
            canvas->SetWindowSize(1200, 480);
            canvas->Divide(3);

            canvas->cd(1);
            TH1Ptr htlep = convert(*analyzer->htlep());
            htlep->SetName("htlep");
            htlep->GetXaxis()->SetTitle("H_{T}^{lep} [GeV/c]");
            htlep->GetXaxis()->SetTitleSize(0.045);
            htlep->Draw("h");

            canvas->cd(2);
            TH1Ptr mttbar = convert(*analyzer->mttbar());
            mttbar->SetName("mttbar");
            mttbar->GetXaxis()->SetTitle("m_{t#bar{t}}^{reco} [GeV/c^{2}]");
            mttbar->GetXaxis()->SetTitleSize(0.045);
            mttbar->Draw("h");

            canvas->Update();

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
