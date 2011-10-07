// Extract Trigger associated objects
//
// Created by Samvel Khalatyan, Oct 07, 2011
// Copyright 2011, All rights reserved

#include <boost/pointer_cast.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Input.pb.h"
#include "bsm_input/interface/Trigger.pb.h"
#include "interface/TriggerWithFilterAnalyzer.h"

using namespace boost;
using namespace bsm;
using namespace std;

TriggerWithFilterAnalyzer::TriggerWithFilterAnalyzer()
{
}

TriggerWithFilterAnalyzer::TriggerWithFilterAnalyzer(const TriggerWithFilterAnalyzer &object):
    _trigger(object._trigger)
{
}

void TriggerWithFilterAnalyzer::setTrigger(const Trigger &trigger)
{
    _trigger = trigger;
}

void TriggerWithFilterAnalyzer::onFileOpen(const string &filename, const Input *input)
{
    _trigger_map.clear();
    _filter_map.clear();

    if (!input->has_info()
            || !input->info().has_trigger())
    {
        _out << "event info or trigger is not available in the input" << endl;

        return;
    }

    if (!input->info().trigger().path().size())
    {
        _out << "trigger path map is not available" << endl;

        return;
    }

    if (!input->info().trigger().filter().size())
    {
        _out << "trigger filters map is not available" << endl;

        return;
    }

    typedef ::google::protobuf::RepeatedPtrField<TriggerItem> TriggerItems;

    const TriggerItems &paths = input->info().trigger().path();
    for(TriggerItems::const_iterator path = paths.begin();
            paths.end() != path;
            ++path)
    {
        if (path->hash() != _trigger.hash()
                || _trigger_map.end() != _trigger_map.find(path->hash()))
            continue;

        _trigger_map[path->hash()] = path->name();
    }

    const TriggerItems &filters = input->info().trigger().filter();
    for(TriggerItems::const_iterator filter = filters.begin();
            filters.end() != filter;
            ++filter)
    {
        if (_filter_map.end() != _filter_map.find(filter->hash()))
            continue;

        _filter_map[filter->hash()] = filter->name();
    }
}

void TriggerWithFilterAnalyzer::process(const Event *event)
{
    if (!event->has_hlt())
    {
        _out << "hlt is not available in the event" << endl;

        return;
    }

    // Check if any triggers are available in the event
    //
    if (!event->hlt().trigger().size())
        return;

    typedef ::google::protobuf::RepeatedPtrField<Trigger> Triggers;
    typedef ::google::protobuf::RepeatedPtrField<TriggerFilter> Filters;
    typedef ::google::protobuf::RepeatedField<uint32_t> Keys;

    const Triggers &triggers = event->hlt().trigger();
    const Filters &filters = event->hlt().filter();
    for(Triggers::const_iterator trigger = triggers.begin();
            triggers.end() != trigger;
            ++trigger)
    {
        if (trigger->hash() != _trigger.hash())
            continue;

        _out << _trigger_map[trigger->hash()] << endl;
        _out << "   ";

        // Get associated filters
        //
        const Keys &keys = trigger->filter();
        for(Keys::const_iterator key = keys.begin();
                keys.end() != key;
                ++key)
        {
            const TriggerFilter &filter = filters.Get(*key);
            _out << " " << _filter_map[filter.hash()];
        }

        _out << endl;
    }
}

uint32_t TriggerWithFilterAnalyzer::id() const
{
    return core::ID<TriggerWithFilterAnalyzer>::get();
}

TriggerWithFilterAnalyzer::ObjectPtr TriggerWithFilterAnalyzer::clone() const
{
    return ObjectPtr(new TriggerWithFilterAnalyzer(*this));
}

void TriggerWithFilterAnalyzer::merge(const ObjectPtr &pointer)
{
    if (id() != pointer->id());

    shared_ptr<TriggerWithFilterAnalyzer> object =
        dynamic_pointer_cast<TriggerWithFilterAnalyzer>(pointer);

    if (!object)
        return;

    Object::merge(pointer);

    _out << endl;
    _out << object->_out.str();
}

void TriggerWithFilterAnalyzer::print(ostream &out) const
{
    if (!_out.str().empty())
        out << _out.str();
}
