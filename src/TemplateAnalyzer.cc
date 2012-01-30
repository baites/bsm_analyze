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

using namespace std;
using namespace boost;

using bsm::ResonanceReconstructor;
using bsm::SimpleResonanceReconstructor;
using bsm::BtagResonanceReconstructor;
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



// -- Resonance Reconstructor -------------------------------------------------
//
ResonanceReconstructor::Mttbar ResonanceReconstructor::run(
        const LorentzVector &lepton,
        const LorentzVector &met,
        const SynchSelector::GoodJets &jets) const
{
    Mttbar result;

    // Reconstruct the neutrino pZ and keep solutions in vector for later
    // use
    //
    NeutrinoReconstruct neutrinoReconstruct;
    NeutrinoReconstruct::Solutions neutrinos =
        neutrinoReconstruct(lepton, met);

    result.solutions = neutrinoReconstruct.solutions();

    // Prepare generator and loop over all hypotheses of the decay
    // (different jets assignment to leptonic/hadronic legs)
    //
    Generator generator;
    generator.init(jets);

    // Best Solution should have minimun value of the DeltaRmin:
    //
    //  DeltaRmin = DeltaR(ltop, b) + DeltaR(ltop, l) + DeltaR(ltop, nu)
    //
    // and maximum value of the DeltaR between leptonic and hadronic
    // tops in case the same DeltaRmin is found:
    //
    //  DeltaRlh = DeltaR(ltop, htop)
    //
    struct Solution
    {
        Solution(): deltaRmin(FLT_MAX), deltaRlh(0), htop_njets(0)
        {
        }

        LorentzVector ltop; // Reconstructed leptonic leg
        LorentzVector htop; // Reconstructed hadronic leg
        LorentzVector missing_energy;

        float deltaRmin;
        float deltaRlh;
        int htop_njets;
    } best_solution;

    // Loop over all possible hypotheses and pick the best one
    // Note: take into account all reconstructed neutrino solutions
    //
    do
    {
        Generator::Hypothesis hypothesis = generator.hypothesis();

        if (!isValidHadronicSide(hypothesis.hadronic)
                || !isValidLeptonicSide(hypothesis.leptonic)
                || !isValidNeutralSide(hypothesis.neutral))

                continue;

        // Leptonic Top p4 = leptonP4 + nuP4 + bP4
        // where bP4 is:
        //  - b-tagged jet
        //  - otherwise, the hardest jet (highest pT)
        //
        LorentzVector ltop = lepton;
        LorentzVector ltop_jet = getLeptonicJet(hypothesis.leptonic);
        ltop += ltop_jet;

        // the neutrino will be taken into account later
        //

        // htop is a sum of all jet p4s assigned to the hadronic leg
        //
        LorentzVector htop;
        for(Generator::Iterators::const_iterator jet =
                    hypothesis.hadronic.begin();
                hypothesis.hadronic.end() != jet;
                ++jet)
        {
            htop += *(*jet)->corrected_p4;
        }

        // Take into account all neutrino solutions. Solutions are kept in
        // a vector of pointer
        //
        for(NeutrinoReconstruct::Solutions::const_iterator neutrino =
                    neutrinos.begin();
                neutrinos.end() != neutrino;
                ++neutrino)
        {
            const LorentzVector &neutrino_p4 = *(*neutrino);

            LorentzVector ltop_tmp = ltop;
            ltop_tmp += neutrino_p4;

            const float deltaRmin = dr(ltop_tmp, ltop_jet)
                + dr(ltop_tmp, lepton)
                + dr(ltop_tmp, neutrino_p4);

            const float deltaRlh = dr(ltop_tmp, htop);

            if (deltaRmin < best_solution.deltaRmin
                    || (deltaRmin == best_solution.deltaRmin
                        && deltaRlh > best_solution.deltaRlh))
            {
                best_solution.deltaRmin = deltaRmin;
                best_solution.deltaRlh = deltaRlh;
                best_solution.ltop = ltop_tmp;
                best_solution.htop = htop;
                best_solution.missing_energy = neutrino_p4;
                best_solution.htop_njets = hypothesis.hadronic.size();
            }
        }
    }
    while(generator.next());

    // Best Solution is found
    //
    result.mttbar = best_solution.ltop + best_solution.htop;
    result.wlep = best_solution.missing_energy + lepton;
    result.neutrino = best_solution.missing_energy;
    result.ltop = best_solution.ltop;
    result.htop = best_solution.htop;
    result.htop_njets = best_solution.htop_njets;

    return result;
}

void ResonanceReconstructor::print(std::ostream &out) const
{
}



// -- Simple Resonance Reconstructor ------------------------------------------ 
//
uint32_t SimpleResonanceReconstructor::id() const
{
    return core::ID<SimpleResonanceReconstructor>::get();
}

SimpleResonanceReconstructor::ObjectPtr SimpleResonanceReconstructor::clone() const
{
    return ObjectPtr(new SimpleResonanceReconstructor(*this));
}

// Private
//
bool SimpleResonanceReconstructor::isValidHadronicSide(const Iterators &jets)
    const
{
    return !jets.empty();
}

bool SimpleResonanceReconstructor::isValidLeptonicSide(const Iterators &jets)
    const
{
    return !jets.empty();
}

bool SimpleResonanceReconstructor::isValidNeutralSide(const Iterators &jets)
    const
{
    return true;
}

bsm::LorentzVector SimpleResonanceReconstructor::getLeptonicJet(
        const Iterators &jets) const
{
    const CorrectedJet *hardest_jet = 0;
    float highest_pt = 0;

    // Select the hardest jet (highest pT)
    // Note: hypothesis keeps vector of iterators to Correcte Jets.
    //       Corrected jet has a pointer to the original jet and
    //       corrected P4
    //
    for(Iterators::const_iterator jet = jets.begin(); jets.end() != jet; ++jet)
    {
        const float jet_pt = pt(*(*jet)->corrected_p4);
        if (jet_pt > highest_pt)
            hardest_jet = &*(*jet);
    }

    return *hardest_jet->corrected_p4;
}



// -- Btag Resonance Reconstructor -------------------------------------------- 
//
uint32_t BtagResonanceReconstructor::id() const
{
    return core::ID<BtagResonanceReconstructor>::get();
}

BtagResonanceReconstructor::ObjectPtr BtagResonanceReconstructor::clone() const
{
    return ObjectPtr(new BtagResonanceReconstructor(*this));
}

// Private
//
bool BtagResonanceReconstructor::isValidHadronicSide(const Iterators &jets)
    const
{
    return SimpleResonanceReconstructor::isValidHadronicSide(jets)
        && 1 >= countBtags(jets);
}

bool BtagResonanceReconstructor::isValidLeptonicSide(const Iterators &jets)
    const
{
    return SimpleResonanceReconstructor::isValidLeptonicSide(jets)
        && 1 >= countBtags(jets);
}

bool BtagResonanceReconstructor::isValidNeutralSide(const Iterators &jets)
    const
{
    return 1 > countBtags(jets);
}

bsm::LorentzVector BtagResonanceReconstructor::getLeptonicJet(
        const Iterators &jets) const
{
    const CorrectedJet *hardest_jet = 0;
    float highest_pt = 0;

    // Select the hardest jet (highest pT)
    // Note: hypothesis keeps vector of iterators to Correcte Jets.
    //       Corrected jet has a pointer to the original jet and
    //       corrected P4
    //
    for(Iterators::const_iterator jet = jets.begin(); jets.end() != jet; ++jet)
    {
        if (isBtagJet((*jet)->jet))
        {
            hardest_jet = &*(*jet);
            break;
        }

        const float jet_pt = pt(*(*jet)->corrected_p4);
        if (jet_pt > highest_pt)
            hardest_jet = &*(*jet);
    }

    return *hardest_jet->corrected_p4;
}

uint32_t BtagResonanceReconstructor::countBtags(const Iterators &jets) const
{
    uint32_t btagged_jets = 0;
    for(Iterators::const_iterator jet = jets.begin(); jets.end() != jet; ++jet)
    {
        if (isBtagJet((*jet)->jet))
            ++btagged_jets;
    }

    return btagged_jets;
}

bool BtagResonanceReconstructor::isBtagJet(const Jet *jet) const
{
    typedef ::google::protobuf::RepeatedPtrField<Jet::BTag> BTags;

    for(BTags::const_iterator btag = jet->btag().begin();
            jet->btag().end() != btag;
            ++btag)
    {
        if (Jet::BTag::SSVHE == btag->type())
        {
            return 1.74 < btag->discriminator();
        }
    }

    return false;
}



// -- Template Analyzer -------------------------------------------------------
//
TemplateAnalyzer::TemplateAnalyzer():
    _use_pileup(false),
    _wjets_input(false),
    _apply_wjet_correction(false)
{
    _synch_selector.reset(new SynchSelector());
    _synch_selector->tricut()->disable();
    monitor(_synch_selector);

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
