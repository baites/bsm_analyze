// Dumpt event content
//
// Created by Samvel Khalatyan, Aug 04, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <ostream>

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

using bsm::EventDumpAnalyzer;
using bsm::EventDumpDelegate;
using bsm::EventDumpOptions;

EventDumpOptions::EventDumpOptions()
{
    _delegate = 0;

    _description.reset(new po::options_description("Event Dump Options"));
    _description->add_options()
        ("events",
         po::value<Events>()->notifier(
             boost::bind(&EventDumpOptions::setEvents, this, _1)),
         "Events to dump [repeatable]. Format: event[:lumi[:run]]")
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



// Event Dump Analyzer
//
class EventDumpAnalyzer::EventSearcher
{
    public:
        EventSearcher(const Event::Extra &extra):
            _extra(extra)
        {
        }

        bool operator()(const Event::Extra &extra)
        {
            return (extra.run() ? extra.run() == _extra.run() : true)
                && (extra.lumi() ? extra.lumi() == _extra.lumi() : true)
                && extra.id() == _extra.id();
        }

    private:
        const Event::Extra &_extra;
};

EventDumpAnalyzer::EventDumpAnalyzer()
{
    _primary_vertex_selector.reset(new PrimaryVertexSelector());
    _jet_selector.reset(new JetSelector());
    _electron_selector.reset(new ElectronSelector());
    _muon_selector.reset(new MuonSelector());

    _electron_selector->primary_vertex()->disable();
    _muon_selector->pt()->setValue(35);

    monitor(_primary_vertex_selector);
    monitor(_jet_selector);
    monitor(_electron_selector);
    monitor(_muon_selector);
}

EventDumpAnalyzer::EventDumpAnalyzer(const EventDumpAnalyzer &object):
    _events(object._events)
{
    _primary_vertex_selector = 
        dynamic_pointer_cast<PrimaryVertexSelector>(object._primary_vertex_selector->clone());

    _jet_selector = 
        dynamic_pointer_cast<JetSelector>(object._jet_selector->clone());

    _electron_selector = 
        dynamic_pointer_cast<ElectronSelector>(object._electron_selector->clone());

    _muon_selector = 
        dynamic_pointer_cast<MuonSelector>(object._muon_selector->clone());

    monitor(_primary_vertex_selector);
    monitor(_jet_selector);
    monitor(_electron_selector);
    monitor(_muon_selector);
}

void EventDumpAnalyzer::setEventNumber(const Event::Extra &event)
{
    if (_events.end() == find(_events.begin(), _events.end(), event))
        _events.push_back(event);
}

void EventDumpAnalyzer::onFileOpen(const std::string &filename, const Input *)
{
}

void EventDumpAnalyzer::process(const Event *event)
{
    if (!event->has_extra())
        return;

    EventSearcher predicate(event->extra());

    if (_events.empty()
            || _events.end() != find_if(_events.begin(), _events.end(), predicate))    
        dump(event);
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

// Private
//
void EventDumpAnalyzer::dump(const Event *event)
{
    _out << "Event: " << event->extra().id()
        << " Lumi: " << event->extra().lumi()
        << " Run: " << event->extra().run() << endl;

    dumpPrimaryVertices(event);
    dumpJets(event);
    dumpElectrons(event);
    dumpMuons(event);
}

void EventDumpAnalyzer::dumpPrimaryVertices(const Event *event)
{
    if (!event->primary_vertices().size())
        return;

    typedef ::google::protobuf::RepeatedPtrField<PrimaryVertex> PrimaryVertices;

    _out << "Primary Vertices" << endl;

    uint32_t id = 1;
    for(PrimaryVertices::const_iterator pv = event->primary_vertices().begin();
            event->primary_vertices().end() != pv;
            ++pv, ++id)
    {
        _out << "[" << setw(2) << right << id << "] "
            << setw(4) << left << (_primary_vertex_selector->apply(*pv)
                    ? "->"
                    : "-x")
            << pv->vertex();

        if (pv->has_extra())
        {
            _out << " rho: " << pv->extra().rho()
                << " ndof: " << pv->extra().ndof();
        }

        _out << endl;
    }

    _out << endl;
}

void EventDumpAnalyzer::dumpJets(const Event *event)
{
    if (!event->jets().size())
        return;

    typedef ::google::protobuf::RepeatedPtrField<Jet> Jets;

    _out << "Jets" << endl;

    uint32_t id = 1;
    for(Jets::const_iterator jet = event->jets().begin();
            event->jets().end() != jet;
            ++jet, ++id)
    {
        const LorentzVector &p4 = jet->physics_object().p4();
        _out << "[" << setw(2) << right << id << "] "
            << setw(4) << left << (_jet_selector->apply(*jet)
                    ? "->"
                    : "-x")
            << "pT: " << pt(p4)
            << " eta: " << eta(p4)
            << endl;
    }

    _out << endl;
}

void EventDumpAnalyzer::dumpElectrons(const Event *event)
{
    if (!event->pf_electrons().size())
        return;

    typedef ::google::protobuf::RepeatedPtrField<Electron> Electrons;

    _out << "Electrons" << endl;

    const PrimaryVertex &pv = *event->primary_vertices().begin();

    uint32_t id = 1;
    for(Electrons::const_iterator electron = event->pf_electrons().begin();
            event->pf_electrons().end() != electron;
            ++electron, ++id)
    {
        const LorentzVector &p4 = electron->physics_object().p4();

        _out << "[" << setw(2) << right << id << "] "
            << setw(4) << left << (_electron_selector->apply(*electron, pv)
                    ? "->"
                    : "-x")
            << "et: " << et(p4)
            << " eta: " << eta(p4)
            << endl;

        _out << setw(9) << " "
            << electron->physics_object().vertex() << endl;
    }

    _out << endl;
}

void EventDumpAnalyzer::dumpMuons(const Event *event)
{
    if (!event->pf_muons().size())
        return;

    typedef ::google::protobuf::RepeatedPtrField<Muon> Muons;

    _out << "Muons" << endl;

    const PrimaryVertex &pv = *event->primary_vertices().begin();

    uint32_t id = 1;
    for(Muons::const_iterator muon = event->pf_muons().begin();
            event->pf_muons().end() != muon;
            ++muon, ++id)
    {
        const LorentzVector &p4 = muon->physics_object().p4();

        _out << "[" << setw(2) << right << id << "] "
            << setw(4) << left << (_muon_selector->apply(*muon, pv)
                    ? "->"
                    : "-x")
            << "pT: " << pt(p4)
            << " eta: " << eta(p4)
            << endl;

        _out << setw(9) << " "
            << muon->physics_object().vertex() << endl;
    }

    _out << endl;
}
