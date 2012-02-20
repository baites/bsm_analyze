// Dump resonance information
//
// Created by Samvel Khalatyan, Feb 17, 2012
// Copyright 2012, All rights reserved

#include <cfloat>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Jet.pb.h"
#include "bsm_input/interface/Input.pb.h"
#include "bsm_input/interface/Utility.h"
#include "bsm_stat/interface/H1.h"
#include "bsm_stat/interface/H2.h"
#include "interface/Algorithm.h"
#include "interface/CorrectedJet.h"
#include "interface/Pileup.h"
#include "interface/GenResonance.h"
#include "interface/StatProxy.h"
#include "interface/ResonanceDumpAnalyzer.h"
#include "interface/Utility.h"

using namespace std;
using namespace boost;

using bsm::ResonanceDumpDelegate;
using bsm::ResonanceDumpOptions;
using bsm::ResonanceDumpAnalyzer;

// -- Resonance Dump options ---------------------------------------------------
//
ResonanceDumpOptions::ResonanceDumpOptions()
{
    description()->add_options()
        ("events",
         po::value<uint32_t>()->notifier(
             boost::bind(&ResonanceDumpOptions::setEvents, this, _1)),
         "Dump first N reconstructed events")
    ;
}


// Private
//
void ResonanceDumpOptions::setEvents(const uint32_t &value)
{
    if (!delegate())
        return;

    delegate<ResonanceDumpDelegate>()->setEvents(value);
}



// -- ResonanceDump Analyzer -------------------------------------------------------
//
ResonanceDumpAnalyzer::ResonanceDumpAnalyzer():
    _max_events(0),
    _dumped_events(0)
{
    _htop_njets.min = 0;
    _htop_njets.max = 0;

    _synch_selector.reset(new SynchSelector());
    monitor(_synch_selector);

    _reconstructor.reset(new SimpleResonanceReconstructor());
    monitor(_reconstructor);

    setFormatLevel(SHORT);
}

ResonanceDumpAnalyzer::ResonanceDumpAnalyzer(const ResonanceDumpAnalyzer &object):
    _htop_njets(object._htop_njets),
    _max_events(object._max_events)
{
    _synch_selector = 
        dynamic_pointer_cast<SynchSelector>(object._synch_selector->clone());
    monitor(_synch_selector);

    _reconstructor =
        dynamic_pointer_cast<ResonanceReconstructor>(
                object._reconstructor->clone());
    monitor(_reconstructor);

    setFormatLevel(object._format_level);
}

bsm::JetEnergyCorrectionDelegate
    *ResonanceDumpAnalyzer::getJetEnergyCorrectionDelegate() const
{
    return _synch_selector.get();
}

bsm::SynchSelectorDelegate *ResonanceDumpAnalyzer::getSynchSelectorDelegate() const
{
    return _synch_selector.get();
}

bsm::TriggerDelegate *ResonanceDumpAnalyzer::getTriggerDelegate() const
{
    return _synch_selector.get();
}

void ResonanceDumpAnalyzer::setHtopNjets(const int &min, const int &max)
{
    _htop_njets.min = min;
    _htop_njets.max = max;
}

void ResonanceDumpAnalyzer::setEvents(const uint32_t &max_events)
{
    _max_events = max_events;
}

void ResonanceDumpAnalyzer::setFormatLevel(const Level &level)
{
    _format_level = level;

    if (FULL == _format_level)
        _format.reset(new FullFormat());
    else if(MEDIUM == _format_level)
        _format.reset(new MediumFormat());
    else
        _format.reset(new ShortFormat());
}

void ResonanceDumpAnalyzer::process(const Event *event)
{
    if (_dumped_events > _max_events)
        return;

    if (!event->has_missing_energy())
        return;
    
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

            const uint32_t width = 15;

            _log << "-- Good Jets ----" << endl;
            for(SynchSelector::GoodJets::const_iterator jet =
                        _synch_selector->goodJets().begin();
                    jet != _synch_selector->goodJets().end();
                    ++jet)
            {
                _log << setw(width) << right << " "
                    << *jet->corrected_p4 << endl;
            }
            _log << "-- Reconstructed Resonance ----" << endl;
            _log << setw(width) << right << "lepton: " << 
                    (SynchSelector::ELECTRON == _synch_selector->leptonMode()
                    ? (*_synch_selector->goodElectrons().begin())->physics_object().p4()
                    : (*_synch_selector->goodMuons().begin())->physics_object().p4())
                << endl;

            _log << setw(width) << right << "met: " << resonance.neutrino << endl;

            for(ResonanceReconstructor::LorentzVectors::const_iterator p4 =
                        resonance.neutrinos.begin();
                    resonance.neutrinos.end() != p4;
                    ++p4)
            {
                _log << setw(width) << right;
                if (p4 != resonance.neutrinos.begin())
                    _log<< " ";
                else
                    _log << "met(s): ";

                _log << *p4 << endl;
            }

            for(ResonanceReconstructor::CorrectedJets::const_iterator jet =
                        resonance.ltop_jets.begin();
                    resonance.ltop_jets.end() != jet;
                    ++jet)
            {
                _log << setw(width) << right;
                if (jet != resonance.ltop_jets.begin())
                    _log<< " ";
                else
                    _log << "ltop jets: ";

                _log << *jet->corrected_p4 << endl;
            }

            for(ResonanceReconstructor::CorrectedJets::const_iterator jet =
                        resonance.htop_jets.begin();
                    resonance.htop_jets.end() != jet;
                    ++jet)
            {
                _log << setw(width) << right;
                if (jet != resonance.htop_jets.begin())
                    _log<< " ";
                else
                    _log << "htop jets: ";

                _log << *jet->corrected_p4 << endl;
            }
            _log << endl;

            ++_dumped_events;
        }
    }
}

// Object interface
//
uint32_t ResonanceDumpAnalyzer::id() const
{
    return core::ID<ResonanceDumpAnalyzer>::get();
}

ResonanceDumpAnalyzer::ObjectPtr ResonanceDumpAnalyzer::clone() const
{
    return ObjectPtr(new ResonanceDumpAnalyzer(*this));
}

void ResonanceDumpAnalyzer::merge(const ObjectPtr &pointer)
{
    if (pointer->id() != id())
        return;

    boost::shared_ptr<ResonanceDumpAnalyzer> object =
        dynamic_pointer_cast<ResonanceDumpAnalyzer>(pointer);

    if (!object)
        return;

    Object::merge(pointer);

    if (!object->_log.str().empty())
        _log << object->_log.str() << endl;
}

void ResonanceDumpAnalyzer::print(std::ostream &out) const
{
    out << *_synch_selector << endl;

    clog << _log.str() << endl;
}

// Private
//
ResonanceDumpAnalyzer::Mttbar ResonanceDumpAnalyzer::mttbar() const
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
