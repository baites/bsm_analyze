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
#include "interface/MonitorCanvas.h"
#include "interface/Pileup.h"
#include "interface/TemplateAnalyzer.h"
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
        boost::shared_ptr<TemplateAnalyzer> analyzer(new TemplateAnalyzer());
        boost::shared_ptr<AppController> app(new AppController());

        boost::shared_ptr<JetEnergyCorrectionOptions> jec_options(new JetEnergyCorrectionOptions());
        boost::shared_ptr<SynchSelectorOptions> synch_selector_options(new SynchSelectorOptions());
        boost::shared_ptr<Cut2DSelectorOptions> cut_2d_selector_options(new Cut2DSelectorOptions());
        boost::shared_ptr<TriggerOptions> trigger_options(new TriggerOptions());
        boost::shared_ptr<PileupOptions> pileup_options(new PileupOptions());

        jec_options->setDelegate(analyzer->getJetEnergyCorrectionDelegate());
        synch_selector_options->setDelegate(analyzer->getSynchSelectorDelegate());
        cut_2d_selector_options->setDelegate(analyzer->getCut2DSelectorDelegate());
        trigger_options->setDelegate(analyzer.get());
        pileup_options->setDelegate(analyzer->getPileupDelegate());

        app->addOptions(*jec_options);
        app->addOptions(*synch_selector_options);
        app->addOptions(*cut_2d_selector_options);
        app->addOptions(*trigger_options);
        app->addOptions(*pileup_options);

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

            TH1Ptr npv = convert(*analyzer->npv());
            npv->SetName("npv");
            npv->GetXaxis()->SetTitle("N_{PV}");
            npv->GetXaxis()->SetTitleSize(0.045);
            npv->SetMarkerSize(0.1);

            TH1Ptr npv_with_pileup = convert(*analyzer->npvWithPileup());
            npv_with_pileup->SetName("npv_with_pileup");
            npv_with_pileup->GetXaxis()->SetTitle("N_{PV}^{with PU}");
            npv_with_pileup->GetXaxis()->SetTitleSize(0.045);
            npv_with_pileup->SetMarkerSize(0.1);

            TH1Ptr njets = convert(*analyzer->njets());
            njets->SetName("njets");
            njets->GetXaxis()->SetTitle("N_{jet}");
            njets->GetXaxis()->SetTitleSize(0.045);
            njets->SetMarkerSize(0.1);

            TH1Ptr d0 = convert(*analyzer->d0());
            d0->SetName("d0");
            d0->GetXaxis()->SetTitle("i.p. [cm]");
            d0->GetXaxis()->SetTitleSize(0.045);
            d0->SetMarkerSize(0.1);

            TH1Ptr htlep = convert(*analyzer->htlep());
            htlep->SetName("htlep");
            htlep->GetXaxis()->SetTitle("H_{T}^{lep} [GeV/c]");
            htlep->GetXaxis()->SetTitleSize(0.045);

            TH1Ptr htlep_before_htlep = convert(*analyzer->htlepBeforeCut());
            htlep_before_htlep->SetName("htlep_before_htlep");
            htlep_before_htlep->GetXaxis()->SetTitle("H_{T}^{lep} [GeV/c]");
            htlep_before_htlep->GetXaxis()->SetTitleSize(0.045);

            TH1Ptr mttbar_before_htlep = convert(*analyzer->mttbarBeforeHtlep());
            mttbar_before_htlep->SetName("mttbar_before_htlep");
            mttbar_before_htlep->GetXaxis()->SetTitle("M_{t#bar{t}} [TeV/c^{2}]");
            mttbar_before_htlep->GetXaxis()->SetTitleSize(0.045);

            TH1Ptr mttbar_after_htlep = convert(*analyzer->mttbarAfterHtlep());
            mttbar_after_htlep->SetName("mttbar_after_htlep");
            mttbar_after_htlep->GetXaxis()->SetTitle("M_{t#bar{t}} [TeV/c^{2}]");
            mttbar_after_htlep->GetXaxis()->SetTitleSize(0.045);

            TH2Ptr dr_vs_ptrel = convert(*analyzer->drVsPtrel());
            dr_vs_ptrel->SetName("dr_vs_ptrel");
            dr_vs_ptrel->GetXaxis()->SetTitle("p_{T}^{rel}(jet,e) [GeV/c]");
            dr_vs_ptrel->GetXaxis()->SetTitleSize(0.045);
            dr_vs_ptrel->GetYaxis()->SetTitle("#Delta R");
            dr_vs_ptrel->GetYaxis()->SetTitleSize(0.045);

            TH1Ptr ttbar_pt = convert(*analyzer->ttbarPt());
            ttbar_pt->SetName("ttbar_pt");
            ttbar_pt->GetXaxis()->SetTitle("p_{T}^{t#bar{t}} [GeV/c]");
            ttbar_pt->GetXaxis()->SetTitleSize(0.045);
            ttbar_pt->SetMarkerSize(0.1);

            TH1Ptr wlep_mt = convert(*analyzer->wlepMt());
            wlep_mt->SetName("wlep_mt");
            wlep_mt->GetXaxis()->SetTitle("M_{T}^{W,lep} [GeV/c^{2}]");
            wlep_mt->GetXaxis()->SetTitleSize(0.045);
            wlep_mt->SetMarkerSize(0.1);

            TH1Ptr whad_mt = convert(*analyzer->whadMt());
            whad_mt->SetName("whad_mt");
            whad_mt->GetXaxis()->SetTitle("M_{T}^{W,had} [GeV/c^{2}]");
            whad_mt->GetXaxis()->SetTitleSize(0.045);
            whad_mt->SetMarkerSize(0.1);

            TH1Ptr wlep_mass = convert(*analyzer->wlepMass());
            wlep_mass->SetName("wlep_mass");
            wlep_mass->GetXaxis()->SetTitle("M^{W,lep} [GeV/c^{2}]");
            wlep_mass->GetXaxis()->SetTitleSize(0.045);
            wlep_mass->SetMarkerSize(0.1);

            TH1Ptr whad_mass = convert(*analyzer->whadMass());
            whad_mass->SetName("whad_mass");
            whad_mass->GetXaxis()->SetTitle("M^{W,had} [GeV/c^{2}]");
            whad_mass->GetXaxis()->SetTitleSize(0.045);
            whad_mass->SetMarkerSize(0.1);

            TH2Ptr ljet_met_dphi_vs_met = convert(*analyzer->ljetMetDphivsMet());
            ljet_met_dphi_vs_met->SetName("ljet_met_dphi_vs_met");
            ljet_met_dphi_vs_met->GetXaxis()->SetTitle("MET [GeV/c]");
            ljet_met_dphi_vs_met->GetXaxis()->SetTitleSize(0.045);
            ljet_met_dphi_vs_met->GetYaxis()->SetTitle("#Delta #phi(jet1, MET)) [rad]");
            ljet_met_dphi_vs_met->GetYaxis()->SetTitleSize(0.045);

            TH2Ptr lepton_met_dphi_vs_met = convert(*analyzer->leptonMetDphivsMet());
            lepton_met_dphi_vs_met->SetName("lepton_met_dphi_vs_met");
            lepton_met_dphi_vs_met->GetXaxis()->SetTitle("MET [GeV/c]");
            lepton_met_dphi_vs_met->GetXaxis()->SetTitleSize(0.045);
            lepton_met_dphi_vs_met->GetYaxis()->SetTitle("#Delta #phi(e, MET)) [rad]");
            lepton_met_dphi_vs_met->GetYaxis()->SetTitleSize(0.045);

            shared_ptr<P4Canvas> first_jet(new P4Canvas("First jet", "jet1"));
            shared_ptr<P4Canvas> second_jet(new P4Canvas("Second jet", "jet2"));
            shared_ptr<P4Canvas> third_jet(new P4Canvas("Third jet", "jet3"));
            shared_ptr<P4Canvas> electron(new P4Canvas("Electron", "e"));

            shared_ptr<P4Canvas> ltop(new P4Canvas("ltop", "ltop"));
            shared_ptr<P4Canvas> htop(new P4Canvas("htop", "htop"));

            if (app->output())
            {
                npv->Write();
                npv_with_pileup->Write();
                njets->Write();
                d0->Write();
                htlep->Write();
                htlep_before_htlep->Write();
                mttbar_before_htlep->Write();
                mttbar_after_htlep->Write();
                dr_vs_ptrel->Write();

                ttbar_pt->Write();
                wlep_mt->Write();
                whad_mt->Write();
                wlep_mass->Write();
                whad_mass->Write();

                ljet_met_dphi_vs_met->Write();
                lepton_met_dphi_vs_met->Write();

                first_jet->write(app->output().get(), *analyzer->firstJet());
                second_jet->write(app->output().get(), *analyzer->secondJet());
                third_jet->write(app->output().get(), *analyzer->thirdJet());

                electron->write(app->output().get(), *analyzer->electron());

                ltop->write(app->output().get(), *analyzer->ltop());
                htop->write(app->output().get(), *analyzer->htop());
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
                canvas2->SetWindowSize(1200, 600);
                canvas2->Divide(4, 2);

                canvas2->cd(1);
                d0->Draw("hist");

                canvas2->cd(3);
                njets->Draw("hist");

                canvas2->cd(4);
                ttbar_pt->Draw("hist");

                canvas2->cd(5);
                wlep_mt->Draw("hist");

                canvas2->cd(6);
                whad_mt->Draw("hist");

                canvas2->cd(7);
                wlep_mass->Draw("hist");

                canvas2->cd(8);
                whad_mass->Draw("hist");

                shared_ptr<TCanvas> canvas3(new TCanvas());
                canvas3->SetTitle("Primary Vertices");
                canvas3->SetWindowSize(640, 400);
                canvas3->Divide(2, 1);

                canvas3->cd(1);
                npv->Draw("hist");

                canvas3->cd(2);
                npv_with_pileup->Draw("hist");

                first_jet->draw(*analyzer->firstJet());
                second_jet->draw(*analyzer->secondJet());
                third_jet->draw(*analyzer->thirdJet());

                electron->draw(*analyzer->electron());

                ltop->draw(*analyzer->ltop());
                htop->draw(*analyzer->htop());

                root->Run();
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
