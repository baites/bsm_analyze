// Generate template distributions of the hTlep, mTTbar for different 2D cut
// regions
//
// Created by Samvel Khalatyan, Aug 29, 2011
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
#include "interface/Cut2DSelector.h"
#include "interface/JetEnergyCorrections.h"
#include "interface/TemplateAnalyzer.h"
#include "interface/TriggerAnalyzer.h"
#include "interface/SynchSelector.h"

using namespace std;

using boost::shared_ptr;

using bsm::AppController;
using bsm::Cut2DSelectorOptions;
using bsm::JetEnergyCorrectionOptions;
using bsm::TemplateAnalyzer;
using bsm::TriggerOptions;
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
        boost::shared_ptr<TriggerOptions> trigger_options(new TriggerOptions());

        jec_options->setDelegate(analyzer->getJetEnergyCorrectionDelegate());
        synch_selector_options->setDelegate(analyzer->getSynchSelectorDelegate());
        cut_2d_selector_options->setDelegate(analyzer->getCut2DSelectorDelegate());
        trigger_options->setDelegate(analyzer.get());

        app->addOptions(*jec_options);
        app->addOptions(*synch_selector_options);
        app->addOptions(*cut_2d_selector_options);
        app->addOptions(*trigger_options);

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

            TH1Ptr d0 = convert(*analyzer->d0());
            d0->SetName("d0");
            d0->GetXaxis()->SetTitle("i.p. [cm]");
            d0->GetXaxis()->SetTitleSize(0.045);
            d0->SetMarkerSize(0.1);

            TH1Ptr htlep = convert(*analyzer->htlep());
            htlep->SetName("htlep");
            htlep->GetXaxis()->SetTitle("H_{T}^{lep} [GeV/c]");
            htlep->GetXaxis()->SetTitleSize(0.045);

            TH1Ptr mttbar_before_htlep = convert(*analyzer->mttbarBeforeHtlep());
            mttbar_before_htlep->SetName("mttbar_before_htlep");
            mttbar_before_htlep->GetXaxis()->SetTitle("m_{t#bar{t}}^{reco} [GeV/c^{2}]");
            mttbar_before_htlep->GetXaxis()->SetTitleSize(0.045);

            TH1Ptr mttbar_after_htlep = convert(*analyzer->mttbarAfterHtlep());
            mttbar_after_htlep->SetName("mttbar_after_htlep");
            mttbar_after_htlep->GetXaxis()->SetTitle("m_{t#bar{t}}^{reco} [GeV/c^{2}]");
            mttbar_after_htlep->GetXaxis()->SetTitleSize(0.045);

            TH2Ptr dr_vs_ptrel = convert(*analyzer->drVsPtrel());
            dr_vs_ptrel->SetName("dr_vs_ptrel");
            dr_vs_ptrel->GetXaxis()->SetTitle("p_{T}^{rel} [GeV/c^{2}]");
            dr_vs_ptrel->GetXaxis()->SetTitleSize(0.045);
            dr_vs_ptrel->GetYaxis()->SetTitle("#Delta R");
            dr_vs_ptrel->GetYaxis()->SetTitleSize(0.045);

            if (app->output())
            {
                d0->Write();
                htlep->Write();
                mttbar_before_htlep->Write();
                mttbar_after_htlep->Write();
                dr_vs_ptrel->Write();
            }

            if (app->isInteractive())
            {
                shared_ptr<TCanvas> canvas(new TCanvas());
                canvas->SetTitle("Mass/Htlep");
                canvas->SetWindowSize(800, 600);
                canvas->Divide(2, 2);

                canvas->cd(1);
                dr_vs_ptrel->Draw("colz");

                canvas->cd(2);
                htlep->Draw("h");

                canvas->cd(3);
                mttbar_before_htlep->Draw("h");

                canvas->cd(4);
                mttbar_after_htlep->Draw("h");

                canvas->Update();

                shared_ptr<TCanvas> canvas2(new TCanvas());
                canvas2->SetTitle("Other");
                canvas2->SetWindowSize(800, 600);
                canvas2->Divide(2, 2);

                canvas2->cd(1);
                d0->Draw("h");

                root->Run();
            }
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
