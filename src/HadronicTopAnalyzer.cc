// Study hadronic top
//
// Created by Samvel Khalatyan, Feb 09, 2011
// Copyright 2011, All rights reserved

#include <cfloat>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Jet.pb.h"
#include "bsm_input/interface/Input.pb.h"
#include "bsm_stat/interface/H1.h"
#include "bsm_stat/interface/H2.h"
#include "interface/Algorithm.h"
#include "interface/CorrectedJet.h"
#include "interface/Pileup.h"
#include "interface/GenResonance.h"
#include "interface/StatProxy.h"
#include "interface/HadronicTopAnalyzer.h"
#include "interface/Utility.h"

using namespace std;
using namespace boost;

using bsm::HadronicTopDelegate;
using bsm::HadronicTopOptions;
using bsm::HadronicTopAnalyzer;

// -- Hadronic Top options -----------------------------------------------------
//
HadronicTopOptions::HadronicTopOptions()
{
    _description.reset(new po::options_description("Hadronic Top Options"));
    _description->add_options()
        ("htop-jets",
         po::value<string>()->notifier(
             boost::bind(&HadronicTopOptions::setHtopNjets, this, _1)),
         "Use events with certain number of jets in reconstructed hadronic tops")
    ;
}

// Options interface
//
HadronicTopOptions::DescriptionPtr
    HadronicTopOptions::description() const
{
    return _description;
}

// Private
//
void HadronicTopOptions::setHtopNjets(const string &value)
{
    if (!delegate())
        return;
    
    smatch matches;
    if (regex_match(value, matches, regex("^(\\d+)..$")))
        delegate()->setHtopNjets(lexical_cast<uint32_t>(matches[1]));
    else if (regex_match(value, matches, regex("^..(\\d+)$")))
        delegate()->setHtopNjets(0, lexical_cast<uint32_t>(matches[1]));
    else if (regex_match(value, matches, regex("^(\\d+)..(\\d+)$")))
        delegate()->setHtopNjets(lexical_cast<uint32_t>(matches[1]),
                                 lexical_cast<uint32_t>(matches[2]));
    else
        cerr << "Didn't understand htop-njets: " << value << endl;
}



// -- HadronicTop Analyzer -------------------------------------------------------
//
HadronicTopAnalyzer::HadronicTopAnalyzer():
    _use_pileup(false)
{
    _htop_njets.min = 0;
    _htop_njets.max = 0;

    _synch_selector.reset(new SynchSelector());
    monitor(_synch_selector);

    _top.reset(new P4Monitor());

    _jet1.reset(new P4Monitor());
    _jet1->mass()->mutable_axis()->init(200, 0, 200);
    _jet1->mt()->mutable_axis()->init(200, 0, 200);

    _jet2.reset(new P4Monitor());
    _jet2->mass()->mutable_axis()->init(100, 0, 100);
    _jet2->mt()->mutable_axis()->init(100, 0, 100);

    _jet3.reset(new P4Monitor());
    _jet3->mass()->mutable_axis()->init(100, 0, 100);
    _jet3->mt()->mutable_axis()->init(100, 0, 100);

    _jet4.reset(new P4Monitor());
    _jet4->mass()->mutable_axis()->init(50, 0, 50);
    _jet4->mt()->mutable_axis()->init(50, 0, 50);

    monitor(_top);
    monitor(_jet1);
    monitor(_jet2);
    monitor(_jet3);
    monitor(_jet4);

    _jet1_parton.reset(new GenParticleMonitor());
    _jet1_parton->mass()->mutable_axis()->init(200, 0, 200);
    _jet1_parton->mt()->mutable_axis()->init(200, 0, 200);

    _jet2_parton.reset(new GenParticleMonitor());
    _jet2_parton->mass()->mutable_axis()->init(200, 0, 200);
    _jet2_parton->mt()->mutable_axis()->init(200, 0, 200);

    monitor(_jet1_parton);
    monitor(_jet2_parton);

    _jet1_parton_vs_jet2_parton.reset(new DeltaMonitor());
    _jet1_parton_vs_jet2_parton->ptrel()->mutable_axis()->init(100, 0, 100);
    _jet1_parton_vs_jet2_parton->ptrel_vs_r()->mutable_xAxis()->init(100, 0, 100);

    monitor(_jet1_parton_vs_jet2_parton);

    _jet1_vs_jet2.reset(new DeltaMonitor());
    _jet1_vs_jet2->ptrel()->mutable_axis()->init(100, 0, 100);
    _jet1_vs_jet2->ptrel_vs_r()->mutable_xAxis()->init(100, 0, 100);

    _jet1_vs_jet3.reset(new DeltaMonitor());
    _jet1_vs_jet4.reset(new DeltaMonitor());
    _jet2_vs_jet3.reset(new DeltaMonitor());
    _jet2_vs_jet4.reset(new DeltaMonitor());
    _jet3_vs_jet4.reset(new DeltaMonitor());

    monitor(_jet1_vs_jet2);
    monitor(_jet1_vs_jet3);
    monitor(_jet1_vs_jet4);
    monitor(_jet2_vs_jet3);
    monitor(_jet2_vs_jet4);
    monitor(_jet3_vs_jet4);

    _njets.reset(new H1Proxy(10, 0, 10));
    monitor(_njets);

    _njets_vs_mass.reset(new H2Proxy(10, 0, 10, 500, 0, 500));
    _pt_vs_mass.reset(new H2Proxy(500, 0, 500, 500, 0, 500));
    _njets_vs_pt.reset(new H2Proxy(10, 0, 10, 500, 0, 500));

    monitor(_njets_vs_mass);
    monitor(_pt_vs_mass);
    monitor(_njets_vs_pt);

    // Gen particles
    //
    _gen_top.reset(new GenParticleMonitor());

    _gen_jet1.reset(new GenParticleMonitor());
    _gen_jet1->mass()->mutable_axis()->init(200, 0, 200);
    _gen_jet1->mt()->mutable_axis()->init(200, 0, 200);

    _gen_jet2.reset(new GenParticleMonitor());
    _gen_jet2->mass()->mutable_axis()->init(100, 0, 100);
    _gen_jet2->mt()->mutable_axis()->init(100, 0, 100);

    _gen_jet3.reset(new GenParticleMonitor());
    _gen_jet3->mass()->mutable_axis()->init(100, 0, 100);
    _gen_jet3->mt()->mutable_axis()->init(100, 0, 100);

    monitor(_gen_top);
    monitor(_gen_jet1);
    monitor(_gen_jet2);
    monitor(_gen_jet3);

    _njets_gen.reset(new H1Proxy(10, 0, 10));
    monitor(_njets_gen);

    _njets_gen_vs_gen_mass.reset(new H2Proxy(10, 0, 10, 500, 0, 500));
    _pt_gen_vs_gen_mass.reset(new H2Proxy(500, 0, 500, 500, 0, 500));
    _njets_gen_vs_gen_pt.reset(new H2Proxy(10, 0, 10, 500, 0, 500));

    monitor(_njets_gen_vs_gen_mass);
    monitor(_pt_gen_vs_gen_mass);
    monitor(_njets_gen_vs_gen_pt);

    // TTbar system
    //
    _ttbar_reco.reset(new P4Monitor());
    _ttbar_reco->mass()->mutable_axis()->init(3000, 500, 3500);

    _ttbar_gen.reset(new P4Monitor());
    _ttbar_gen->mass()->mutable_axis()->init(3000, 500, 3500);

    monitor(_ttbar_reco);
    monitor(_ttbar_gen);

    _ttbar_reco_delta.reset(new DeltaMonitor());
    _ttbar_gen_delta.reset(new DeltaMonitor());

    monitor(_ttbar_reco_delta);
    monitor(_ttbar_gen_delta);

    _pileup.reset(new Pileup());
    monitor(_pileup);

    _reconstructor.reset(new SimpleResonanceReconstructor());
    monitor(_reconstructor);
}

HadronicTopAnalyzer::HadronicTopAnalyzer(const HadronicTopAnalyzer &object):
    _use_pileup(false),
    _htop_njets(object._htop_njets)
{
    _synch_selector = 
        dynamic_pointer_cast<SynchSelector>(object._synch_selector->clone());
    monitor(_synch_selector);

    _top = dynamic_pointer_cast<P4Monitor>(object._top->clone());
    monitor(_top);

    _jet1 = dynamic_pointer_cast<P4Monitor>(object._jet1->clone());
    monitor(_jet1);

    _jet2 = dynamic_pointer_cast<P4Monitor>(object._jet2->clone());
    monitor(_jet2);

    _jet3 = dynamic_pointer_cast<P4Monitor>(object._jet3->clone());
    monitor(_jet3);

    _jet4 = dynamic_pointer_cast<P4Monitor>(object._jet4->clone());
    monitor(_jet4);

    _jet1_parton = dynamic_pointer_cast<GenParticleMonitor>(object._jet1_parton->clone());
    monitor(_jet1_parton);

    _jet2_parton = dynamic_pointer_cast<GenParticleMonitor>(object._jet2_parton->clone());
    monitor(_jet2_parton);

    _jet1_parton_vs_jet2_parton = dynamic_pointer_cast<DeltaMonitor>(
            object._jet1_parton_vs_jet2_parton->clone());
    monitor(_jet1_parton_vs_jet2_parton);

    _jet1_vs_jet2 = dynamic_pointer_cast<DeltaMonitor>(object._jet1_vs_jet2->clone());
    monitor(_jet1_vs_jet2);

    _jet1_vs_jet3 = dynamic_pointer_cast<DeltaMonitor>(object._jet1_vs_jet3->clone());
    monitor(_jet1_vs_jet3);

    _jet1_vs_jet4 = dynamic_pointer_cast<DeltaMonitor>(object._jet1_vs_jet4->clone());
    monitor(_jet1_vs_jet4);

    _jet2_vs_jet3 = dynamic_pointer_cast<DeltaMonitor>(object._jet2_vs_jet3->clone());
    monitor(_jet2_vs_jet3);

    _jet2_vs_jet4 = dynamic_pointer_cast<DeltaMonitor>(object._jet2_vs_jet4->clone());
    monitor(_jet2_vs_jet4);

    _jet3_vs_jet4 = dynamic_pointer_cast<DeltaMonitor>(object._jet3_vs_jet4->clone());
    monitor(_jet3_vs_jet4);

    _njets = dynamic_pointer_cast<H1Proxy>(object._njets->clone());
    monitor(_njets);

    _njets_vs_mass = dynamic_pointer_cast<H2Proxy>(object._njets_vs_mass->clone());
    monitor(_njets_vs_mass);

    _pt_vs_mass = dynamic_pointer_cast<H2Proxy>(object._pt_vs_mass->clone());
    monitor(_pt_vs_mass);

    _njets_vs_pt = dynamic_pointer_cast<H2Proxy>(object._njets_vs_pt->clone());
    monitor(_njets_vs_pt);

    // Gen Particles
    //
    _gen_top = dynamic_pointer_cast<GenParticleMonitor>(object._gen_top->clone());
    monitor(_gen_top);

    _gen_jet1 = dynamic_pointer_cast<GenParticleMonitor>(object._gen_jet1->clone());
    monitor(_gen_jet1);

    _gen_jet2 = dynamic_pointer_cast<GenParticleMonitor>(object._gen_jet2->clone());
    monitor(_gen_jet2);

    _gen_jet3 = dynamic_pointer_cast<GenParticleMonitor>(object._gen_jet3->clone());
    monitor(_gen_jet3);

    _njets_gen = dynamic_pointer_cast<H1Proxy>(object._njets_gen->clone());
    monitor(_njets_gen);

    _njets_gen_vs_gen_mass = dynamic_pointer_cast<H2Proxy>(object._njets_gen_vs_gen_mass->clone());
    monitor(_njets_gen_vs_gen_mass);

    _pt_gen_vs_gen_mass = dynamic_pointer_cast<H2Proxy>(object._pt_gen_vs_gen_mass->clone());
    monitor(_pt_gen_vs_gen_mass);

    _njets_gen_vs_gen_pt = dynamic_pointer_cast<H2Proxy>(object._njets_gen_vs_gen_pt->clone());
    monitor(_njets_gen_vs_gen_pt);

    _ttbar_reco = dynamic_pointer_cast<P4Monitor>(object._ttbar_reco->clone());
    monitor(_ttbar_reco);

    _ttbar_gen = dynamic_pointer_cast<P4Monitor>(object._ttbar_gen->clone());
    monitor(_ttbar_gen);

    _ttbar_reco_delta = dynamic_pointer_cast<DeltaMonitor>(object._ttbar_reco_delta->clone());
    monitor(_ttbar_reco_delta);

    _ttbar_gen_delta = dynamic_pointer_cast<DeltaMonitor>(object._ttbar_gen_delta->clone());
    monitor(_ttbar_gen_delta);

    _pileup = dynamic_pointer_cast<Pileup>(object._pileup->clone());
    monitor(_pileup);

    _reconstructor = dynamic_pointer_cast<ResonanceReconstructor>(object._reconstructor->clone());
    monitor(_reconstructor);
}

const HadronicTopAnalyzer::P4MonitorPtr HadronicTopAnalyzer::top() const
{
    return _top;
}

const HadronicTopAnalyzer::P4MonitorPtr HadronicTopAnalyzer::jet1() const
{
    return _jet1;
}

const HadronicTopAnalyzer::P4MonitorPtr HadronicTopAnalyzer::jet2() const
{
    return _jet2;
}

const HadronicTopAnalyzer::P4MonitorPtr HadronicTopAnalyzer::jet3() const
{
    return _jet3;
}

const HadronicTopAnalyzer::P4MonitorPtr HadronicTopAnalyzer::jet4() const
{
    return _jet4;
}

const HadronicTopAnalyzer::GenParticleMonitorPtr HadronicTopAnalyzer::jet1_parton() const
{
    return _jet1_parton;
}

const HadronicTopAnalyzer::GenParticleMonitorPtr HadronicTopAnalyzer::jet2_parton() const
{
    return _jet2_parton;
}

const HadronicTopAnalyzer::DeltaMonitorPtr
    HadronicTopAnalyzer::jet1_parton_vs_jet2_parton() const
{
    return _jet1_parton_vs_jet2_parton;
}

const HadronicTopAnalyzer::DeltaMonitorPtr HadronicTopAnalyzer::jet1_vs_jet2() const
{
    return _jet1_vs_jet2;
}

const HadronicTopAnalyzer::DeltaMonitorPtr HadronicTopAnalyzer::jet1_vs_jet3() const
{
    return _jet1_vs_jet3;
}

const HadronicTopAnalyzer::DeltaMonitorPtr HadronicTopAnalyzer::jet1_vs_jet4() const
{
    return _jet1_vs_jet4;
}

const HadronicTopAnalyzer::DeltaMonitorPtr HadronicTopAnalyzer::jet2_vs_jet3() const
{
    return _jet2_vs_jet3;
}

const HadronicTopAnalyzer::DeltaMonitorPtr HadronicTopAnalyzer::jet2_vs_jet4() const
{
    return _jet2_vs_jet4;
}

const HadronicTopAnalyzer::DeltaMonitorPtr HadronicTopAnalyzer::jet3_vs_jet4() const
{
    return _jet3_vs_jet4;
}

const HadronicTopAnalyzer::H1Ptr HadronicTopAnalyzer::njets() const
{
    return _njets->histogram();
}

const HadronicTopAnalyzer::H2Ptr HadronicTopAnalyzer::njets_vs_mass() const
{
    return _njets_vs_mass->histogram();
}

const HadronicTopAnalyzer::H2Ptr HadronicTopAnalyzer::pt_vs_mass() const
{
    return _pt_vs_mass->histogram();
}

const HadronicTopAnalyzer::H2Ptr HadronicTopAnalyzer::njets_vs_pt() const
{
    return _njets_vs_pt->histogram();
}

const HadronicTopAnalyzer::GenParticleMonitorPtr HadronicTopAnalyzer::gen_top() const
{
    return _gen_top;
}

const HadronicTopAnalyzer::GenParticleMonitorPtr HadronicTopAnalyzer::gen_jet1() const
{
    return _gen_jet1;
}

const HadronicTopAnalyzer::GenParticleMonitorPtr HadronicTopAnalyzer::gen_jet2() const
{
    return _gen_jet2;
}

const HadronicTopAnalyzer::GenParticleMonitorPtr HadronicTopAnalyzer::gen_jet3() const
{
    return _gen_jet3;
}

const HadronicTopAnalyzer::H1Ptr HadronicTopAnalyzer::njets_gen() const
{
    return _njets_gen->histogram();
}

const HadronicTopAnalyzer::H2Ptr HadronicTopAnalyzer::njets_gen_vs_gen_mass() const
{
    return _njets_gen_vs_gen_mass->histogram();
}

const HadronicTopAnalyzer::H2Ptr HadronicTopAnalyzer::pt_gen_vs_gen_mass() const
{
    return _pt_gen_vs_gen_mass->histogram();
}

const HadronicTopAnalyzer::H2Ptr HadronicTopAnalyzer::njets_gen_vs_gen_pt() const
{
    return _njets_gen_vs_gen_pt->histogram();
}

const HadronicTopAnalyzer::P4MonitorPtr HadronicTopAnalyzer::ttbar_reco() const
{
    return _ttbar_reco;
}

const HadronicTopAnalyzer::P4MonitorPtr HadronicTopAnalyzer::ttbar_gen() const
{
    return _ttbar_gen;
}

const HadronicTopAnalyzer::DeltaMonitorPtr HadronicTopAnalyzer::ttbar_reco_delta() const
{
    return _ttbar_reco_delta;
}

const HadronicTopAnalyzer::DeltaMonitorPtr HadronicTopAnalyzer::ttbar_gen_delta() const
{
    return _ttbar_gen_delta;
}

bsm::JetEnergyCorrectionDelegate
    *HadronicTopAnalyzer::getJetEnergyCorrectionDelegate() const
{
    return _synch_selector.get();
}

bsm::SynchSelectorDelegate *HadronicTopAnalyzer::getSynchSelectorDelegate() const
{
    return _synch_selector.get();
}

bsm::PileupDelegate *HadronicTopAnalyzer::getPileupDelegate() const
{
    return _pileup.get();
}

bsm::TriggerDelegate *HadronicTopAnalyzer::getTriggerDelegate() const
{
    return _synch_selector.get();
}

void HadronicTopAnalyzer::setHtopNjets(const int &min, const int &max)
{
    _htop_njets.min = min;
    _htop_njets.max = max;
}

void HadronicTopAnalyzer::onFileOpen(const std::string &filename, const Input *input)
{
    if (input->has_type())
    {
        _use_pileup = (Input::DATA != input->type());
    }
    else
    {
        clog << "Input type is not available: pile-up correction is not applied"
            << endl;

        _use_pileup = false;
    }
}

void HadronicTopAnalyzer::process(const Event *event)
{
    if (!event->has_missing_energy())
        return;
    
    _pileup_weight = _use_pileup ? 0 : 1;

    if (_use_pileup)
        _pileup_weight = _pileup->scale(event);

    // Process only events, that pass the synch selector
    //
    if (_synch_selector->apply(event))
    {

        Mttbar resonance = mttbar();

        if (_synch_selector->reconstruction(resonance.valid)
               && _synch_selector->ltop(pt(resonance.ltop)))
        {
            if ((_htop_njets.min
                 && resonance.htop_jets.size() < _htop_njets.min)
                    || (_htop_njets.max
                        && resonance.htop_jets.size() >= _htop_njets.max))
                return;

            // Generator plots
            //
            typedef vector<TopQuark> TopQuarks;

            TopQuarks top_quarks = gen_resonance(event);
            if (2 != top_quarks.size())
                _log << "found " << top_quarks.size() << " tops" << endl;
            else
            {
                uint32_t wlep = 0;
                TopQuarks::const_iterator gen_htop = top_quarks.end();

                LorentzVector ttbar_gen_p4;
                for(TopQuarks::const_iterator top_quark = top_quarks.begin();
                        top_quarks.end() != top_quark;
                        ++top_quark)
                {
                    if (top_quark->wboson())
                    {
                        if (top_quark->wboson()->is_leptonic_decay())
                            ++wlep;
                        else
                            gen_htop = top_quark;
                    }

                    ttbar_gen_p4 += top_quark->gen_particle()->physics_object().p4();
                }

                if (1 != wlep)
                {
                    _log << "Found " << wlep
                        << " leptonically decaying W-bosons "
                        << (gen_htop == top_quarks.end() ? "without" : "with")
                        << " htop" << endl;
                }
                else if (gen_htop != top_quarks.end())
                {
                    gen_top()->fill(*gen_htop->gen_particle(), _pileup_weight);

                    const uint32_t njets_ =
                        gen_htop->gen_particle()->child().size() - 1
                            + gen_htop->wboson()->gen_particle()->child().size();;;;
                    const float pt_ = pt(gen_htop->gen_particle()->physics_object().p4());
                    const float mass_ = mass(gen_htop->gen_particle()->physics_object().p4());

                    njets_gen()->fill(njets_, _pileup_weight);
                    njets_gen_vs_gen_mass()->fill(njets_, mass_, _pileup_weight);
                    pt_gen_vs_gen_mass()->fill(pt_, mass_, _pileup_weight);
                    njets_gen_vs_gen_pt()->fill(njets_, pt_, _pileup_weight);
                }

                ttbar_gen()->fill(ttbar_gen_p4, _pileup_weight);
                ttbar_gen_delta()->fill(top_quarks[0].gen_particle()->physics_object().p4(),
                        top_quarks[1].gen_particle()->physics_object().p4(),
                        _pileup_weight);
            }

            // Reconstructed plots
            //
            const float mass_ = mass(resonance.htop);

            top()->fill(resonance.htop, _pileup_weight);

            const ResonanceReconstructor::CorrectedJets &htop_jets =
                resonance.htop_jets;

            const int njets_ = htop_jets.size();
            njets()->fill(njets_, _pileup_weight);

            const float pt_ = pt(resonance.htop);

            njets_vs_mass()->fill(njets_, mass_, _pileup_weight);
            pt_vs_mass()->fill(pt_, mass_, _pileup_weight);
            njets_vs_pt()->fill(njets_, pt_, _pileup_weight);

            ttbar_reco()->fill(resonance.htop + resonance.ltop, _pileup_weight);
            ttbar_reco_delta()->fill(resonance.htop, resonance.ltop, _pileup_weight);

            if (0 < njets_)
            {
                const LorentzVector &jet1_p4 = *htop_jets[0].corrected_p4;
                jet1()->fill(jet1_p4, _pileup_weight);

                const Jet *raw_jet1 = htop_jets[0].jet;
                if (raw_jet1->has_gen_parton())
                    jet1_parton()->fill(raw_jet1->gen_parton(), _pileup_weight);
            
                if (1 < njets_)
                {
                    const LorentzVector &jet2_p4 = *htop_jets[1].corrected_p4;
                    jet2()->fill(jet2_p4, _pileup_weight);

                    jet1_vs_jet2()->fill(jet1_p4, jet2_p4, _pileup_weight);

                    const Jet *raw_jet2 = htop_jets[1].jet;
                    if (raw_jet2->has_gen_parton())
                    {
                        jet2_parton()->fill(raw_jet2->gen_parton(), _pileup_weight);

                        if (raw_jet1->has_gen_parton())
                            jet1_parton_vs_jet2_parton()->fill(
                                    raw_jet1->gen_parton().physics_object().p4(),
                                    raw_jet2->gen_parton().physics_object().p4(),
                                    _pileup_weight);
                    }

                    if (2 < njets_)
                    {
                        const LorentzVector &jet3_p4 = *htop_jets[2].corrected_p4;
                        jet3()->fill(jet3_p4, _pileup_weight);

                        jet1_vs_jet3()->fill(jet1_p4, jet3_p4, _pileup_weight);
                        jet2_vs_jet3()->fill(jet2_p4, jet3_p4, _pileup_weight);

                        if (3 < njets_)
                        {
                            const LorentzVector &jet4_p4 = *htop_jets[3].corrected_p4;
                            jet4()->fill(jet4_p4, _pileup_weight);

                            jet1_vs_jet4()->fill(jet1_p4, jet4_p4, _pileup_weight);
                            jet2_vs_jet4()->fill(jet2_p4, jet4_p4, _pileup_weight);
                            jet3_vs_jet4()->fill(jet3_p4, jet4_p4, _pileup_weight);
                        }
                    }
                }
            }
        }
    }
}

// Object interface
//
uint32_t HadronicTopAnalyzer::id() const
{
    return core::ID<HadronicTopAnalyzer>::get();
}

HadronicTopAnalyzer::ObjectPtr HadronicTopAnalyzer::clone() const
{
    return ObjectPtr(new HadronicTopAnalyzer(*this));
}

void HadronicTopAnalyzer::merge(const ObjectPtr &pointer)
{
    if (pointer->id() != id())
        return;

    boost::shared_ptr<HadronicTopAnalyzer> object =
        dynamic_pointer_cast<HadronicTopAnalyzer>(pointer);

    if (!object)
        return;

    Object::merge(pointer);

    if (!object->_log.str().empty())
        _log << object->_log.str() << endl;
}

void HadronicTopAnalyzer::print(std::ostream &out) const
{
    out << *_synch_selector << endl;

    clog << _log.str() << endl;
}

// Private
//
HadronicTopAnalyzer::Mttbar HadronicTopAnalyzer::mttbar() const
{
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

