// Extract Trigger producers and associated trigger objects
//
// Created by Samvel Khalatyan, Oct 07, 2011
// Copyright 2011, All rights reserved

#include <boost/pointer_cast.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Input.pb.h"
#include "bsm_input/interface/Trigger.pb.h"
#include "interface/TriggerProducerAnalyzer.h"

using namespace boost;
using namespace bsm;
using namespace std;

TriggerProducerAnalyzer::TriggerProducerAnalyzer()
{
}

TriggerProducerAnalyzer::TriggerProducerAnalyzer(const TriggerProducerAnalyzer &)
{
}

void TriggerProducerAnalyzer::onFileOpen(const string &filename, const Input *input)
{
    _producer_map.clear();

    if (!input->has_info()
            || !input->info().has_trigger())
    {
        _out << "event info or trigger is not available in the input" << endl;

        return;
    }

    if (!input->info().trigger().producer().size())
    {
        _out << "trigger producers map is not available" << endl;

        return;
    }

    typedef ::google::protobuf::RepeatedPtrField<TriggerItem> TriggerItems;
    const TriggerItems &producers = input->info().trigger().producer();
    for(TriggerItems::const_iterator producer = producers.begin();
            producers.end() != producer;
            ++producer)
    {
        _producer_map[producer->hash()] = producer->name();
    }
}

void TriggerProducerAnalyzer::process(const Event *event)
{
    if (!event->has_hlt())
    {
        _out << "hlt is not available in the event" << endl;

        return;
    }

    if (!event->hlt().producer().size())
        return;

    typedef ::google::protobuf::RepeatedPtrField<TriggerProducer>
        TriggerProducers;
    const TriggerProducers &producers = event->hlt().producer();
    for(TriggerProducers::const_iterator producer = producers.begin();
            producers.end() != producer;
            ++producer)
    {
        HashTable::const_iterator name = _producer_map.find(producer->hash());
        if (_producer_map.end() == name)
        {
            _out << "unexpected producer: " << producer->hash() << endl;

            continue;
        }

        _out << setw(3) << producer->from() << ".."
            << setw(3) << producer->to() << "   "
            << name->second << endl;

        // Print associated Trigger Objects
        //
    }

    _out << endl;
}

uint32_t TriggerProducerAnalyzer::id() const
{
    return core::ID<TriggerProducerAnalyzer>::get();
}

TriggerProducerAnalyzer::ObjectPtr TriggerProducerAnalyzer::clone() const
{
    return ObjectPtr(new TriggerProducerAnalyzer(*this));
}

void TriggerProducerAnalyzer::merge(const ObjectPtr &pointer)
{
    if (id() != pointer->id());

    shared_ptr<TriggerProducerAnalyzer> object =
        dynamic_pointer_cast<TriggerProducerAnalyzer>(pointer);

    if (!object)
        return;

    Object::merge(pointer);

    _out << endl;
    _out << object->_out.str();
}

void TriggerProducerAnalyzer::print(ostream &out) const
{
    if (!_out.str().empty())
        out << _out.str();
}
