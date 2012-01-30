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

// Counter
//
Counter::Counter():
    _counts(0),
    _is_locked(false),
    _is_lock_on_update(false)
{
    _delegate = 0;
}

Counter::Counter(const Counter &counter):
    _counts(counter._counts),
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

// Obsolete method: use explicit counts instead
//
Counter::operator uint32_t() const
{
    return _counts;
}

uint32_t Counter::counts() const
{
    return _counts;
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

void Counter::add(const uint32_t &counts)
{
    if (isLocked())
        return;

    _counts += counts;

    if (isLockOnUpdate())
    {
        lock();

        _is_lock_on_update = false;
    }

    if (delegate())
        delegate()->didCounterAdd(this);
}

uint32_t Counter::id() const
{
    return core::ID<Counter>::get();
}

Counter::ObjectPtr Counter::clone() const
{
    return ObjectPtr(new Counter(*this));
}

void Counter::merge(const ObjectPtr &pointer)
{
    if (id() != pointer->id())
        return;

    boost::shared_ptr<Counter> object =
        boost::dynamic_pointer_cast<Counter>(pointer);

    if (!object)
        return;

    add(object->counts());
}

void Counter::print(ostream &out) const
{
    out << _counts;
}



// Cut
//
Cut::Cut():
    _value(0),
    _name(""),
    _is_disabled(false),
    _is_inverted(false)
{
    _objects.reset(new Counter());
    _events.reset(new Counter());

    monitor(_objects);
    monitor(_events);
}

Cut::Cut(const float &value, const string &name):
    _value(value),
    _name(name),
    _is_disabled(false),
    _is_inverted(false)
{
    _objects.reset(new Counter());
    _events.reset(new Counter());

    monitor(_objects);
    monitor(_events);
}

Cut::Cut(const Cut &object):
    _value(object.value()),
    _name(object.name()),
    _is_disabled(object.isDisabled()),
    _is_inverted(object.isInverted())
{
    _objects.reset(new Counter(*object.objects()));
    _events.reset(new Counter(*object.events()));

    monitor(_objects);
    monitor(_events);
}

CounterPtr Cut::objects() const
{
    return _objects;
}

CounterPtr Cut::events() const
{
    return _events;
}

float Cut::value() const
{
    return _value;
}

void Cut::setValue(const float &value)
{
    _value = value;
}

string Cut::name() const
{
    return _name;
}

void Cut::setName(const string &name)
{
    _name = name;
}

bool Cut::apply(const float &value)
{
    if (isDisabled())
        return true;

    bool pass = isPass(value);

    if (
        (pass && isInverted()) ||
        (!pass && !isInverted())
    )   return false;

    objects()->add();
    events()->add();

    return true;
}

void Cut::disable()
{
    _is_disabled = true;
}

void Cut::enable()
{
    _is_disabled = false;
}

bool Cut::isDisabled() const
{
    return _is_disabled;
}

void Cut::invert()
{
    _is_inverted = true;
}

void Cut::noinvert()
{
    _is_inverted = false;
}

bool Cut::isInverted() const
{
    return _is_inverted;
}

uint32_t Cut::id() const
{
    return core::ID<Cut>::get();
}

void Cut::merge(const ObjectPtr &pointer)
{
    if (id() != pointer->id())
        return;

    boost::shared_ptr<Cut> object =
        boost::dynamic_pointer_cast<Cut>(pointer);

    if (!object
            || value() != object->value())
        return;

    Object::merge(pointer);
}

void Cut::print(ostream &out) const
{
    out << setw(5) << left << value() << " ";
   
    if (!isDisabled())
        out << setw(7) << left << *objects()
            << " " << *events();
    else
        out << setw(7) << left << "-"
            << " " << " ";
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
