// Cut's framework
//
// Created by Samvel Khalatyan, Jun 02, 2011
// Copyright 2011, All rights reserved

#include <functional>

#include <boost/pointer_cast.hpp>

#include "interface/Cut.h"
#include "interface/Utility.h"

using namespace std;

using boost::dynamic_pointer_cast;

using bsm::Counter;
using bsm::CounterPtr;
using bsm::Cut;
using bsm::LockCounterOnUpdate;
using bsm::RangeCut;

// Counter
//
Counter::Counter():
    _count(0),
    _is_locked(false),
    _is_lock_on_update(false)
{
    _delegate = 0;
}

void Counter::setDelegate(CounterDelegate *delegate)
{
    if (_delegate != delegate)
        _delegate = delegate;
}

bsm::CounterDelegate *Counter::delegate() const
{
    return _delegate;
}

Counter::operator uint32_t() const
{
    return _count;
}

bool Counter::isLocked() const
{
    return _is_locked;
}

bool Counter::isLockOnUpdate() const
{
    return _is_lock_on_update;
}

void Counter::lock()
{
    _is_locked = true;
}

void Counter::lockOnUpdate()
{
    _is_lock_on_update = true;
}

void Counter::unlock()
{
    _is_locked = false;
    _is_lock_on_update = false;
}

void Counter::add()
{
    if (isLocked())
        return;

    ++_count;

    update();

    if (delegate())
        delegate()->didCounterAdd();
}

uint32_t Counter::id() const
{
    return core::ID<Counter>::get();
}

Counter::ObjectPtr Counter::clone() const
{
    return ObjectPtr(new Counter(*this));
}

void Counter::merge(const ObjectPtr &object_pointer)
{
    if (isLocked()
            || id() != object_pointer->id())
        return;

    boost::shared_ptr<Counter> object =
        boost::dynamic_pointer_cast<Counter>(object_pointer);

    if (!object)
        return;

    _count += object->_count;

    update();
}

void Counter::print(ostream &out) const
{
    out << _count;
}

// Private
//
void Counter::update()
{
    if (isLockOnUpdate())
    {
        lock();

        _is_lock_on_update = false;
    }
}



// Cut
//
Cut::Cut():
    _value(0),
    _is_disabled(false)
{
    _objects.reset(new Counter());
    _events.reset(new Counter());

    monitor(_objects);
    monitor(_events);
}

Cut::Cut(const float &value, const string &name):
    _value(value),
    _name(name),
    _is_disabled(false)
{
    _objects.reset(new Counter());
    _events.reset(new Counter());

    monitor(_objects);
    monitor(_events);
}

Cut::Cut(const Cut &object):
    _value(object._value),
    _name(object._name),
    _is_disabled(object._is_disabled)
{
    _objects.reset(new Counter(*object._objects));
    _events.reset(new Counter(*object._events));

    monitor(_objects);
    monitor(_events);
}

Cut::~Cut()
{
}

const CounterPtr Cut::objects() const
{
    return _objects;
}

const CounterPtr Cut::events() const
{
    return _events;
}

string Cut::name() const
{
    return _name;
}

void Cut::setName(const string &name)
{
    _name = name;
}

float Cut::value() const
{
    return _value;
}

void Cut::setValue(const float &value)
{
    _value = value;
}

bool Cut::apply(const float &value)
{
    if (isDisabled())
        return true;

    if (!isPass(value))
        return false;

    _objects->add();
    _events->add();

    return true;
}

bool Cut::isDisabled() const
{
    return _is_disabled;
}

void Cut::disable()
{
    _is_disabled = true;
}

void Cut::enable()
{
    _is_disabled = false;
}

uint32_t Cut::id() const
{
    return core::ID<Cut>::get();
}

void Cut::merge(const ObjectPtr &object_pointer)
{
    if (id() != object_pointer->id())
        return;

    boost::shared_ptr<Cut> object =
        boost::dynamic_pointer_cast<Cut>(object_pointer);

    if (!object
            || _value != object->_value
            || _name != object->_name)
        return;

    _is_disabled = object->_is_disabled;

    Object::merge(object_pointer);
}

void Cut::print(ostream &out) const
{
    out << setw(5) << left << value() << " ";
   
    if (!isDisabled())
        out << setw(7) << left << *objects()
            << " " << *events();
}



// RangeCut
//
RangeCut::RangeCut(const float &lower_cut_value,
        const float &upper_cut_value,
        const string &name)
{
    _lower_cut.reset(new Comparator<>(lower_cut_value, name));
    _upper_cut.reset(new Comparator<less<float> >(upper_cut_value, name));

    monitor(_lower_cut);
    monitor(_upper_cut);

    _objects.reset(new Counter());
    _events.reset(new Counter());

    monitor(_objects);
    monitor(_events);
}

RangeCut::RangeCut(const RangeCut &object)
{
    _lower_cut = dynamic_pointer_cast<Cut>(object.lowerCut()->clone());
    _upper_cut = dynamic_pointer_cast<Cut>(object.upperCut()->clone());

    monitor(_lower_cut);
    monitor(_upper_cut);

    _objects = dynamic_pointer_cast<Counter>(object.objects()->clone());
    _events = dynamic_pointer_cast<Counter>(object.events()->clone());

    monitor(_objects);
    monitor(_events);
}

RangeCut::CutPtr RangeCut::lowerCut() const
{
    return _lower_cut;
}

RangeCut::CutPtr RangeCut::upperCut() const
{
    return _upper_cut;
}

const CounterPtr RangeCut::objects() const
{
    return _objects;
}

const CounterPtr RangeCut::events() const
{
    return _events;
}

float RangeCut::value() const
{
    return 0;
}

void RangeCut::setValue(const float &value)
{
}

string RangeCut::name() const
{
    return lowerCut()->name();
}

void RangeCut::setName(const string &name)
{
    lowerCut()->setName(name);
    upperCut()->setName(name);
}

bool RangeCut::apply(const float &value)
{
    if (isDisabled())
        return true;

    // Both cuts should be applied independently of each other
    //
    bool lower_cut = lowerCut()->apply(value);
    bool upper_cut = upperCut()->apply(value);

    if (!lower_cut
            || !upper_cut)
        return false;

    _objects->add();
    _events->add();

    return true;
}

bool RangeCut::isDisabled() const
{
    return lowerCut()->isDisabled()
        && upperCut()->isDisabled();
}

void RangeCut::disable()
{
    lowerCut()->disable();
    upperCut()->disable();
}

void RangeCut::enable()
{
    lowerCut()->enable();
    upperCut()->enable();
}

uint32_t RangeCut::id() const
{
    return core::ID<RangeCut>::get();
}

void RangeCut::print(ostream &out) const
{
    lowerCut()->print(out);
    out  << endl;
    upperCut()->print(out);
}



// Lock counter on update
//
LockCounterOnUpdate::LockCounterOnUpdate(const CounterPtr &counter):
    _counter(counter)
{
    _counter->lockOnUpdate();
}

LockCounterOnUpdate::~LockCounterOnUpdate()
{
    _counter->unlock();
}
