// Dumpt event content
//
// Created by Samvel Khalatyan, Aug 04, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <ostream>

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>

#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Electron.pb.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Jet.pb.h"
#include "bsm_input/interface/Muon.pb.h"
#include "bsm_input/interface/Physics.pb.h"
#include "bsm_input/interface/PrimaryVertex.pb.h"
#include "bsm_input/interface/Utility.h"
#include "interface/EventDump.h"
#include "interface/Selector.h"

using namespace std;

using boost::dynamic_pointer_cast;
using boost::lexical_cast;
using boost::regex;
using boost::shared_ptr;
using boost::smatch;
using boost::to_lower;

using bsm::EventDumpAnalyzer;
using bsm::EventDumpDelegate;
using bsm::EventDumpOptions;

EventDumpOptions::EventDumpOptions()
{
    _delegate = 0;

    _description.reset(new po::options_description("Event Dump Options"));
    _description->add_options()
        ("event",
         po::value<Events>()->notifier(
             boost::bind(&EventDumpOptions::setEvents, this, _1)),
         "Event(s) to dump [repeatable]. Format: event[:lumi[:run]]")

        ("format",
         po::value<string>()->notifier(
             boost::bind(&EventDumpOptions::setFormatLevel, this, _1)),
         "Level of the event print content")
    ;
}

EventDumpOptions::~EventDumpOptions()
{
}

void EventDumpOptions::setDelegate(EventDumpDelegate *delegate)
{
    if (_delegate != delegate)
        _delegate = delegate;
}

EventDumpDelegate *EventDumpOptions::delegate() const
{
    return _delegate;
}

// Options interface
//
EventDumpOptions::DescriptionPtr EventDumpOptions::description() const
{
    return _description;
}

// Private
//
void EventDumpOptions::setEvents(const Events &events)
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

void EventDumpOptions::setFormatLevel(std::string level)
{
    if (!_delegate
            || level.empty())
        return;

    to_lower(level);

    if ("short" == level)
        _delegate->setFormatLevel(EventDumpDelegate::SHORT);
    else if ("medium" == level)
        _delegate->setFormatLevel(EventDumpDelegate::MEDIUM);
    else if ("full" == level)
        _delegate->setFormatLevel(EventDumpDelegate::FULL);
    else
        cerr << "didn't understand dump level: " << level << endl;
}



// Event Dump Analyzer
//
EventDumpAnalyzer::EventDumpAnalyzer()
{
    setFormatLevel(SHORT);
}

EventDumpAnalyzer::EventDumpAnalyzer(const EventDumpAnalyzer &object):
    _events(object._events.begin(), object._events.end())
{
    setFormatLevel(object._format_level);
}

void EventDumpAnalyzer::setEventNumber(const Event::Extra &event)
{
    if (_events.end() == find(_events.begin(), _events.end(), event))
        _events.push_back(event);
}

void EventDumpAnalyzer::setFormatLevel(const Level &level)
{
    _format_level = level;

    if (FULL == _format_level)
        _format.reset(new FullFormat());
    else if(MEDIUM == _format_level)
        _format.reset(new MediumFormat());
    else
        _format.reset(new ShortFormat());
}

void EventDumpAnalyzer::onFileOpen(const std::string &filename, const Input *)
{
}

void EventDumpAnalyzer::process(const Event *event)
{
    EventSearcher predicate(event->extra());

    if (_events.empty()
            || _events.end() != find_if(_events.begin(), _events.end(), predicate))    
        _out << (*_format)(*event) << endl;
}

uint32_t EventDumpAnalyzer::id() const
{
    return core::ID<EventDumpAnalyzer>::get();
}

EventDumpAnalyzer::ObjectPtr EventDumpAnalyzer::clone() const
{
    return ObjectPtr(new EventDumpAnalyzer(*this));
}

void EventDumpAnalyzer::merge(const ObjectPtr &pointer)
{
    if (id() != pointer->id());

    boost::shared_ptr<EventDumpAnalyzer> object =
        dynamic_pointer_cast<EventDumpAnalyzer>(pointer);

    if (!object)
        return;

    Object::merge(pointer);

    _out << endl;
    _out << object->_out.str();
}

void EventDumpAnalyzer::print(std::ostream &out) const
{
    out << _out.str();
}
