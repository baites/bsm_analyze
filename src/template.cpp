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
#include "interface/Btag.h"
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
        boost::shared_ptr<TemplatesOptions> templates_options(new TemplatesOptions());
        boost::shared_ptr<BtagOptions> btag_options(new BtagOptions());

        jec_options->setDelegate(analyzer->getJetEnergyCorrectionDelegate());
        synch_selector_options->setDelegate(analyzer->getSynchSelectorDelegate());
        cut_2d_selector_options->setDelegate(analyzer->getCut2DSelectorDelegate());
        trigger_options->setDelegate(analyzer->getTriggerDelegate());
        pileup_options->setDelegate(analyzer->getPileupDelegate());
        templates_options->setDelegate(analyzer.get());
        btag_options->setDelegate(analyzer->getBtagDelegate());

        app->addOptions(*jec_options);
        app->addOptions(*synch_selector_options);
        app->addOptions(*cut_2d_selector_options);
        app->addOptions(*trigger_options);
        app->addOptions(*pileup_options);
        app->addOptions(*templates_options);
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
            cutflow->GetXaxis()->SetTitle("Cutflow");

            TH1Ptr npv = convert(*analyzer->npv());
            npv->SetName("npv");
            npv->GetXaxis()->SetTitle("N_{PV}");

            TH1Ptr npv_with_pileup = convert(*analyzer->npvWithPileup());
            npv_with_pileup->SetName("npv_with_pileup");
            npv_with_pileup->GetXaxis()->SetTitle("N_{PV}^{with PU}");

            TH1Ptr njets = convert(*analyzer->njets());
            njets->SetName("njets");
            njets->GetXaxis()->SetTitle("N_{jet}");

            TH1Ptr d0 = convert(*analyzer->d0());
            d0->SetName("d0");
            d0->GetXaxis()->SetTitle("i.p. [cm]");

            TH1Ptr htlep = convert(*analyzer->htlep());
            htlep->SetName("htlep");
            htlep->GetXaxis()->SetTitle("H_{T}^{lep} [GeV/c]");

            TH1Ptr htall = convert(*analyzer->htall());
            htall->SetName("htall");
            htall->GetXaxis()->SetTitle("H_{T}^{all} [GeV/c]");

            TH1Ptr htlep_after_htlep = convert(*analyzer->htlepAfterHtlep());
            htlep_after_htlep->SetName("htlep_after_htlep");
            htlep_after_htlep->GetXaxis()->SetTitle("H_{T}^{lep} [GeV/c]");

            TH1Ptr htlep_before_htlep = convert(*analyzer->htlepBeforeHtlep());
            htlep_before_htlep->SetName("htlep_before_htlep");
            htlep_before_htlep->GetXaxis()->SetTitle("H_{T}^{lep} [GeV/c]");

            TH1Ptr htlep_before_htlep_noweight = convert(*analyzer->htlepBeforeHtlepNoWeight());
            htlep_before_htlep_noweight->SetName("htlep_before_htlep_qcd_noweight");
            htlep_before_htlep_noweight->GetXaxis()->SetTitle("H_{T}^{lep} [GeV/c]");

            TH1Ptr solutions = convert(*analyzer->solutions());
            solutions->SetName("solutions");
            solutions->GetXaxis()->SetTitle("N_{solutions}^{#nu}");

            TH1Ptr mttbar_before_htlep = convert(*analyzer->mttbarBeforeHtlep());
            mttbar_before_htlep->SetName("mttbar_before_htlep");
            mttbar_before_htlep->GetXaxis()->SetTitle("M_{t#bar{t}} [TeV/c^{2}]");

            TH1Ptr mttbar_after_htlep = convert(*analyzer->mttbarAfterHtlep());
            mttbar_after_htlep->SetName("mttbar_after_htlep");
            mttbar_after_htlep->GetXaxis()->SetTitle("M_{t#bar{t}} [TeV/c^{2}]");

            TH2Ptr dr_vs_ptrel = convert(*analyzer->drVsPtrel());
            dr_vs_ptrel->SetName("dr_vs_ptrel");
            dr_vs_ptrel->GetXaxis()->SetTitle("p_{T}^{rel}(jet,e) [GeV/c]");
            dr_vs_ptrel->GetYaxis()->SetTitle("#Delta R");

            TH1Ptr ltop_drsum = convert(*analyzer->ltop_drsum());
            ltop_drsum->SetName("ltop_drsum");

            TH1Ptr htop_drsum = convert(*analyzer->htop_drsum());
            htop_drsum->SetName("htop_drsum");

            TH1Ptr htop_dphi = convert(*analyzer->htop_dphi());
            htop_dphi->SetName("htop_dphi");

            TH1Ptr chi2 = convert(*analyzer->chi2());
            chi2->SetName("chi2");

            TH1Ptr ltop_chi2 = convert(*analyzer->ltop_chi2());
            ltop_chi2->SetName("ltop_chi2");

            TH1Ptr htop_chi2 = convert(*analyzer->htop_chi2());
            htop_chi2->SetName("htop_chi2");

            TH1Ptr ttbar_pt = convert(*analyzer->ttbarPt());
            ttbar_pt->SetName("ttbar_pt");
            ttbar_pt->GetXaxis()->SetTitle("p_{T}^{t#bar{t}} [GeV/c]");

            TH1Ptr wlep_mt = convert(*analyzer->wlepMt());
            wlep_mt->SetName("wlep_mt");
            wlep_mt->GetXaxis()->SetTitle("M_{T}^{W,lep} [GeV/c^{2}]");

            TH1Ptr whad_mt = convert(*analyzer->whadMt());
            whad_mt->SetName("whad_mt");
            whad_mt->GetXaxis()->SetTitle("M_{T}^{W,had} [GeV/c^{2}]");

            TH1Ptr wlep_mass = convert(*analyzer->wlepMass());
            wlep_mass->SetName("wlep_mass");
            wlep_mass->GetXaxis()->SetTitle("M^{W,lep} [GeV/c^{2}]");

            TH1Ptr whad_mass = convert(*analyzer->whadMass());
            whad_mass->SetName("whad_mass");
            whad_mass->GetXaxis()->SetTitle("M^{W,had} [GeV/c^{2}]");

            TH1Ptr met = convert(*analyzer->met());
            met->SetName("met");
            met->GetXaxis()->SetTitle("MET [GeV/c]");

            TH1Ptr met_noweight = convert(*analyzer->metNoWeight());
            met_noweight->SetName("met_noweight");
            met_noweight->GetXaxis()->SetTitle("MET [GeV/c]");

            TH2Ptr ljet_met_dphi_vs_met_before_tricut = convert(*analyzer->ljetMetDphivsMetBeforeTricut());
            ljet_met_dphi_vs_met_before_tricut->SetName("ljet_met_dphi_vs_met_before_tricut");
            ljet_met_dphi_vs_met_before_tricut->GetXaxis()->SetTitle("MET [GeV/c]");
            ljet_met_dphi_vs_met_before_tricut->GetYaxis()->SetTitle("#Delta #phi(jet1, MET)) [rad]");

            TH2Ptr lepton_met_dphi_vs_met_before_tricut = convert(*analyzer->leptonMetDphivsMetBeforeTricut());
            lepton_met_dphi_vs_met_before_tricut->SetName("lepton_met_dphi_vs_met_before_tricut");
            lepton_met_dphi_vs_met_before_tricut->GetXaxis()->SetTitle("MET [GeV/c]");
            lepton_met_dphi_vs_met_before_tricut->GetYaxis()->SetTitle("#Delta #phi(e, MET)) [rad]");

            TH2Ptr ljet_met_dphi_vs_met = convert(*analyzer->ljetMetDphivsMet());
            ljet_met_dphi_vs_met->SetName("ljet_met_dphi_vs_met");
            ljet_met_dphi_vs_met->GetXaxis()->SetTitle("MET [GeV/c]");
            ljet_met_dphi_vs_met->GetYaxis()->SetTitle("#Delta #phi(jet1, MET)) [rad]");

            TH2Ptr lepton_met_dphi_vs_met = convert(*analyzer->leptonMetDphivsMet());
            lepton_met_dphi_vs_met->SetName("lepton_met_dphi_vs_met");
            lepton_met_dphi_vs_met->GetXaxis()->SetTitle("MET [GeV/c]");
            lepton_met_dphi_vs_met->GetYaxis()->SetTitle("#Delta #phi(e, MET)) [rad]");

            TH1Ptr htop_njets = convert(*analyzer->htopNjets());
            htop_njets->SetName("htop_njets");
            htop_njets->GetXaxis()->SetTitle("N_{jets}^{htop}");

            TH1Ptr htop_delta_r = convert(*analyzer->htopDeltaR());
            htop_delta_r->SetName("htop_delta_r");
            htop_delta_r->GetXaxis()->SetTitle("#Delta R(jet1^{htop}, jet2^{htop})");

            TH2Ptr htop_njet_vs_m = convert(*analyzer->htopNjetvsM());
            htop_njet_vs_m->SetName("htop_njet_vs_m");
            htop_njet_vs_m->GetXaxis()->SetTitle("M^{htop} [GeV/c^{2}]");
            htop_njet_vs_m->GetYaxis()->SetTitle("N^{htop jet}");

            TH2Ptr htop_pt_vs_m = convert(*analyzer->htopPtvsM());
            htop_pt_vs_m->SetName("htop_pt_vs_m");
            htop_pt_vs_m->GetXaxis()->SetTitle("M^{htop} [GeV/c^{2}]");
            htop_pt_vs_m->GetYaxis()->SetTitle("p_{T}^{htop} [GeV/c]");

            TH2Ptr htop_pt_vs_njets = convert(*analyzer->htopPtvsNjets());
            htop_pt_vs_njets->SetName("htop_pt_vs_njets");
            htop_pt_vs_njets->GetXaxis()->SetTitle("N_jets");
            htop_pt_vs_njets->GetYaxis()->SetTitle("p_{T}^{htop} [GeV/c]");

            TH2Ptr htop_pt_vs_ltop_pt = convert(*analyzer->htopPtvsLtoppt());
            htop_pt_vs_ltop_pt->SetName("htop_pt_vs_ltop_pt");
            htop_pt_vs_ltop_pt->GetXaxis()->SetTitle("p_{T}^{ltop} [GeV/c]");
            htop_pt_vs_ltop_pt->GetYaxis()->SetTitle("p_{T}^{htop} [GeV/c]");

            shared_ptr<P4Canvas> first_jet(new P4Canvas("First jet", "jet1"));
            shared_ptr<P4Canvas> second_jet(new P4Canvas("Second jet", "jet2"));
            shared_ptr<P4Canvas> third_jet(new P4Canvas("Third jet", "jet3"));
            shared_ptr<P4Canvas> electron(new P4Canvas("Electron", "e"));
            shared_ptr<P4Canvas> electron_before_tricut(new P4Canvas("Electron Before Tricut", "e_no_tricut"));

            shared_ptr<P4Canvas> ltop(new P4Canvas("ltop", "ltop"));
            shared_ptr<P4Canvas> htop(new P4Canvas("htop", "htop"));

            shared_ptr<P4Canvas> htop_first_jet(new P4Canvas("htop jet1", "htop_jet1"));
            shared_ptr<P4Canvas> htop_second_jet(new P4Canvas("htop jet2", "htop_jet2"));
            shared_ptr<P4Canvas> htop_third_jet(new P4Canvas("htop jet3", "htop_jet3"));
            shared_ptr<P4Canvas> htop_fourth_jet(new P4Canvas("htop jet4", "htop_jet4"));

            shared_ptr<P4Canvas> ltop_first_jet(new P4Canvas("ltop jet1", "ltop_jet1"));

            TH1Ptr njets_before_reconstruction =
                convert(*analyzer->njetsBeforeReconstruction());
            njets_before_reconstruction->SetName("njets_before_reconstruction");
            njets_before_reconstruction->GetXaxis()->SetTitle("N_{jet}^{before reconstruction}");

            TH1Ptr njet2_dr_lepton_jet1_before_reconstruction =
                convert(*analyzer->njet2DrLeptonJet1BeforeReconstruction());
            njet2_dr_lepton_jet1_before_reconstruction->SetName("njet2_dr_lepton_jet1_before_reconstruction");
            njet2_dr_lepton_jet1_before_reconstruction->GetXaxis()->SetTitle("#Delta R(lepton, jet1)_{N_{jets} = 2}");

            TH1Ptr njet2_dr_lepton_jet2_before_reconstruction =
                convert(*analyzer->njet2DrLeptonJet2BeforeReconstruction());
            njet2_dr_lepton_jet2_before_reconstruction->SetName("njet2_dr_lepton_jet2_before_reconstruction");
            njet2_dr_lepton_jet2_before_reconstruction->GetXaxis()->SetTitle("#Delta R(lepton, jet2)_{N_{jets} = 2}");

            TH1Ptr njets_after_reconstruction =
                convert(*analyzer->njetsAfterReconstruction());
            njets_after_reconstruction->SetName("njets_after_reconstruction");
            njets_after_reconstruction->GetXaxis()->SetTitle("N_{jet}^{after reconstruction}");

            TH1Ptr njet2_dr_lepton_jet1_after_reconstruction =
                convert(*analyzer->njet2DrLeptonJet1AfterReconstruction());
            njet2_dr_lepton_jet1_after_reconstruction->SetName("njet2_dr_lepton_jet1_after_reconstruction");
            njet2_dr_lepton_jet1_after_reconstruction->GetXaxis()->SetTitle("#Delta R(lepton, jet1)_{N_{jets} = 2}");

            TH1Ptr njet2_dr_lepton_jet2_after_reconstruction =
                convert(*analyzer->njet2DrLeptonJet2AfterReconstruction());
            njet2_dr_lepton_jet2_after_reconstruction->SetName("njet2_dr_lepton_jet2_after_reconstruction");
            njet2_dr_lepton_jet2_after_reconstruction->GetXaxis()->SetTitle("#Delta R(lepton, jet2)_{N_{jets} = 2}");

            if (app->output())
            {
                cutflow->Write();

                npv->Write();
                npv_with_pileup->Write();
                njets->Write();
                d0->Write();
                htlep->Write();
                htall->Write();
                htlep_after_htlep->Write();
                htlep_before_htlep->Write();
                htlep_before_htlep_noweight->Write();           
                solutions->Write();
                mttbar_before_htlep->Write();
                mttbar_after_htlep->Write();
                dr_vs_ptrel->Write();

                ttbar_pt->Write();
                wlep_mt->Write();
                whad_mt->Write();
                wlep_mass->Write();
                whad_mass->Write();
                met->Write();
                met_noweight->Write();

                ltop_drsum->Write();
                htop_drsum->Write();
                htop_dphi->Write();

                chi2->Write();
                ltop_chi2->Write();
                htop_chi2->Write();

                ljet_met_dphi_vs_met_before_tricut->Write();
                lepton_met_dphi_vs_met_before_tricut->Write();
                ljet_met_dphi_vs_met->Write();
                lepton_met_dphi_vs_met->Write();

                htop_njets->Write();
                htop_delta_r->Write();
                htop_njet_vs_m->Write();
                htop_pt_vs_m->Write();
                htop_pt_vs_njets->Write();
                htop_pt_vs_ltop_pt->Write();

                njets_before_reconstruction->Write();
                njet2_dr_lepton_jet1_before_reconstruction->Write();
                njet2_dr_lepton_jet2_before_reconstruction->Write();

                njets_after_reconstruction->Write();
                njet2_dr_lepton_jet1_after_reconstruction->Write();
                njet2_dr_lepton_jet2_after_reconstruction->Write();

                first_jet->write(*analyzer->firstJet(), app->output().get());
                second_jet->write(*analyzer->secondJet(), app->output().get());
                third_jet->write(*analyzer->thirdJet(), app->output().get());

                electron->write(*analyzer->electron(), app->output().get());
                electron_before_tricut->write(*analyzer->electronBeforeTricut(),
                        app->output().get());

                ltop->write(*analyzer->ltop(), app->output().get());
                htop->write(*analyzer->htop(), app->output().get());

                htop_first_jet->write(*analyzer->htopJet1(), app->output().get());
                htop_second_jet->write(*analyzer->htopJet2(), app->output().get());
                htop_third_jet->write(*analyzer->htopJet3(), app->output().get());
                htop_fourth_jet->write(*analyzer->htopJet4(), app->output().get());

                ltop_first_jet->write(*analyzer->ltopJet1(), app->output().get());
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
                electron_before_tricut->draw(*analyzer->electronBeforeTricut());

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
