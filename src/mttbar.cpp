// Mttbar reconstructions
//
// Created by Samvel Khalatyan, Jun 07, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <TCanvas.h>
#include <TGaxis.h>
#include <TH1.h>
#include <TH2.h>
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
using bsm::MttbarOptions;
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
        boost::shared_ptr<MttbarOptions> mttbar_options(new MttbarOptions());

        jec_options->setDelegate(analyzer->getJetEnergyCorrectionDelegate());
        synch_selector_options->setDelegate(analyzer->getSynchSelectorDelegate());
        mttbar_options->setDelegate(analyzer.get());

        app->addOptions(*jec_options);
        app->addOptions(*synch_selector_options);
        app->addOptions(*mttbar_options);

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
            canvas->SetTitle("Mass");
            canvas->SetWindowSize(1200, 800);
            canvas->Divide(3, 2);

            canvas->cd(1);
            TH1Ptr mreco = convert(*analyzer->mreco());
            mreco->SetName("mreco");
            mreco->GetXaxis()->SetTitle("m_{t#bar{t}}^{reco} [GeV/c^{2}]");
            mreco->GetXaxis()->SetTitleSize(0.045);
            mreco->Draw();

            mreco->SaveAs("mreco.root");

            canvas->cd(2);
            TH1Ptr mgen = convert(*analyzer->mgen());
            mgen->SetName("mgen");
            mgen->GetXaxis()->SetTitle("m_{t#bar{t}}^{gen} [GeV/c^{2}]");
            mgen->GetXaxis()->SetTitleSize(0.045);
            mgen->Draw();

            canvas->cd(3);
            TH1Ptr mreco_minus_mgen = convert(*analyzer->mrecoMinusMgen());
            mreco_minus_mgen->SetName("mreco_minus_mgen");
            mreco_minus_mgen->GetXaxis()->SetTitle("(m_{t#bar{t}}^{reco} - m_{t#bar{t}}^{gen}) / m_{t#bar{t}}^{gen}");
            mreco_minus_mgen->GetXaxis()->SetTitleSize(0.045);
            mreco_minus_mgen->Draw();

            canvas->cd(4);
            canvas->cd(4)->SetLeftMargin(10);
            TH2Ptr mltop_vs_mhtop = convert(*analyzer->mltopVsMhtop());
            mltop_vs_mhtop->SetName("mltop_vs_mhtop");
            mltop_vs_mhtop->GetXaxis()->SetTitle("m_{t,lepton} [GeV/c^{2}]");
            mltop_vs_mhtop->GetXaxis()->SetTitleSize(0.045);
            mltop_vs_mhtop->GetYaxis()->SetTitle("m_{t,hadron} [GeV/c^{2}]");
            mltop_vs_mhtop->GetYaxis()->SetTitleSize(0.06);
            mltop_vs_mhtop->Draw("colz");

            canvas->cd(5);
            canvas->cd(5)->SetLeftMargin(10);
            TH2Ptr mreco_vs_mgen = convert(*analyzer->mrecoVsMgen());
            mreco_vs_mgen->SetName("mreco_vs_mgen");
            mreco_vs_mgen->GetXaxis()->SetTitle("m_{t#bar{t}}^{reco} [GeV/c^{2}]");
            mreco_vs_mgen->GetXaxis()->SetTitleSize(0.045);
            mreco_vs_mgen->GetYaxis()->SetTitle("m_{t#bar{t}}^{gen} [GeV/c^{2}]");
            mreco_vs_mgen->GetYaxis()->SetTitleSize(0.045);
            mreco_vs_mgen->Draw("colz");

            canvas->Update();

            mltop_vs_mhtop->SaveAs("mltop_vs_mhtop.root");

            boost::shared_ptr<LorentzVectorCanvas> met(
                    new LorentzVectorCanvas("Reconstructed Missing Energy"));
            met->draw(*analyzer->missingEnergyMonitor());

            boost::shared_ptr<LorentzVectorCanvas> lwboson(
                    new LorentzVectorCanvas("Leptonic W-Boson"));
            lwboson->draw(*analyzer->lwbosonMonitor());

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
