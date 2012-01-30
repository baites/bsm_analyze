// Cut's framework
//
// Created by Samvel Khalatyan, Jun 02, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_CUT
#define BSM_CUT

#include <iomanip>
#include <stdexcept>
#include <string>

#include <boost/shared_ptr.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_core/interface/Object.h"
#include "interface/bsm_fwd.h"

namespace bsm
{
    class CounterDelegate
    {
        public:
            virtual ~CounterDelegate() {};

            virtual void didCounterAdd(const Counter *) {};
    };



    // Simple counter of anything. There are two states defined: locked and
    // unlocked. If counter is locked, then any attempt to modify it will
    // silently be skipped. Counter may also lock itself on update.
    //
    class Counter : public core::Object
    {
        public:
            Counter();
            Counter(const Counter &);

            void setDelegate(CounterDelegate *);
            CounterDelegate *delegate() const;

            // Get number of counts
            //
            operator uint32_t() const; // obsolete: do not use
            uint32_t counts() const; // replacement for the convertion

            // Status check
            //
            bool isLocked() const;
            bool isLockOnUpdate() const;

            // Lock counter: no further changes are allowed until unlocked
            //
            void lock();

            // Lock counter on update: if changed, no further updates are
            // allowed until unlocked
            //
            void lockOnUpdate();

            // Unlock the counter: allow further changes
            //
            void unlock();

            // Increase counter
            //
            void add(const uint32_t &counts = 1);

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;

            // Counters will be merged only if current one is unlocked
            //
            virtual void merge(const ObjectPtr &);

            virtual void print(std::ostream &) const;

        private:
            uint32_t _counts;

            bool _is_locked;
            bool _is_lock_on_update;

            CounterDelegate *_delegate;
    };



    // Cut is a wrapper around the cut value and its name. It has two internal
    // counters: objects and events. By default, both counters are advanced if
    // cut is passed. However, one many lock/unlock these counters from
    // outside.
    //
    // For example, events counter may be locked on update at the beginning of
    // the event analysis. The objects counter can be left as is. Unlock the
    // events counter at the end of the event analysis. This way one may count
    // how many events, objects pass the cut. Note: event many have more than
    // one objects - the above counters are different.
    //
    class Cut : public core::Object
    {
        public:
            // By default, cut value will be initialized with zero
            //
            Cut();

            Cut(const float &value, const std::string &name = "");
            Cut(const Cut &);

            virtual CounterPtr objects() const;
            virtual CounterPtr events() const;

            // Get/Set the cut value
            //
            virtual float value() const;
            virtual void setValue(const float &);

            // Get/Set the cut name
            //
            virtual std::string name() const;
            virtual void setName(const std::string &);

            // apply cut and count a number of success
            //
            virtual bool apply(const float &);

            // Disable cut: apply method will always return True
            //
            virtual void disable();

            // Enable cut: let apply method do its job
            //
            virtual void enable();

            // Check the cut status
            //
            virtual bool isDisabled() const;

            // Disable cut: apply method will always return True
            //
            virtual void noinvert();

            // Enable cut: let apply method do its job
            //
            virtual void invert();

            // Check the cut status
            //
            virtual bool isInverted() const;

            // Object interface
            //
            virtual uint32_t id() const;

            // Only cuts with the same cut value can be merged
            //
            virtual void merge(const ObjectPtr &);

            virtual void print(std::ostream &) const;

        private:
            // Cut implementation: method should be overriden by children
            //
            virtual bool isPass(const float &) = 0;

            float _value;       // cut value
            std::string _name;  // cut name
            bool _is_disabled;
            bool _is_inverted;

            CounterPtr _objects;    // Counter of objects
            CounterPtr _events;     // Counter of events
    };



    // RAII type counter lock: set counter to lock on update when object is
    // created and unlock the same counter on object destruction
    //
    class LockCounterOnUpdate
    {
        public:
            LockCounterOnUpdate(const CounterPtr &);
            ~LockCounterOnUpdate();

        private:
            // Prevent copying
            //
            LockCounterOnUpdate(const LockCounterOnUpdate &);
            LockCounterOnUpdate &operator =(const LockCounterOnUpdate &);

            CounterPtr _counter;
    };



    // One side cut with comparison policy: less, greater, etc. Policy is
    // defined with std functors [http://goo.gl/bh9dl]
    //
    // Examples:
    //
    //  1.  boost::shared_ptr<Cut> default_cut(new Comparator<>(10));
    //      if (default_cut->apply(energy))
    //      {
    //          // Energy > 10
    //      }
    //      else
    //      {
    //          // Energy <= 10
    //      }
    //
    //  2.  boost::shared_ptr<Cut> my_cut(
    //          new Comparator<std::less_equal<float> >(10));
    //      if (my_cut->apply(energy))
    //      {
    //          // Energy <= 10
    //      }
    //      else
    //      {
    //          // Energy > 10
    //      }
    //
    //  3.  // Default Cut print
    //      cout << "Default cut (passes): " << *default_cut << endl;
    //
    //  4.  // Custom print
    //      cout << "Default cut (> " << default_cut->value() << ")" << endl;
    //
    template<class Compare = std::greater<float> >
        class Comparator : public Cut
        {
            public:
                Comparator(const float &value, const std::string &name = "");

                // Access functor
                //
                const Compare functor() const;

                // Object interface
                //
                virtual uint32_t id() const;

                virtual ObjectPtr clone() const;

                virtual void print(std::ostream &out) const;

            protected:
                // Apply cut
                //
                virtual bool isPass(const float &number);

            private:
                Compare _functor;
        };



    // One sides cut with comparison policy on each side: less, greater, etc.
    // Policy is defined with std functors [http://goo.gl/bh9dl]
    //
    // Note: both lower/uppers cuts are always applied, and then results are
    //       merged with logic
    //
    // Examples:
    //
    //  1.  boost::shared_ptr<Cut>
    //          cut(new RangeComparator<>(5, 10, "Default Cut"));
    //      if (cut->apply(value))
    //      {
    //          // value is > 5 and < 10
    //      }
    //      else
    //      {
    //          // value is <= 5 or >= 10
    //      }
    //
    //  2.  boost::shared_ptr<Cut>
    //          cut(new RnageComparator<std::greater_equal<float>,
    //              std::less_equal<float> >(5, 10, "Inclusive Cut"));
    //      if (cut->apply(value))
    //      {
    //          // value is >= 5 and <= 10
    //      }
    //      else
    //      {
    //          // value is < 5 or > 10
    //      }
    //
    //  3.  boost::shared_ptr<Cut>
    //          cut(new RnageComparator<std::less<float>,
    //              std::greater<float>,
    //              std::logical_or<bool> >(5, 10, "Outside Range"));
    //      if (cut->apply(value))
    //      {
    //          // value is < 5 or > 10
    //      }
    //      else
    //      {
    //          // value is >= 5 and <= 10
    //      }
    //
    template<class LowerCompare = std::greater<float>,
        class UpperCompare = std::less<float>,
        class Logic = std::logical_and<bool> >
            class RangeComparator : public Cut
        {
            public:
                typedef Comparator<LowerCompare> LowerCut;
                typedef Comparator<UpperCompare> UpperCut;

                typedef boost::shared_ptr<LowerCut> LowerCutPtr;
                typedef boost::shared_ptr<UpperCut> UpperCutPtr;

                RangeComparator(const float &lower_cut,
                        const float &upper_cut,
                        const std::string &name = "");

                RangeComparator(const RangeComparator &);

                // Access lower/upeer cut individually
                //
                LowerCutPtr lowerCut() const;
                UpperCutPtr upperCut() const;

                // Cut interface
                //
                virtual void setName(const std::string &);

                virtual void disable();
                virtual void enable();

                // Object interface
                //
                virtual uint32_t id() const;

                virtual ObjectPtr clone() const;

                virtual void print(std::ostream &out) const;

            protected:
                virtual bool isPass(const float &value);

            private:
                // Prohibited Cut interface
                //

                // cut default value does not make sence since there are two
                // separate cuts: lower and upper. Use these instead
                //
                virtual void setValue(const float &);   // throw exception

                Logic _logic;

                LowerCutPtr _lower_cut;
                UpperCutPtr _upper_cut;
        };
}



// Comparator Template implementation
//
template<class Compare>
    bsm::Comparator<Compare>::Comparator(const float &value,
            const std::string &name):
        Cut(value, name)
{
}

template<class Compare>
    const Compare bsm::Comparator<Compare>::functor() const
{
    return _functor;
}

template<class Compare>
    uint32_t bsm::Comparator<Compare>::id() const
{
    return core::ID<Comparator<Compare> >::get();
}

template<class Compare>
    typename bsm::Comparator<Compare>::ObjectPtr
        bsm::Comparator<Compare>::clone() const
{
    return ObjectPtr(new Comparator<Compare>(*this));
}

template<class Compare>
    void bsm::Comparator<Compare>::print(std::ostream &out) const
{
    out << " [+] " << std::setw(30) << std::right << name() << " " << _functor << " ";

    Cut::print(out);
}

// Protected
//
template<class Compare>
    bool bsm::Comparator<Compare>::isPass(const float &number)
{
    return functor()(number, value());
}



// Range Comparator Template implementation
//
template<class LowerCompare, class UpperCompare, class Logic>
    bsm::RangeComparator<LowerCompare,
        UpperCompare,
        Logic>::RangeComparator(const float &lower_cut,
            const float &upper_cut,
            const std::string &name):
    Cut(0, name)
{
    _lower_cut.reset(new LowerCut(lower_cut, name));
    _upper_cut.reset(new UpperCut(upper_cut, name));

    monitor(_lower_cut);
    monitor(_upper_cut);
}

template<class LowerCompare, class UpperCompare, class Logic>
    bsm::RangeComparator<LowerCompare,
        UpperCompare,
        Logic>::RangeComparator(const RangeComparator<LowerCompare,
                UpperCompare,
                Logic> &object):
    Cut(object)
{
    _lower_cut.reset(new LowerCut(*object.lowerCut()));
    _upper_cut.reset(new UpperCut(*object.upperCut()));

    monitor(_lower_cut);
    monitor(_upper_cut);
}

template<class LowerCompare, class UpperCompare, class Logic>
    typename bsm::RangeComparator<LowerCompare,
        UpperCompare,
        Logic>::LowerCutPtr

        bsm::RangeComparator<LowerCompare,
            UpperCompare,
            Logic>::lowerCut() const
{
    return _lower_cut;
}

template<class LowerCompare, class UpperCompare, class Logic>
    typename bsm::RangeComparator<LowerCompare,
        UpperCompare,
        Logic>::UpperCutPtr

        bsm::RangeComparator<LowerCompare,
            UpperCompare,
            Logic>::upperCut() const
{
    return _upper_cut;
}

template<class LowerCompare, class UpperCompare, class Logic>
    void bsm::RangeComparator<LowerCompare,
        UpperCompare,
        Logic>::setName(const std::string &name)
{
    Cut::setName(name);

    lowerCut()->setName(name);
    upperCut()->setName(name);
}

template<class LowerCompare, class UpperCompare, class Logic>
    void bsm::RangeComparator<LowerCompare,
        UpperCompare,
        Logic>::disable()
{
    Cut::disable();

    lowerCut()->disable();
    upperCut()->disable();
}

template<class LowerCompare, class UpperCompare, class Logic>
    void bsm::RangeComparator<LowerCompare,
        UpperCompare,
        Logic>::enable()
{
    Cut::enable();

    lowerCut()->enable();
    upperCut()->enable();
}

template<class LowerCompare, class UpperCompare, class Logic>
    uint32_t bsm::RangeComparator<LowerCompare, UpperCompare, Logic>::id() const
{
    return core::ID<RangeComparator<LowerCompare, UpperCompare, Logic> >::get();
}

template<class LowerCompare, class UpperCompare, class Logic>
    typename bsm::RangeComparator<LowerCompare, UpperCompare, Logic>::ObjectPtr
        bsm::RangeComparator<LowerCompare, UpperCompare, Logic>::clone() const
{
    return ObjectPtr(new RangeComparator<LowerCompare,
            UpperCompare,
            Logic>(*this));
}

template<class LowerCompare, class UpperCompare, class Logic>
    void bsm::RangeComparator<LowerCompare,
        UpperCompare,
        Logic>::print(std::ostream &out) const
{
    out << " [+] " << std::setw(30) << std::right << name() << " ";

    out << lowerCut()->functor() << " " << lowerCut()->value()
        << " " << _logic << " ";

    out << upperCut()->functor() << " " << upperCut()->value()
        << std::setw(5) << " ";

    if (!isDisabled())
        out << std::setw(7) << std::left << *objects()
            << " " << *events();
    else
        out << std::setw(7) << std::left << "-"
            << " " << " ";
}

// Protected
//
template<class LowerCompare, class UpperCompare, class Logic>
    bool bsm::RangeComparator<LowerCompare,
        UpperCompare,
        Logic>::isPass(const float &number)
{
    // Apply both cuts independently of results
    //
    bool lower_cut = lowerCut()->apply(number);
    bool upper_cut = upperCut()->apply(number);

    return _logic(lower_cut, upper_cut);
}

// Private
//
template<class LowerCompare, class UpperCompare, class Logic>
    void bsm::RangeComparator<LowerCompare,
        UpperCompare,
        Logic>::setValue(const float &value)
{
    throw std::runtime_error("can not set the value to RangeComparator");
}

#endif
