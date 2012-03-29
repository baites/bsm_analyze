// Apply SynchSelector, use events with matched reconstructed jets with
// gen partons.
//
// Created by Samvel Khalatyan, Mar 28, 2012
// Copyright 2011, All rights reserved

#include <cfloat>
#include <iostream>

#include <boost/regex.hpp>

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
#include "interface/CorrectedJet.h"
#include "interface/Cut.h"
#include "interface/Monitor.h"
#include "interface/StatProxy.h"
#include "interface/GenMatchingAnalyzer.h"
#include "interface/Utility.h"

using namespace std;
using namespace boost;
using namespace bsm;

// -- GenMatching Analyzer -------------------------------------------------------
//
GenMatchingAnalyzer::GenMatchingAnalyzer()
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

    _cutflow.reset(new H1Proxy(SynchSelector::SELECTIONS, 0,
                SynchSelector::SELECTIONS));
    monitor(_cutflow);

    _ttbar.reset(new P4Monitor());
    _ttbar->mass()->mutable_axis()->init(4000, 0, 4);
    monitor(_ttbar);

    _ltop_drsum.reset(new H1Proxy(50, 0, 5));
    monitor(_ltop_drsum);

    _htop_drsum.reset(new H1Proxy(50, 0, 5));
    monitor(_htop_drsum);

    _htop_dphi.reset(new H1Proxy(80, -4, 4));
    monitor(_htop_dphi);

    _ltop.reset(new P4Monitor());
    _ltop->mt()->mutable_axis()->init(1000, 0, 1000);
    _ltop->pt()->mutable_axis()->init(1000, 0, 1000);
    monitor(_ltop);

    _htop.reset(new P4Monitor());
    _htop->mt()->mutable_axis()->init(1000, 0, 1000);
    _htop->pt()->mutable_axis()->init(1000, 0, 1000);
    monitor(_htop);
}

GenMatchingAnalyzer::GenMatchingAnalyzer(const GenMatchingAnalyzer &object)
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

    _cutflow = dynamic_pointer_cast<H1Proxy>(object._cutflow->clone());
    monitor(_cutflow);

    _ttbar = dynamic_pointer_cast<P4Monitor>(object._ttbar->clone());
    monitor(_ttbar);

    _ltop_drsum = dynamic_pointer_cast<H1Proxy>(
            object._ltop_drsum->clone());
    monitor(_ltop_drsum);

    _htop_drsum = dynamic_pointer_cast<H1Proxy>(
            object._htop_drsum->clone());
    monitor(_htop_drsum);

    _htop_dphi = dynamic_pointer_cast<H1Proxy>(
            object._htop_dphi->clone());
    monitor(_htop_dphi);

    _ltop =
        dynamic_pointer_cast<P4Monitor>(object._ltop->clone());
    monitor(_ltop);

    _htop =
        dynamic_pointer_cast<P4Monitor>(object._htop->clone());
    monitor(_htop);
}

const GenMatchingAnalyzer::H1Ptr GenMatchingAnalyzer::cutflow() const
{
    return _cutflow->histogram();
}

const GenMatchingAnalyzer::P4MonitorPtr GenMatchingAnalyzer::ttbar() const
{
    return _ttbar;
}

const GenMatchingAnalyzer::H1Ptr GenMatchingAnalyzer::ltop_drsum() const
{
    return _ltop_drsum->histogram();
}

const GenMatchingAnalyzer::H1Ptr GenMatchingAnalyzer::htop_drsum() const
{
    return _htop_drsum->histogram();
}

const GenMatchingAnalyzer::H1Ptr GenMatchingAnalyzer::htop_dphi() const
{
    return _htop_dphi->histogram();
}

const GenMatchingAnalyzer::P4MonitorPtr GenMatchingAnalyzer::ltop() const
{
    return _ltop;
}

const GenMatchingAnalyzer::P4MonitorPtr GenMatchingAnalyzer::htop() const
{
    return _htop;
}

bsm::JetEnergyCorrectionDelegate
    *GenMatchingAnalyzer::getJetEnergyCorrectionDelegate() const
{
    return _synch_selector.get();
}

bsm::SynchSelectorDelegate *GenMatchingAnalyzer::getSynchSelectorDelegate() const
{
    return _synch_selector.get();
}

bsm::TriggerDelegate *GenMatchingAnalyzer::getTriggerDelegate() const
{
    return _synch_selector.get();
}

void GenMatchingAnalyzer::didCounterAdd(const Counter *counter)
{
    if (_counters.end() != _counters.find(counter))
        cutflow()->fill(_counters[counter]);
}

void GenMatchingAnalyzer::onFileOpen(const std::string &filename, const Input *input)
{
}

void GenMatchingAnalyzer::process(const Event *event)
{
    if (!event->has_missing_energy())
        return;

    // Process only events, that pass the synch selector
    //
    if (_synch_selector->apply(event))
    {
        gen::TTbar resonance = gen_decay(event);

        // Prepare collection of corrected jets
        //
        gen::CorrectedJets corrected_jets;
        for(SynchSelector::GoodJets::const_iterator good_jet =
                    _synch_selector->goodJets().begin();
                _synch_selector->goodJets().end() != good_jet;
                ++good_jet)
        {
            corrected_jets.push_back(&*good_jet);
        }

        if (_synch_selector->reconstruction(
                    resonance.match(corrected_jets) &&
                    gen::Wboson::ELECTRON == resonance.ltop.wboson.decay))
        {
            const LorentzVector &el_p4 = _synch_selector->goodElectrons()[0]->physics_object().p4();
            const LorentzVector &nu_p4 = *_synch_selector->goodMET();

            LorentzVector ltop_p4 = el_p4;
            ltop_p4 += nu_p4;
            for(vector<gen::MatchedJet>::const_iterator matched_jet =
                        resonance.ltop.jets.begin();
                    resonance.ltop.jets.end() != matched_jet;
                    ++matched_jet)
            {
                ltop_p4 += *matched_jet->jet->corrected_p4;
            }

            LorentzVector htop_p4;
            for(vector<gen::MatchedJet>::const_iterator matched_jet =
                        resonance.htop.jets.begin();
                    resonance.htop.jets.end() != matched_jet;
                    ++matched_jet)
            {
                htop_p4 += *matched_jet->jet->corrected_p4;
            }

            ltop_drsum()->fill(dr(ltop_p4, el_p4) +
                               dr(ltop_p4, nu_p4) +
                               dr(ltop_p4, *resonance.ltop.jets.begin()->jet->corrected_p4));

            float drsum = 0;
            for(vector<gen::MatchedJet>::const_iterator matched_jet =
                        resonance.htop.jets.begin();
                    resonance.htop.jets.end() != matched_jet;
                    ++matched_jet)
            {
                drsum += dr(htop_p4, *matched_jet->jet->corrected_p4);
            }

            htop_drsum()->fill(drsum);
            htop_dphi()->fill(dphi(htop_p4, ltop_p4));

            ltop()->fill(ltop_p4);
            htop()->fill(htop_p4);

            ttbar()->fill(htop_p4 + ltop_p4);
        }
    }
}

// Object interface
//
uint32_t GenMatchingAnalyzer::id() const
{
    return core::ID<GenMatchingAnalyzer>::get();
}

GenMatchingAnalyzer::ObjectPtr GenMatchingAnalyzer::clone() const
{
    return ObjectPtr(new GenMatchingAnalyzer(*this));
}

void GenMatchingAnalyzer::merge(const ObjectPtr &pointer)
{
    if (pointer->id() != id())
        return;

    boost::shared_ptr<GenMatchingAnalyzer> object =
        dynamic_pointer_cast<GenMatchingAnalyzer>(pointer);

    if (!object)
        return;

    // Reset counter delegates
    //
    for(uint32_t cut = 0; SynchSelector::SELECTIONS < cut; ++cut)
        _synch_selector->cutflow()->cut(cut)->events().get()->setDelegate(0);

    Object::merge(pointer);
}

void GenMatchingAnalyzer::print(std::ostream &out) const
{
    out << *_synch_selector << endl;
}

// Private
//
gen::TTbar GenMatchingAnalyzer::gen_decay(const Event *event) const
{
    gen::TTbar ttbar;

    typedef ::google::protobuf::RepeatedPtrField<GenParticle> GenParticles;

    const GenParticles &particles = event->gen_particle();
    for(GenParticles::const_iterator particle = particles.begin();
            particles.end() != particle;
            ++particle)
    {
        if (3 != particle->status())
            continue;

        // Skip everything but t-quark
        //
        if (5 == abs(particle->id()))
            ttbar.fill(*particle);
    }

    return ttbar;
}

void gen::TTbar::fill(const GenParticle &particle)
{
    Top top;

    typedef ::google::protobuf::RepeatedPtrField<GenParticle> GenParticles;

    const GenParticles &particles = particle.child();
    for(GenParticles::const_iterator child = particles.begin();
            particles.end() != child;
            ++child)
    {
        if (3 != child->status())
            continue;

        top.fill(*child);
    }

    switch(top.wboson.decay)
    {
        case Wboson::ELECTRON: // fall through
        case Wboson::MUON: // fall through
        case Wboson::TAU:
            ltop = top;
            break;

        case Wboson::HADRONIC:
            htop = top;
            break;

        default:
            // something went wrong
            //
            throw runtime_error("unknown W-boson decay");
            break;
    }
}

bool gen::TTbar::match(CorrectedJets &corrected_jets)
{
    return ltop.match(corrected_jets) && htop.match(corrected_jets);
}

void gen::Top::fill(const GenParticle &particle)
{
    typedef ::google::protobuf::RepeatedPtrField<GenParticle> GenParticles;

    const GenParticles &particles = particle.child();
    for(GenParticles::const_iterator child = particles.begin();
            particles.end() != child;
            ++child)
    {
        if (3 != child->status())
            continue;

        if (24 == abs(child->id()))
            wboson.fill(*child);
        else
        {
            MatchedJet jet;
            jet.parton = &*child;

            jets.push_back(jet);
        }
    }

    // perform t-quark validity
    //
    switch(wboson.decay)
    {
        case Wboson::ELECTRON: // fall through
        case Wboson::MUON: // fall through
        case Wboson::TAU:
            if (jets.empty())
                throw runtime_error("missing b-quark in ltop");

            if (1 < jets.size())
                throw runtime_error("too many partons are found in ltop");

            break;

        case Wboson::HADRONIC:
            if (jets.empty())
                throw runtime_error("missing b-quark in htop");

            break;

        default:
            throw runtime_error("failed to detect top decay type");

            break;
    }
}

bool gen::Top::match(CorrectedJets &corrected_jets)
{
    // Skip leptonic decays
    //
    if (jets.empty())
        return true;

    for(vector<MatchedJet>::iterator matched_jet = jets.begin();
            jets.end() != matched_jet;
            ++matched_jet)
    {
        if (!matched_jet->match(corrected_jets))
            return false;
    }

    return true;
}

void gen::Wboson::fill(const GenParticle &particle)
{
    typedef ::google::protobuf::RepeatedPtrField<GenParticle> GenParticles;

    const GenParticles &particles = particle.child();
    for(GenParticles::const_iterator child = particles.begin();
            particles.end() != child;
            ++child)
    {
        // Skip all unstable particles
        //
        if (3 != child->status())
            continue;

        switch(abs(child->id()))
        {
            case 11: // Electron
                decay = ELECTRON;
                lepton = &*child;
                break;

            case 12: // Ele-neutrino
                decay = ELECTRON;
                neutrino = &*child;
                break;

            case 13: // Muon
                decay = MUON;
                lepton = &*child;
                break;

            case 14: // Mu-neutrino
                decay = MUON;
                neutrino = &*child;
                break;

            case 15: // Tau
                decay = TAU;
                lepton = &*child;
                break;

            case 16: // Tau-neutrino
                decay = TAU;
                neutrino = &*child;
                break;

            default: // hadronic decay
                decay = HADRONIC;

                MatchedJet jet;
                jet.parton = &*child;

                jets.push_back(jet);
                break;
        }
    }

    // check w-boson validity
    //
    switch(decay)
    {
        case ELECTRON: // fall through
        case MUON: // fall through
        case TAU:
            if (!lepton || !neutrino || !jets.empty())
                throw runtime_error("wrong w-boson leptonic decay");

            break;

        case HADRONIC:
            if (lepton || neutrino || jets.empty())
                throw runtime_error("wrong w-boson hadronic decay");

            break;

        default:
            if (lepton || neutrino || !jets.empty())
                throw runtime_error("wrong w-boson unknown decay");

            break;
    }
}

bool gen::Wboson::match(CorrectedJets &corrected_jets)
{
    // Skip leptonic decays
    //
    if (jets.empty())
        return true;

    for(vector<MatchedJet>::iterator matched_jet = jets.begin();
            jets.end() != matched_jet;
            ++matched_jet)
    {
        if (!matched_jet->match(corrected_jets))
            return false;
    }

    return true;
}

bool gen::MatchedJet::match(CorrectedJets &corrected_jets)
{
    if (!parton)
        return false;

    if (jet)
        return true;

    for(CorrectedJets::iterator corrected_jet = corrected_jets.begin();
            corrected_jets.end() != corrected_jet;
            ++corrected_jet)
    {
        if (0.3 > dr(parton->physics_object().p4(), *(*corrected_jet)->corrected_p4))
        {
            jet = *corrected_jet;

            corrected_jets.erase(corrected_jet);
            
            return true;
        }
    }

    return false;
}
