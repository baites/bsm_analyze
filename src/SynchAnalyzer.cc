// Synchronization Exercises
//
// Different Analyzers that performan the sinchronization exercises at
// different development stages
//
// Created by Samvel Khalatyan, Jul 05, 2011
// Copyright 2011, All rights reserved

#include <iomanip>
#include <ostream>
#include <string>
#include <utility>

#include <boost/algorithm/string.hpp>
#include <boost/pointer_cast.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Utility.h"
#include "JetMETObjects/interface/FactorizedJetCorrector.h"
#include "interface/Monitor.h"
#include "interface/Selector.h"
#include "interface/SynchAnalyzer.h"

using namespace std;

using boost::dynamic_pointer_cast;
using boost::to_lower;

using bsm::SynchAnalyzer;
using bsm::SynchAnalyzerDelegate;
using bsm::SynchAnalyzerOptions;

// Synch Analyzer Options
//
SynchAnalyzerOptions::SynchAnalyzerOptions()
{
    _delegate = 0;

    _description.reset(new po::options_description("Synchronization Analyzer Options"));
    _description->add_options()
        ("selection",
         po::value<string>()->default_value("")->notifier(
             boost::bind(&SynchAnalyzerOptions::setSelection, this, _1)),
         "print events passing selection: htlep, leading_jet, cut_lepton, veto_second_lepton, lepton, good_jets, pv")
    ;
}

SynchAnalyzerOptions::~SynchAnalyzerOptions()
{
}

void SynchAnalyzerOptions::setDelegate(SynchAnalyzerDelegate *delegate)
{
    if (_delegate != delegate)
        _delegate = delegate;
}

SynchAnalyzerDelegate *SynchAnalyzerOptions::delegate() const
{
    return _delegate;
}

SynchAnalyzerOptions::DescriptionPtr SynchAnalyzerOptions::description() const
{
    return _description;
}

// Private
//
void SynchAnalyzerOptions::setSelection(std::string selection)
{
    if (!_delegate
            || selection.empty())
        return;

    to_lower(selection);

    if ("htlep" == selection)
        _delegate->setSelection(SynchSelector::HTLEP);
    else if ("leading_jet" == selection)
        _delegate->setSelection(SynchSelector::LEADING_JET);
    else if ("cut_lepton" == selection)
        _delegate->setSelection(SynchSelector::CUT_LEPTON);
    else if ("veto_second_lepton" == selection)
        _delegate->setSelection(SynchSelector::VETO_SECOND_LEPTON);
    else if ("lepton" == selection)
        _delegate->setSelection(SynchSelector::LEPTON);
    else if ("good_jets" == selection)
        _delegate->setSelection(SynchSelector::JET);
    else if ("pv" == selection)
        _delegate->setSelection(SynchSelector::PRIMARY_VERTEX);
    else
        cerr << "didn't understand selection: " << selection << endl;
}



// Synch Analyzer
//
SynchAnalyzer::SynchAnalyzer():
    _selection(SynchSelector::SELECTIONS)
{
    _synch_selector.reset(new SynchSelector());
    monitor(_synch_selector);

    _format.reset(new ShortFormat());
    _dump_format.reset(new MediumFormat());

    _event = 0;
}

SynchAnalyzer::SynchAnalyzer(const SynchAnalyzer &object):
    _selection(SynchSelector::SELECTIONS),
    _events_to_dump(object._events_to_dump.begin(), object._events_to_dump.end())
{
    _synch_selector = 
        dynamic_pointer_cast<SynchSelector>(object._synch_selector->clone());
    monitor(_synch_selector);

    _format.reset(new ShortFormat());
    _dump_format.reset(new MediumFormat());

    _event = 0;

    setSelection(object._selection);
}

SynchAnalyzer::~SynchAnalyzer()
{
}

bsm::JetEnergyCorrectionDelegate *SynchAnalyzer::getJetEnergyCorrectionDelegate() const
{
    return _synch_selector->getJetEnergyCorrectionDelegate();
}

bsm::SynchSelectorDelegate *SynchAnalyzer::getSynchSelectorDelegate() const
{
    return _synch_selector.get();
}

bsm::Cut2DSelectorDelegate *SynchAnalyzer::getCut2DSelectorDelegate() const
{
    return _synch_selector->getCut2DSelectorDelegate();
}

void SynchAnalyzer::setSelection(const SynchSelector::Selection &selection)
{
    if (SynchSelector::SELECTIONS != _selection)
        _synch_selector->cutflow()->cut(_selection)->objects()->setDelegate(0);

    _selection = selection;

    if (SynchSelector::SELECTIONS != _selection)
        _synch_selector->cutflow()->cut(_selection)->objects()->setDelegate(this);
}

void SynchAnalyzer::didCounterAdd(const Counter *)
{
    if (_event)
        _out << _format->operator()(*_event) << endl;
}

void SynchAnalyzer::setEventNumber(const Event::Extra &event)
{
    EventSearcher predicate(event);
    if (_events_to_dump.end() == find_if(_events_to_dump.begin(),
                _events_to_dump.end(),
                predicate))

        _events_to_dump.push_back(event);
}

void SynchAnalyzer::onFileOpen(const std::string &filename, const Input *)
{
}

void SynchAnalyzer::process(const Event *event)
{
    _event = event;

    _synch_selector->apply(event);

    if (!_events_to_dump.empty())
    {
        EventSearcher predicate(event->extra());
        if (_events_to_dump.end() != find_if(_events_to_dump.begin(),
                    _events_to_dump.end(),
                    predicate))    
            dump(event);
    }

    _event = 0;
}

uint32_t SynchAnalyzer::id() const
{
    return core::ID<SynchAnalyzer>::get();
}

SynchAnalyzer::ObjectPtr SynchAnalyzer::clone() const
{
    return ObjectPtr(new SynchAnalyzer(*this));
}

void SynchAnalyzer::merge(const ObjectPtr &pointer)
{
    if (pointer->id() != id())
        return;

    boost::shared_ptr<SynchAnalyzer> object =
        dynamic_pointer_cast<SynchAnalyzer>(pointer);

    if (!object)
        return;

    Object::merge(pointer);

    if (!object->_out.str().empty())
        _out << object->_out.str() << endl;
}

void SynchAnalyzer::print(std::ostream &out) const
{
    out << *_synch_selector << endl;

    clog << _out.str() << endl;
}

// Private
//
void SynchAnalyzer::dump(const Event *event)
{
    const Format &format = *_dump_format;

    _out << endl;
    _out << format(*event) << endl;

    _out << setw(20) << setfill('-') << " " << endl;
    _out << "Selected Objects" << endl;
    _out << setw(20) << setfill('-') << " " << endl;
    _out << _synch_selector->goodMuons().size() << " good muons" << endl;
    for(SynchSelector::GoodMuons::const_iterator muon =
            _synch_selector->goodMuons().begin();
            _synch_selector->goodMuons().end() != muon;
            ++muon)
    {
        _out << format(*(*muon)) << endl;
        _out << "---" << endl;
    }

    _out << endl;
    _out << _synch_selector->goodElectrons().size() << " good electrons" << endl;
    for(SynchSelector::GoodElectrons::const_iterator electron =
            _synch_selector->goodElectrons().begin();
            _synch_selector->goodElectrons().end() != electron;
            ++electron)
    {
        _out << format(*(*electron)) << endl;
        _out << "---" << endl;
    }

    _out << endl;
    _out << _synch_selector->niceJets().size() << " nice jets" << endl;
    for(SynchSelector::GoodJets::const_iterator jet =
            _synch_selector->niceJets().begin();
            _synch_selector->niceJets().end() != jet;
            ++jet)
    {
        _out << "corr p4: " << *jet->corrected_p4 << endl;
        _out << format(*jet->jet) << endl;
        _out << "---" << endl;
    }

    _out << endl;
    SynchSelector::GoodJets::const_iterator closest_jet =
        _synch_selector->closestJet();
    if (_synch_selector->niceJets().end() == closest_jet)
        _out << "closest jet not available" << endl;
    else
    {
        _out << "closest jet" << endl;
        _out << "corr p4: " << *closest_jet->corrected_p4 << endl;
        _out << format(*closest_jet->jet) << endl;

        const LorentzVector *lepton_p4 =
            SynchSelector::ELECTRON == _synch_selector->leptonMode()
            ? &(*_synch_selector->goodElectrons().begin())->physics_object().p4()
            : &(*_synch_selector->goodMuons().begin())->physics_object().p4();

        _out << "ptrel: " << ptrel(*lepton_p4, *closest_jet->corrected_p4)
            << " dr: " << dr(*lepton_p4, *closest_jet->corrected_p4);
    }
}



// Helpers
//
std::ostream &bsm::operator <<(std::ostream &out, const SynchMode &mode)
{
    switch(mode)
    {
        case ELECTRON:   out << "electron";
                         break;

        case MUON:       out << "muon";
                         break;

        default:         out << "unknown";
                         break;
    }

    return out;
}
