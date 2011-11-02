// Apply selectors and filter events
//
// Created by Samvel Khalatyan, May 20, 2011
// Copyright 2011, All rights reserved

#include <algorithm>
#include <iostream>
#include <ostream>
#include <sstream>

#include <boost/functional/hash.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/regex.hpp>
#include <boost/version.hpp>

#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Input.pb.h"
#include "bsm_input/interface/Trigger.pb.h"
#include "bsm_input/interface/Writer.h"
#include "bsm_input/interface/Utility.h"
#include "interface/Cut.h"
#include "interface/FilterAnalyzer.h"
#include "interface/SynchSelector.h"

using namespace std;
using namespace boost;

namespace fs = boost::filesystem;

using bsm::FilterAnalyzer;
using bsm::FilterOptions;

FilterOptions::FilterOptions()
{
    _delegate = 0;

    _description.reset(new po::options_description("Filter Options"));
    _description->add_options()
        ("event",
         po::value<Events>()->notifier(
             boost::bind(&FilterOptions::setEvents, this, _1)),
         "Event(s) to dump [repeatable]. Format: event[:lumi[:run]]")
    ;
}

void FilterOptions::setDelegate(FilterDelegate *delegate)
{
    if (_delegate != delegate)
        _delegate = delegate;
}

bsm::FilterDelegate *FilterOptions::delegate() const
{
    return _delegate;
}

// Options interface
//
FilterOptions::DescriptionPtr FilterOptions::description() const
{
    return _description;
}

// Private
//
void FilterOptions::setEvents(const Events &events)
{
    if (!delegate())
        return;

    shared_ptr<Event::Extra> event_id(new Event::Extra());
    for(Events::const_iterator event = events.begin();
            events.end() != event;
            ++event)
    {
        smatch matches;
        regex pattern("^(\\d+)(?::(\\d+)(?::(\\d+))?)?$");

        if (!regex_match(*event, matches, pattern))
        {
            cerr << "Didn't understand event: " << *event << endl;

            continue;
        }

        event_id->set_id(lexical_cast<uint32_t>(matches[1]));
        event_id->set_lumi(matches[2].matched
            ? lexical_cast<uint32_t>(matches[2])
            : 0);
        event_id->set_run(matches[3].matched
            ? lexical_cast<uint32_t>(matches[3])
            : 0);

        delegate()->setEventNumber(*event_id);

        event_id->Clear();
    }
}




FilterAnalyzer::FilterAnalyzer()
{
    _synch_selector.reset(new SynchSelector());
    _synch_selector->htlep()->disable();
    _synch_selector->leadingJet()->disable();
    _synch_selector->cut()->disable();
    monitor(_synch_selector);

    _input.reset(new Input());
}

FilterAnalyzer::FilterAnalyzer(const FilterAnalyzer &object):
    _events(object._events.begin(), object._events.end())
{
    _synch_selector = 
        dynamic_pointer_cast<SynchSelector>(object._synch_selector->clone());
    monitor(_synch_selector);

    _input.reset(new Input());
}

bsm::JetEnergyCorrectionDelegate *FilterAnalyzer::getJetEnergyCorrectionDelegate() const
{
    return _synch_selector.get();
}

bsm::SynchSelectorDelegate *FilterAnalyzer::getSynchSelectorDelegate() const
{
    return _synch_selector.get();
}

void FilterAnalyzer::onFileOpen(const string &filename, const Input *input)
{
    if (_writer)
    {
        _writer->close();
        _writer.reset();
    }

    fs::path path(filename);

    ostringstream file_name;

    boost::hash<string> make_hash;

    file_name << make_hash(filename);

#if BOOST_VERSION < 104600
    file_name << path.extension();
#else
    file_name << path.extension();
#endif

    _writer.reset(new Writer(file_name.str()));

    _input->Clear();
    _input->CopyFrom(*input);
    _input->set_events(0);
}

void FilterAnalyzer::process(const Event *event)
{
    if (!_writer)
        return;

    if (_events.empty())
    {
        if (!_synch_selector->apply(event))
            return;
    }
    else
    {
        EventSearcher predicate(event->extra());

        if (_events.end() == find_if(_events.begin(), _events.end(), predicate))
            return;
    }

    if (!_writer->isOpen())
    {
        _writer->open();
        if (!_writer->isOpen())
        {
            _writer.reset();
            return;
        }

        _writer->input()->CopyFrom(*_input);
    }

    _writer->write(event);
}

void FilterAnalyzer::fileWillClose(const Reader *)
{
    _writer->close();
}

void FilterAnalyzer::setEventNumber(const Event::Extra &event)
{
    if (_events.end() == find(_events.begin(), _events.end(), event))
        _events.push_back(event);
}

uint32_t FilterAnalyzer::id() const
{
    return core::ID<FilterAnalyzer>::get();
}

FilterAnalyzer::ObjectPtr FilterAnalyzer::clone() const
{
    return ObjectPtr(new FilterAnalyzer(*this));
}

void FilterAnalyzer::print(ostream &out) const
{
    out << *_synch_selector;
}
