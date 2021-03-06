// Trigger Analyzer
//
// Apply trigger to events
//
// Created by Samvel Khalatyan, May 26, 2011
// Copyright 2011, All rights reserved

#include <iomanip>
#include <iostream>
#include <ostream>

#include <boost/algorithm/string.hpp>
#include <boost/functional/hash.hpp>
#include <boost/pointer_cast.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Input.pb.h"
#include "bsm_input/interface/Trigger.pb.h"
#include "interface/TriggerAnalyzer.h"

using namespace std;
using namespace boost;

using bsm::TriggerAnalyzer;
using bsm::TriggerOptions;

// Trigger options
//
TriggerOptions::TriggerOptions()
{
    _delegate = 0;

    _description.reset(new po::options_description("Trigger Options"));
    _description->add_options()
        ("trigger",
         po::value<Triggers>()->notifier(
             boost::bind(&TriggerOptions::setTrigger, this, _1)),
         "Use trigger")

        ("trigger-filter",
         po::value<string>()->notifier(
             boost::bind(&TriggerOptions::setFilter, this, _1)),
         "Set trigger filter name to be used")

        ("trigger-producer",
         po::value<string>()->notifier(
             boost::bind(&TriggerOptions::setProducer, this, _1)),
         "Set trigger producer name to be used")
    ;
}

void TriggerOptions::setDelegate(TriggerDelegate *delegate)
{
    if (_delegate != delegate)
        _delegate = delegate;
}

bsm::TriggerDelegate *TriggerOptions::delegate() const
{
    return _delegate;
}

// Options interface
//
TriggerOptions::DescriptionPtr
    TriggerOptions::description() const
{
    return _description;
}

// Private
//
void TriggerOptions::setTrigger(const Triggers &trigger_names) const
{
    if (!delegate())
        return;

    for(Triggers::const_iterator name = trigger_names.begin();
            trigger_names.end() != name;
            ++name)
    {
        hash<std::string> make_hash;

        string trigger_name = *name;
        to_lower(trigger_name);
        Trigger trigger;
        trigger.set_hash(make_hash(trigger_name));

        delegate()->setTrigger(trigger);
    }
}

void TriggerOptions::setFilter(string filter) const
{
    if (!delegate())
        return;

    hash<std::string> make_hash;

    to_lower(filter);
    delegate()->setFilter(make_hash(filter));
}

void TriggerOptions::setProducer(string producer) const
{
    if (!delegate())
        return;

    hash<std::string> make_hash;

    to_lower(producer);
    delegate()->setProducer(make_hash(producer));
}



// Trigger Analzyer
//
TriggerAnalyzer::TriggerAnalyzer()
{
}

void TriggerAnalyzer::onFileOpen(const std::string &filename, const Input *input)
{
    if (!input->has_info())
    {
        clog << "event info is not available" << endl;

        return;
    }

    if (!input->info().trigger().path().size())
    {
        clog << "Trigger menu is not available" << endl;

        return;
    }

    typedef ::google::protobuf::RepeatedPtrField<TriggerItem> TriggerItems;
    for(TriggerItems::const_iterator hlt = input->info().trigger().path().begin();
            input->info().trigger().path().end() != hlt;
            ++hlt)
    {
        if (_hlt_map.end() != _hlt_map.find(hlt->hash()))
            continue;

        _hlt_map[hlt->hash()] = hlt->name();
    }
}

void TriggerAnalyzer::process(const Event *event)
{
    typedef ::google::protobuf::RepeatedPtrField<Trigger> Triggers;

    if (!event->hlt().trigger().size())
    {
        cout << "HLT is not available" << endl;

        return;
    }

    for(Triggers::const_iterator hlt = event->hlt().trigger().begin();
            event->hlt().trigger().end() != hlt;
            ++hlt)
    {
        if (hlt->pass())
            ++_hlt_cutflow[*hlt];
        else
            _hlt_cutflow[*hlt];
    }
}

uint32_t TriggerAnalyzer::id() const
{
    return core::ID<TriggerAnalyzer>::get();
}

TriggerAnalyzer::ObjectPtr TriggerAnalyzer::clone() const
{
    return ObjectPtr(new TriggerAnalyzer(*this));
}

void TriggerAnalyzer::merge(const ObjectPtr &object_pointer)
{
    if (id() != object_pointer->id())
        return;

    boost::shared_ptr<TriggerAnalyzer> object =
        dynamic_pointer_cast<TriggerAnalyzer>(object_pointer);

    if (!object)
        return;

    // Copy found Triggers
    //
    for(HLTMap::const_iterator hlt = object->_hlt_map.begin();
            object->_hlt_map.end() != hlt;
            ++hlt)
    {
        _hlt_map.insert(*hlt);
    }

    for(HLTCutflow::const_iterator hlt = object->_hlt_cutflow.begin();
            object->_hlt_cutflow.end() != hlt;
            ++hlt)
    {
        _hlt_cutflow[hlt->first] += hlt->second;
    }
}

void TriggerAnalyzer::print(std::ostream &out) const
{
    out << "Found " << _hlt_map.size() << " HLT(s) in file(s)" << endl;
    out << setw(70) << right << setfill('-') << " " << setfill(' ') << endl;
    out << setw(50) << left << "Name" << " "
        << setw(2) << left << "V" << " "
        << setw(3) << left << "PS"  << " "
        << "Events" << endl;
    out << setw(70) << right << setfill('-') << " " << setfill(' ') << endl;
    for(HLTCutflow::const_iterator hlt = _hlt_cutflow.begin();
            _hlt_cutflow.end() != hlt;
            ++hlt)
    {
        HLTMap::const_iterator name = _hlt_map.find(hlt->first.hash());
        if (_hlt_map.end() == name)
        {
            cerr << "Trigger is missing in the Hash map" << endl;

            continue;
        }

        out << setw(50) << left << name->second
            << " " << setw(2) << left <<hlt->first.version()
            << " " << setw(3) << left << hlt->first.prescale()
            << " " << hlt->second << endl;
    }
    out << setw(70) << right << setfill('-') << " " << setfill(' ') << endl;
}



// Helpers
//
bool bsm::operator <(const bsm::Trigger &t1, const bsm::Trigger &t2)
{
    if (t1.hash() < t2.hash())
        return true;

    if (t1.hash() == t2.hash())
    {
        if (t1.version() < t2.version())
            return true;

        if (t1.version() == t2.version())
        {
            if (t1.prescale() < t2.prescale())
                return true;
        }
    }

    return false;
}

std::ostream &bsm::operator <<(std::ostream &out, const bsm::Trigger &t)
{
    out << "h: " << setw(20) << left << t.hash()
        << " " << setw(3) << left << t.version()
        << " " << t.prescale();

    return out;
}
