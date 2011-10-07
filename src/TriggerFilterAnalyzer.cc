// Extract Trigger filters and associated trigger objects
//
// Created by Samvel Khalatyan, Oct 07, 2011
// Copyright 2011, All rights reserved

#include <boost/pointer_cast.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Input.pb.h"
#include "bsm_input/interface/Trigger.pb.h"
#include "interface/TriggerFilterAnalyzer.h"

using namespace boost;
using namespace bsm;
using namespace std;

TriggerFilterAnalyzer::TriggerFilterAnalyzer()
{
}

TriggerFilterAnalyzer::TriggerFilterAnalyzer(const TriggerFilterAnalyzer &)
{
}

void TriggerFilterAnalyzer::onFileOpen(const string &filename, const Input *input)
{
    _filter_map.clear();

    if (!input->has_info()
            || !input->info().has_trigger())
    {
        _out << "event info or trigger is not available in the input" << endl;

        return;
    }

    if (!input->info().trigger().filter().size())
    {
        _out << "trigger filters map is not available" << endl;

        return;
    }

    typedef ::google::protobuf::RepeatedPtrField<TriggerItem> TriggerItems;
    const TriggerItems &filters = input->info().trigger().filter();
    for(TriggerItems::const_iterator filter = filters.begin();
            filters.end() != filter;
            ++filter)
    {
        _filter_map[filter->hash()] = filter->name();
    }
}

void TriggerFilterAnalyzer::process(const Event *event)
{
    if (!event->has_hlt())
    {
        _out << "hlt is not available in the event" << endl;

        return;
    }

    if (!event->hlt().filter().size())
        return;

    typedef ::google::protobuf::RepeatedPtrField<TriggerFilter>
        TriggerFilters;

    typedef ::google::protobuf::RepeatedField<uint32_t> Keys;

    const TriggerFilters &filters = event->hlt().filter();
    for(TriggerFilters::const_iterator filter = filters.begin();
            filters.end() != filter;
            ++filter)
    {
        HashTable::const_iterator name = _filter_map.find(filter->hash());
        if (_filter_map.end() == name)
        {
            _out << "unexpected filter: " << filter->hash() << endl;

            continue;
        }

        _out << setw(70) << name->second << "  ";
        const Keys &keys = filter->key();
        for(Keys::const_iterator key = keys.begin();
                keys.end() != key;
                ++key)
        {
            _out << " " << *key;
        }
        _out << endl;
    }

    _out << endl;
}

uint32_t TriggerFilterAnalyzer::id() const
{
    return core::ID<TriggerFilterAnalyzer>::get();
}

TriggerFilterAnalyzer::ObjectPtr TriggerFilterAnalyzer::clone() const
{
    return ObjectPtr(new TriggerFilterAnalyzer(*this));
}

void TriggerFilterAnalyzer::merge(const ObjectPtr &pointer)
{
    if (id() != pointer->id());

    shared_ptr<TriggerFilterAnalyzer> object =
        dynamic_pointer_cast<TriggerFilterAnalyzer>(pointer);

    if (!object)
        return;

    Object::merge(pointer);

    _out << endl;
    _out << object->_out.str();
}

void TriggerFilterAnalyzer::print(ostream &out) const
{
    if (!_out.str().empty())
        out << _out.str();
}
