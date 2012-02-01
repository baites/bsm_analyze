// Generate Template plots and comparison:
//
//  [MC]        hTlep, Mttbar in regions S1, S2, Signal
//  [Data]      hTlep, Mttbar in regions S1, S2
//  [MC/Data]   compare hTlep, Mttbar in S1 + S2
//
// Created by Samvel Khalatyan, Aug 29, 2011
// Copyright 2011, All rights reserved

#include <cfloat>
#include <iostream>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Electron.pb.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/GenParticle.pb.h"
#include "bsm_input/interface/Input.pb.h"
#include "bsm_input/interface/Jet.pb.h"
#include "bsm_input/interface/Muon.pb.h"
#include "bsm_input/interface/Physics.pb.h"
#include "bsm_stat/interface/H1.h"
#include "bsm_stat/interface/H2.h"
#include "interface/Algorithm.h"
#include "interface/CorrectedJet.h"
#include "interface/Cut.h"
#include "interface/Monitor.h"
#include "interface/StatProxy.h"
#include "interface/TemplateAnalyzer.h"
#include "interface/Utility.h"

using namespace std;
using namespace boost;

using bsm::TemplateAnalyzer;
using bsm::WDecay;
using bsm::TemplatesDelegate;
using bsm::TemplatesOptions;

TemplatesOptions::TemplatesOptions()
{
    _delegate = 0;

    _description.reset(new po::options_description("Templates Options"));
    _description->add_options()
        ("wjet-correction",
         po::value<bool>()->implicit_value(true)->notifier(
             boost::bind(&TemplatesOptions::setWjetCorrection, this)),
         "Correct Wjets scale-down sample")

        ("btag-reconstruction",
         po::value<bool>()->notifier(
             boost::bind(&TemplatesOptions::setBtagReconstruction, this)),
         "Use b-tag jets in reconstruction")

        ("simple-dr-reconstruction",
         po::value<bool>()->notifier(
             boost::bind(&TemplatesOptions::setSimpleDrReconstruction, this)),
         "Use simple delta-r based reconstruction")
    ;
}

void TemplatesOptions::setDelegate(TemplatesDelegate *delegate)
{
    if (_delegate != delegate)
        _delegate = delegate;
}

TemplatesDelegate *TemplatesOptions::delegate() const
{
    return _delegate;
}

// Options interface
//
TemplatesOptions::DescriptionPtr
    TemplatesOptions::description() const
{
    return _description;
}

// Private
//
void TemplatesOptions::setWjetCorrection()
{
    if (!delegate())
        return;

    delegate()->setWjetCorrection();
}

void TemplatesOptions::setBtagReconstruction()
{
    if (!delegate())
        return;

    delegate()->setBtagReconstruction();
}

void TemplatesOptions::setSimpleDrReconstruction()
{
    if (!delegate())
        return;

    delegate()->setSimpleDrReconstruction();
}



// -- Template Analyzer -------------------------------------------------------
//
TemplateAnalyzer::TemplateAnalyzer():
    _use_pileup(false),
    _wjets_input(false),
    _apply_wjet_correction(false)
{
    _synch_selector.reset(new SynchSelector());
    monitor(_synch_selector);

    // Assign cutflow delegate
    //
    for(uint32_t cut = 0; SynchSelector::SELECTIONS > cut; ++cut)
    {
        Counter *counter = _synch_selector->cutflow()->cut(cut)->events().get();
        _counters[counter] = cut;

        counter->setDelegate(this);
    }

    _secondary_lepton_counter =
        _synch_selector->cutflow()->cut(
                SynchSelector::VETO_SECOND_MUON)->objects().get();
    _secondary_lepton_counter->setDelegate(this);

    _leading_jet_counter =
        _synch_selector->cutflow()->cut(
                SynchSelector::LEADING_JET)->objects().get();
    _leading_jet_counter->setDelegate(this);

    _htlep_counter =
        _synch_selector->cutflow()->cut(
                SynchSelector::HTLEP)->objects().get();
    _htlep_counter->setDelegate(this);

    _cutflow.reset(new H1Proxy(SynchSelector::SELECTIONS, 0,
                SynchSelector::SELECTIONS));
    monitor(_cutflow);

    _npv.reset(new H1Proxy(25, 0, 25));
    monitor(_npv);

    _npv_with_pileup.reset(new H1Proxy(25, 0, 25));
    monitor(_npv_with_pileup);

    _njets.reset(new H1Proxy(15, 0, 15));
    monitor(_njets);

    _d0.reset(new H1Proxy(500, 0, .05));
    monitor(_d0);

    _htlep.reset(new H1Proxy(500, 0, 500));
    monitor(_htlep);

    _htall.reset(new H1Proxy(500, 300, 1600));
    monitor(_htall);

    _htlep_after_htlep.reset(new H1Proxy(500, 0, 500));
    monitor(_htlep_after_htlep);

    _htlep_before_htlep.reset(new H1Proxy(50, 100, 150));
    monitor(_htlep_before_htlep);

    _htlep_before_htlep_noweight.reset(new H1Proxy(50, 100, 150));
    monitor(_htlep_before_htlep_noweight); 

    _solutions.reset(new H1Proxy(3, 0, 3));
    monitor(_solutions);

    _mttbar_before_htlep.reset(new H1Proxy(4000, 0, 4));
    monitor(_mttbar_before_htlep);

    _mttbar_after_htlep.reset(new H1Proxy(4000, 0, 4));
    monitor(_mttbar_after_htlep);

    _dr_vs_ptrel.reset(new H2Proxy(100, 0, 100, 15, 0, 1.5));
    monitor(_dr_vs_ptrel);

    _ttbar_pt.reset(new H1Proxy(500, 0, 500));
    monitor(_ttbar_pt);

    _wlep_mt.reset(new H1Proxy(500, 0, 500));
    monitor(_wlep_mt);

    _whad_mt.reset(new H1Proxy(500, 0, 500));
    monitor(_whad_mt);

    _wlep_mass.reset(new H1Proxy(500, 0, 500));
    monitor(_wlep_mass);

    _whad_mass.reset(new H1Proxy(500, 0, 500));
    monitor(_whad_mass);

    _met.reset(new H1Proxy(500, 0, 500));
    monitor(_met);

    _met_noweight.reset(new H1Proxy(500, 0, 500));
    monitor(_met_noweight);

    _ljet_met_dphi_vs_met_before_tricut.reset(new H2Proxy(500, 0, 500, 400, 0, 4));
    monitor(_ljet_met_dphi_vs_met_before_tricut);

    _lepton_met_dphi_vs_met_before_tricut.reset(new H2Proxy(500, 0, 500, 400, 0, 4));
    monitor(_lepton_met_dphi_vs_met_before_tricut);

    _ljet_met_dphi_vs_met.reset(new H2Proxy(500, 0, 500, 400, 0, 4));
    monitor(_ljet_met_dphi_vs_met);

    _lepton_met_dphi_vs_met.reset(new H2Proxy(500, 0, 500, 400, 0, 4));
    monitor(_lepton_met_dphi_vs_met);

    _htop_njets.reset(new H1Proxy(10, 0, 10));
    monitor(_htop_njets);

    _htop_delta_r.reset(new H1Proxy(500, 0, 5));
    monitor(_htop_delta_r);

    _htop_njet_vs_m.reset(new H2Proxy(1000, 0, 1, 10, 0, 10));
    monitor(_htop_njet_vs_m);

    _htop_pt_vs_m.reset(new H2Proxy(1000, 0, 1, 10, 0, 10));
    monitor(_htop_pt_vs_m);

    _htop_pt_vs_njets.reset(new H2Proxy(5, 0, 5, 1000, 0, 1));
    monitor(_htop_pt_vs_njets);

    _htop_pt_vs_ltop_pt.reset(new H2Proxy(1000, 0, 1, 1000, 0, 1));
    monitor(_htop_pt_vs_ltop_pt);

    _event = 0;

    _first_jet.reset(new P4Monitor());
    _second_jet.reset(new P4Monitor());
    _third_jet.reset(new P4Monitor());
    _electron.reset(new P4Monitor());
    _electron_before_tricut.reset(new P4Monitor());

    _ltop.reset(new P4Monitor());
    _ltop->mt()->mutable_axis()->init(1000, 0, 1000);
    _ltop->pt()->mutable_axis()->init(1000, 0, 1000);

    _htop.reset(new P4Monitor());
    _htop->mt()->mutable_axis()->init(1000, 0, 1000);
    _htop->pt()->mutable_axis()->init(1000, 0, 1000);

    monitor(_first_jet);
    monitor(_second_jet);
    monitor(_third_jet);
    monitor(_electron);
    monitor(_electron_before_tricut);

    monitor(_ltop);
    monitor(_htop);

    _htop_jet1.reset(new P4Monitor());
    _htop_jet1->mass()->mutable_axis()->init(250, 0, 250);

    _htop_jet2.reset(new P4Monitor());
    _htop_jet2->mass()->mutable_axis()->init(100, 0, 100);

    _htop_jet3.reset(new P4Monitor());
    _htop_jet3->mass()->mutable_axis()->init(100, 0, 100);

    _htop_jet4.reset(new P4Monitor());
    _htop_jet4->mass()->mutable_axis()->init(100, 0, 100);

    _ltop_jet1.reset(new P4Monitor());
    _ltop_jet1->mass()->mutable_axis()->init(100, 0, 100);

    monitor(_htop_jet1);
    monitor(_htop_jet2);
    monitor(_htop_jet3);
    monitor(_htop_jet4);

    monitor(_ltop_jet1);

    _pileup.reset(new Pileup());
    monitor(_pileup);

    _reconstructor.reset(new SimpleResonanceReconstructor());
    monitor(_reconstructor);
}

TemplateAnalyzer::TemplateAnalyzer(const TemplateAnalyzer &object):
    _use_pileup(false),
    _wjets_input(false),
    _apply_wjet_correction(object._apply_wjet_correction)
{
    _synch_selector = 
        dynamic_pointer_cast<SynchSelector>(object._synch_selector->clone());
    monitor(_synch_selector);

    // Assign cutflow delegate
    //
    for(uint32_t cut = 0; SynchSelector::SELECTIONS > cut; ++cut)
    {
        Counter *counter = _synch_selector->cutflow()->cut(cut)->events().get();
        _counters[counter] = cut;

        counter->setDelegate(this);
    }

    _secondary_lepton_counter =
        _synch_selector->cutflow()->cut(
                SynchSelector::VETO_SECOND_MUON)->objects().get();
    _secondary_lepton_counter->setDelegate(this);

    _leading_jet_counter =
        _synch_selector->cutflow()->cut(
                SynchSelector::LEADING_JET)->objects().get();
    _leading_jet_counter->setDelegate(this);

    _htlep_counter =
        _synch_selector->cutflow()->cut(
                SynchSelector::HTLEP)->objects().get();
    _htlep_counter->setDelegate(this);

    _cutflow = dynamic_pointer_cast<H1Proxy>(object._cutflow->clone());
    monitor(_cutflow);

    _npv = dynamic_pointer_cast<H1Proxy>(object._npv->clone());
    monitor(_npv);

    _npv_with_pileup = dynamic_pointer_cast<H1Proxy>(object._npv_with_pileup->clone());
    monitor(_npv_with_pileup);

    _njets = dynamic_pointer_cast<H1Proxy>(object._njets->clone());
    monitor(_njets);

    _d0 = dynamic_pointer_cast<H1Proxy>(object._d0->clone());
    monitor(_d0);

    _htlep = dynamic_pointer_cast<H1Proxy>(object._htlep->clone());
    monitor(_htlep);

    _htall = dynamic_pointer_cast<H1Proxy>(object._htall->clone());
    monitor(_htall);

    _htlep_after_htlep = dynamic_pointer_cast<H1Proxy>(object._htlep_after_htlep->clone());
    monitor(_htlep_after_htlep);

    _htlep_before_htlep = dynamic_pointer_cast<H1Proxy>(object._htlep_before_htlep->clone());
    monitor(_htlep_before_htlep);

    _htlep_before_htlep_noweight = dynamic_pointer_cast<H1Proxy>(object._htlep_before_htlep_noweight->clone());
    monitor(_htlep_before_htlep_noweight);

    _solutions = dynamic_pointer_cast<H1Proxy>(object._solutions->clone());
    monitor(_solutions);

    _mttbar_before_htlep =
        dynamic_pointer_cast<H1Proxy>(object._mttbar_before_htlep->clone());
    monitor(_mttbar_before_htlep);

    _mttbar_after_htlep =
        dynamic_pointer_cast<H1Proxy>(object._mttbar_after_htlep->clone());
    monitor(_mttbar_after_htlep);

    _dr_vs_ptrel = dynamic_pointer_cast<H2Proxy>(object._dr_vs_ptrel->clone());
    monitor(_dr_vs_ptrel);

    _ttbar_pt = dynamic_pointer_cast<H1Proxy>(object._ttbar_pt->clone());
    monitor(_ttbar_pt);

    _wlep_mt = dynamic_pointer_cast<H1Proxy>(object._wlep_mt->clone());
    monitor(_wlep_mt);

    _whad_mt = dynamic_pointer_cast<H1Proxy>(object._whad_mt->clone());
    monitor(_whad_mt);

    _wlep_mass = dynamic_pointer_cast<H1Proxy>(object._wlep_mass->clone());
    monitor(_wlep_mass);

    _whad_mass = dynamic_pointer_cast<H1Proxy>(object._whad_mass->clone());
    monitor(_whad_mass);

    _met = dynamic_pointer_cast<H1Proxy>(object._met->clone());
    monitor(_met);

    _met_noweight = dynamic_pointer_cast<H1Proxy>(object._met_noweight->clone());
    monitor(_met_noweight);

    _ljet_met_dphi_vs_met_before_tricut = dynamic_pointer_cast<H2Proxy>(
            object._ljet_met_dphi_vs_met_before_tricut->clone());
    monitor(_ljet_met_dphi_vs_met_before_tricut);

    _lepton_met_dphi_vs_met_before_tricut = dynamic_pointer_cast<H2Proxy>(
            object._lepton_met_dphi_vs_met_before_tricut->clone());
    monitor(_lepton_met_dphi_vs_met_before_tricut);

    _ljet_met_dphi_vs_met = dynamic_pointer_cast<H2Proxy>(
            object._ljet_met_dphi_vs_met->clone());
    monitor(_ljet_met_dphi_vs_met);

    _lepton_met_dphi_vs_met = dynamic_pointer_cast<H2Proxy>(
            object._lepton_met_dphi_vs_met->clone());
    monitor(_lepton_met_dphi_vs_met);

    _htop_njets = dynamic_pointer_cast<H1Proxy>(
            object._htop_njets->clone());
    monitor(_htop_njets);

    _htop_delta_r = dynamic_pointer_cast<H1Proxy>(
            object._htop_delta_r->clone());
    monitor(_htop_delta_r);

    _htop_njet_vs_m = dynamic_pointer_cast<H2Proxy>(
            object._htop_njet_vs_m->clone());
    monitor(_htop_njet_vs_m);

    _htop_pt_vs_m = dynamic_pointer_cast<H2Proxy>(
            object._htop_pt_vs_m->clone());
    monitor(_htop_pt_vs_m);

    _htop_pt_vs_njets = dynamic_pointer_cast<H2Proxy>(
            object._htop_pt_vs_njets->clone());
    monitor(_htop_pt_vs_njets);

    _htop_pt_vs_ltop_pt = dynamic_pointer_cast<H2Proxy>(
            object._htop_pt_vs_ltop_pt->clone());
    monitor(_htop_pt_vs_ltop_pt);

    _event = 0;

    _first_jet =
        dynamic_pointer_cast<P4Monitor>(object._first_jet->clone());

    _second_jet =
        dynamic_pointer_cast<P4Monitor>(object._second_jet->clone());

    _third_jet =
        dynamic_pointer_cast<P4Monitor>(object._third_jet->clone());

    _electron =
        dynamic_pointer_cast<P4Monitor>(object._electron->clone());

    _electron_before_tricut =
        dynamic_pointer_cast<P4Monitor>(object._electron_before_tricut->clone());

    _ltop =
        dynamic_pointer_cast<P4Monitor>(object._ltop->clone());

    _htop =
        dynamic_pointer_cast<P4Monitor>(object._htop->clone());

    monitor(_first_jet);
    monitor(_second_jet);
    monitor(_third_jet);
    monitor(_electron);
    monitor(_electron_before_tricut);

    monitor(_ltop);
    monitor(_htop);

    _htop_jet1 = dynamic_pointer_cast<P4Monitor>(object._htop_jet1->clone());
    _htop_jet2 = dynamic_pointer_cast<P4Monitor>(object._htop_jet2->clone());
    _htop_jet3 = dynamic_pointer_cast<P4Monitor>(object._htop_jet3->clone());
    _htop_jet4 = dynamic_pointer_cast<P4Monitor>(object._htop_jet4->clone());
    _ltop_jet1 = dynamic_pointer_cast<P4Monitor>(object._ltop_jet1->clone());

    monitor(_htop_jet1);
    monitor(_htop_jet2);
    monitor(_htop_jet3);
    monitor(_htop_jet4);

    monitor(_ltop_jet1);

    _pileup =
        dynamic_pointer_cast<Pileup>(object._pileup->clone());
    monitor(_pileup);

    _reconstructor = 
        dynamic_pointer_cast<ResonanceReconstructor>(object._reconstructor->clone());
    monitor(_reconstructor);
}

void TemplateAnalyzer::setBtagReconstruction()
{
    stopMonitor(_reconstructor);

    _reconstructor.reset(new BtagResonanceReconstructor());
    monitor(_reconstructor);
}

void TemplateAnalyzer::setSimpleDrReconstruction()
{
    stopMonitor(_reconstructor);

    _reconstructor.reset(new SimpleDrResonanceReconstructor());
    monitor(_reconstructor);
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::cutflow() const
{
    return _cutflow->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::npv() const
{
    return _npv->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::npvWithPileup() const
{
    return _npv_with_pileup->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::njets() const
{
    return _njets->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::d0() const
{
    return _d0->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::htlep() const
{
    return _htlep->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::htall() const
{
    return _htall->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::htlepAfterHtlep() const
{
    return _htlep_after_htlep->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::htlepBeforeHtlep() const
{
    return _htlep_before_htlep->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::htlepBeforeHtlepNoWeight() const
{
    return _htlep_before_htlep_noweight->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::solutions() const
{
    return _solutions->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::mttbarBeforeHtlep() const
{
    return _mttbar_before_htlep->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::mttbarAfterHtlep() const
{
    return _mttbar_after_htlep->histogram();
}

const TemplateAnalyzer::H2Ptr TemplateAnalyzer::drVsPtrel() const
{
    return _dr_vs_ptrel->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::ttbarPt() const
{
    return _ttbar_pt->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::wlepMt() const
{
    return _wlep_mt->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::whadMt() const
{
    return _whad_mt->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::wlepMass() const
{
    return _wlep_mass->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::whadMass() const
{
    return _whad_mass->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::met() const
{
    return _met->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::metNoWeight() const
{
    return _met_noweight->histogram();
}

const TemplateAnalyzer::H2Ptr TemplateAnalyzer::ljetMetDphivsMetBeforeTricut() const
{
    return _ljet_met_dphi_vs_met_before_tricut->histogram();
}

const TemplateAnalyzer::H2Ptr TemplateAnalyzer::leptonMetDphivsMetBeforeTricut() const
{
    return _lepton_met_dphi_vs_met_before_tricut->histogram();
}

const TemplateAnalyzer::H2Ptr TemplateAnalyzer::ljetMetDphivsMet() const
{
    return _ljet_met_dphi_vs_met->histogram();
}

const TemplateAnalyzer::H2Ptr TemplateAnalyzer::leptonMetDphivsMet() const
{
    return _lepton_met_dphi_vs_met->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::htopNjets() const
{
    return _htop_njets->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::htopDeltaR() const
{
    return _htop_delta_r->histogram();
}

const TemplateAnalyzer::H2Ptr TemplateAnalyzer::htopNjetvsM() const
{
    return _htop_njet_vs_m->histogram();
}

const TemplateAnalyzer::H2Ptr TemplateAnalyzer::htopPtvsM() const
{
    return _htop_pt_vs_m->histogram();
}

const TemplateAnalyzer::H2Ptr TemplateAnalyzer::htopPtvsNjets() const
{
    return _htop_pt_vs_njets->histogram();
}

const TemplateAnalyzer::H2Ptr TemplateAnalyzer::htopPtvsLtoppt() const
{
    return _htop_pt_vs_ltop_pt->histogram();
}

const TemplateAnalyzer::P4MonitorPtr TemplateAnalyzer::firstJet() const
{
    return _first_jet;
}

const TemplateAnalyzer::P4MonitorPtr TemplateAnalyzer::secondJet() const
{
    return _second_jet;
}

const TemplateAnalyzer::P4MonitorPtr TemplateAnalyzer::thirdJet() const
{
    return _third_jet;
}

const TemplateAnalyzer::P4MonitorPtr TemplateAnalyzer::electron() const
{
    return _electron;
}

const TemplateAnalyzer::P4MonitorPtr TemplateAnalyzer::electronBeforeTricut() const
{
    return _electron_before_tricut;
}

const TemplateAnalyzer::P4MonitorPtr TemplateAnalyzer::ltop() const
{
    return _ltop;
}

const TemplateAnalyzer::P4MonitorPtr TemplateAnalyzer::htop() const
{
    return _htop;
}

const TemplateAnalyzer::P4MonitorPtr TemplateAnalyzer::htopJet1() const
{
    return _htop_jet1;
}

const TemplateAnalyzer::P4MonitorPtr TemplateAnalyzer::htopJet2() const
{
    return _htop_jet2;
}

const TemplateAnalyzer::P4MonitorPtr TemplateAnalyzer::htopJet3() const
{
    return _htop_jet3;
}

const TemplateAnalyzer::P4MonitorPtr TemplateAnalyzer::htopJet4() const
{
    return _htop_jet4;
}

const TemplateAnalyzer::P4MonitorPtr TemplateAnalyzer::ltopJet1() const
{
    return _ltop_jet1;
}

bsm::JetEnergyCorrectionDelegate
    *TemplateAnalyzer::getJetEnergyCorrectionDelegate() const
{
    return _synch_selector.get();
}

bsm::SynchSelectorDelegate *TemplateAnalyzer::getSynchSelectorDelegate() const
{
    return _synch_selector.get();
}

bsm::Cut2DSelectorDelegate *TemplateAnalyzer::getCut2DSelectorDelegate() const
{
    return _synch_selector->getCut2DSelectorDelegate();
}

bsm::PileupDelegate *TemplateAnalyzer::getPileupDelegate() const
{
    return _pileup.get();
}

bsm::TriggerDelegate *TemplateAnalyzer::getTriggerDelegate() const
{
    return _synch_selector.get();
}

void TemplateAnalyzer::didCounterAdd(const Counter *counter)
{
    if (_counters.end() != _counters.find(counter))
        cutflow()->fill(_counters[counter], _pileup_weight * _wjets_weight);

    if (counter == _secondary_lepton_counter)
        fillDrVsPtrel();
    else if (counter == _htlep_counter)
    {
        const LorentzVector &el_p4 =
            (*_synch_selector->goodElectrons().begin())->physics_object().p4();

        _electron_before_tricut->fill(el_p4, _pileup_weight * _wjets_weight);

        const LorentzVector &missing_energy = *_synch_selector->goodMET();

        ljetMetDphivsMetBeforeTricut()->fill(pt(missing_energy),
                fabs(dphi(*_synch_selector->goodJets()[0].corrected_p4, missing_energy)),
                _pileup_weight * _wjets_weight);

        leptonMetDphivsMetBeforeTricut()->fill(pt(missing_energy),
                fabs(dphi(el_p4, missing_energy)),
                _pileup_weight * _wjets_weight);
    }
}

void TemplateAnalyzer::onFileOpen(const std::string &filename, const Input *input)
{
    if (input->has_type())
    {
        _use_pileup = (Input::DATA != input->type());
        _wjets_input = (Input::WJETS == input->type());
    }
    else
    {
        clog << "Input type is not available: pile-up correction is not applied"
            << endl;

        _use_pileup = false;
        _wjets_input = false;
    }
}

void TemplateAnalyzer::process(const Event *event)
{
    if (!_synch_selector_with_inverted_htlep)
    {
        _synch_selector_with_inverted_htlep =
            dynamic_pointer_cast<SynchSelector>(_synch_selector->clone());

        _synch_selector_with_inverted_htlep->htlep()->invert();
    }

    _pileup_weight = _use_pileup ? 0 : 1;
    _wjets_weight = 1;
    
    if (_apply_wjet_correction
            && _wjets_input)
    {
        WDecay decay = eventDecay(event);

        _wjets_weight *= decay.correction();
    }

    if (!event->has_missing_energy())
        return;

    _event = event;
    if (_use_pileup)
        _pileup_weight = _pileup->scale(event);

    // Process only events, that pass the synch selector
    //
    if (_synch_selector->apply(event))
    {
        Mttbar resonance = mttbar();

        if (100 >= pt(resonance.ltop))
            return;

        _synch_selector->cutflow()->apply(SynchSelector::LTOP);

        mttbarAfterHtlep()->fill(mass(resonance.mttbar) / 1000,
                _pileup_weight * _wjets_weight);

        ttbarPt()->fill(pt(resonance.mttbar), _pileup_weight * _wjets_weight);

        const LorentzVector &el_p4 = _synch_selector->goodElectrons()[0]->physics_object().p4();
        wlepMt()->fill(mt(resonance.neutrino, el_p4),
                _pileup_weight * _wjets_weight);

        wlepMass()->fill(mass(resonance.wlep),
                _pileup_weight * _wjets_weight);
        whadMass()->fill(mass(resonance.whad),
                _pileup_weight * _wjets_weight);

        monitorJets();
        _electron->fill(el_p4, _pileup_weight * _wjets_weight);

        _ltop->fill(resonance.ltop, _pileup_weight * _wjets_weight);
        _htop->fill(resonance.htop, _pileup_weight * _wjets_weight);
        
        npv()->fill(event->primary_vertex().size());
        npvWithPileup()->fill(event->primary_vertex().size(),
                _pileup_weight * _wjets_weight);
        njets()->fill(_synch_selector->goodJets().size(),
                _pileup_weight * _wjets_weight);

        const LorentzVector &missing_energy = *_synch_selector->goodMET();
        ljetMetDphivsMet()->fill(pt(missing_energy),
                fabs(dphi(*_synch_selector->goodJets()[0].corrected_p4, missing_energy)),
                _pileup_weight * _wjets_weight);

        met()->fill(pt(missing_energy), _pileup_weight * _wjets_weight);
        metNoWeight()->fill(pt(missing_energy));

        if (resonance.htop_njets != int(resonance.htop_jets.size()))
            cerr << "inconsistent number of htop jets. htop_njets: "
                << resonance.htop_njets << " htop_jets.size(): "
                << resonance.htop_jets.size() << endl;

        htopNjets()->fill(resonance.htop_jets.size(),
                _pileup_weight * _wjets_weight);

        const ResonanceReconstructor::CorrectedJets &htop_jets =
            resonance.htop_jets;
        if (1 < htop_jets.size())
        {
            htopDeltaR()->fill(dr(*htop_jets[0].corrected_p4, *htop_jets[1].corrected_p4),
                    _pileup_weight * _wjets_weight);
        }

        htopNjetvsM()->fill(mass(resonance.htop), resonance.htop_njets,
                _pileup_weight * _wjets_weight);
        htopPtvsM()->fill(mass(resonance.htop), pt(resonance.htop),
                _pileup_weight * _wjets_weight);

        htopPtvsNjets()->fill(resonance.htop_njets, pt(resonance.htop),
                _pileup_weight * _wjets_weight);

        htopPtvsLtoppt()->fill(pt(resonance.ltop), pt(resonance.htop),
                _pileup_weight * _wjets_weight);

        leptonMetDphivsMet()->fill(pt(missing_energy), fabs(dphi(el_p4, missing_energy)),
                _pileup_weight * _wjets_weight);

        htlep()->fill(htlepValue(), _pileup_weight * _wjets_weight);
        htall()->fill(htallValue(), _pileup_weight * _wjets_weight);
        htlepAfterHtlep()->fill(htlepValue(), _pileup_weight * _wjets_weight);

        solutions()->fill(resonance.solutions);

        if (0 < htop_jets.size())
            htopJet1()->fill(*htop_jets[0].corrected_p4,
                    _pileup_weight * _wjets_weight);
        
        if (1 < htop_jets.size())
            htopJet2()->fill(*htop_jets[1].corrected_p4,
                    _pileup_weight * _wjets_weight);

        if (2 < htop_jets.size())
            htopJet3()->fill(*htop_jets[2].corrected_p4,
                    _pileup_weight * _wjets_weight);

        if (3 < htop_jets.size())
            htopJet4()->fill(*htop_jets[3].corrected_p4,
                    _pileup_weight * _wjets_weight);

        ltopJet1()->fill(resonance.ltop_jet, _pileup_weight * _wjets_weight);
    }

    // Process only events, that pass the synch selector with htlep inverted
    //
    if (_synch_selector_with_inverted_htlep->apply(event))
    {
        Mttbar resonance = mttbar();

        if (pt(resonance.ltop) >= 100.0)
        {
            _synch_selector_with_inverted_htlep->cutflow()->apply(SynchSelector::LTOP);

            htlep()->fill(htlepValue(), _pileup_weight * _wjets_weight);
            htlepBeforeHtlep()->fill(htlepValue(), _pileup_weight * _wjets_weight);
            htlepBeforeHtlepNoWeight()->fill(htlepValue());
            mttbarBeforeHtlep()->fill(mass(mttbar().mttbar) / 1000, _pileup_weight * _wjets_weight);
        }
    } 

    _event = 0;
}

uint32_t TemplateAnalyzer::id() const
{
    return core::ID<TemplateAnalyzer>::get();
}

TemplateAnalyzer::ObjectPtr TemplateAnalyzer::clone() const
{
    return ObjectPtr(new TemplateAnalyzer(*this));
}

void TemplateAnalyzer::merge(const ObjectPtr &pointer)
{
    if (pointer->id() != id())
        return;

    boost::shared_ptr<TemplateAnalyzer> object =
        dynamic_pointer_cast<TemplateAnalyzer>(pointer);

    if (!object)
        return;

    _secondary_lepton_counter->setDelegate(0);
    _leading_jet_counter->setDelegate(0);
    _htlep_counter->setDelegate(0);

    // Reset counter delegates
    //
    for(uint32_t cut = 0; SynchSelector::SELECTIONS < cut; ++cut)
        _synch_selector->cutflow()->cut(cut)->events().get()->setDelegate(0);

    Object::merge(pointer);
}

void TemplateAnalyzer::print(std::ostream &out) const
{
    out << *_synch_selector << endl;
}

// Private
//
void TemplateAnalyzer::fillDrVsPtrel()
{
    // Secondary lepton veto cut passed: find closest jet to the lepton
    //
    const LorentzVector &lepton_p4 =
        (SynchSelector::ELECTRON == _synch_selector->leptonMode()
            ? (*_synch_selector->goodElectrons().begin())->physics_object().p4()
            : (*_synch_selector->goodMuons().begin())->physics_object().p4());

    typedef SynchSelector::GoodJets GoodJets;

    const GoodJets &nice_jets = _synch_selector->niceJets();
    GoodJets::const_iterator closest_jet = nice_jets.end();
    float deltar_min = 999999;

    for(GoodJets::const_iterator jet = nice_jets.begin();
            nice_jets.end() != jet;
            ++jet)
    {
        const float deltar = dr(lepton_p4, *jet->corrected_p4);
        if (deltar < deltar_min)
        {
            deltar_min = deltar;
            closest_jet = jet;
        }
    }

    if (nice_jets.end() == closest_jet)
        return;

    const float ptrel_value = ptrel(lepton_p4, *closest_jet->corrected_p4);
    drVsPtrel()->fill(ptrel_value, deltar_min,  _pileup_weight * _wjets_weight);

    if (5 > ptrel_value)
    {
        if (SynchSelector::ELECTRON == _synch_selector->leptonMode())
        {
            d0()->fill((*_synch_selector->goodElectrons().begin())->extra().d0(),
                    _pileup_weight * _wjets_weight);
        }
        else
        {
            d0()->fill((*_synch_selector->goodMuons().begin())->extra().d0(),
                    _pileup_weight * _wjets_weight);
        }
    }
}

TemplateAnalyzer::Mttbar TemplateAnalyzer::mttbar() const
{
    if (!_event)
    {
        clog << "event is not available: can not reconstruct mttbar" << endl;

        return Mttbar();
    }

    // Note: leptons are kept in a vector of pointers
    //
    const LorentzVector &lepton_p4 =
        SynchSelector::ELECTRON == _synch_selector->leptonMode()
        ? (*_synch_selector->goodElectrons().begin())->physics_object().p4()
        : (*_synch_selector->goodMuons().begin())->physics_object().p4();

    if (10 < _synch_selector->goodJets().size())
    {
        clog << _synch_selector->goodJets().size()
            << " good jets are found: skip hypothesis generation" << endl;

        return Mttbar();
    }

    return _reconstructor->run(lepton_p4,
                               *_synch_selector->goodMET(),
                               _synch_selector->goodJets());
}

void TemplateAnalyzer::monitorJets()
{
    if (_synch_selector->goodJets().size())
        _first_jet->fill(*_synch_selector->goodJets()[0].corrected_p4,
                _pileup_weight * _wjets_weight);

    if (1 < _synch_selector->goodJets().size())
        _second_jet->fill(*_synch_selector->goodJets()[1].corrected_p4,
                _pileup_weight * _wjets_weight);

    if (2 < _synch_selector->goodJets().size())
        _third_jet->fill(*_synch_selector->goodJets()[2].corrected_p4,
                _pileup_weight * _wjets_weight);
}


float TemplateAnalyzer::htlepValue() const
{
    // Note: leptons are kept in a vector of pointers
    const LorentzVector &lepton_p4 =
        SynchSelector::ELECTRON == _synch_selector->leptonMode()
        ? (*_synch_selector->goodElectrons().begin())->physics_object().p4()
        : (*_synch_selector->goodMuons().begin())->physics_object().p4();

    return pt(*_synch_selector->goodMET()) + pt(lepton_p4);
}


float TemplateAnalyzer::htallValue() const
{
    // Computting the HT of the event
    float htjets = 0;
    for(
        SynchSelector::GoodJets::const_iterator jet =
        _synch_selector->goodJets().begin();
        _synch_selector->goodJets().end() != jet;
        ++jet
    )
        htjets += pt(*jet->corrected_p4);

    return htjets + htlepValue();
}

WDecay TemplateAnalyzer::eventDecay(const Event *event) const
{
    WDecay decay;

    const GenParticles &particles = event->gen_particle();
    for(GenParticles::const_iterator particle = particles.begin();
            particles.end() != particle
                && WDecay::UNKNOWN == decay.type();
            ++particle)
    {
        if (3 != particle->status())
            continue;

        /*
        decay = decayType(*particle);

        continue;
        */

        // Skip everything but W-boson
        //
        if (24 != abs(particle->id()))
            continue;

        decay = wdecayType(*particle);
    }

    return decay;
}

WDecay TemplateAnalyzer::decayType(const GenParticle &particle) const
{
    WDecay decay;

    const GenParticles &particles = particle.child();
    for(GenParticles::const_iterator child = particles.begin();
            particles.end() != child
                && WDecay::UNKNOWN == decay.type();
            ++child)
    {
        // Skip all unstable parents
        //
        if (3 != child->status())
            continue;

        // Skip everything but W-boson
        //
        if (24 != abs(child->id()))
            continue;

        decay = wdecayType(*child);
    } // End loop over parents

    return decay;
}

WDecay TemplateAnalyzer::wdecayType(const GenParticle &particle) const
{
    WDecay decay;

    const GenParticles &children = particle.child();
    for(GenParticles::const_iterator child = children.begin();
            children.end() != child
                && WDecay::UNKNOWN == decay.type();
            ++child)
    {
        // Skip unstable particles
        //
        if (3 != child->status())
            continue;

        switch(abs(child->id()))
        {
            case 11: // Electron
                decay.setType(WDecay::ELECTRON);
                break;

            case 13: // Muon
                decay.setType(WDecay::MUON);
                break;

            case 15: // Tau
                decay.setType(WDecay::TAU);
                break;
        }
    } // end loop over children

    return decay;
}
