// Cut's framework
//
// Created by Samvel Khalatyan, Jun 02, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_CUT
#define BSM_CUT

#include <iomanip>
#include <stdexcept>
#include <string>

#include <boost/pointer_cast.hpp>
#include <boost/shared_ptr.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_core/interface/Object.h"
#include "interface/bsm_fwd.h"

namespace bsm
{
    typedef boost::shared_ptr<Counter> CounterPtr;
    typedef boost::shared_ptr<Cut> CutPtr;

    class CounterDelegate
    {
        public:
            virtual ~CounterDelegate() {};

            virtual void didCounterAdd(const Counter *) {};
    };

    // Simple counter of anything. There are two states defined: locked and
    // unlocked. If counter is locked, then any attempt to modify it will
    // silently be skipped. Counter may also automatically lock itself on
    // update
    //
    class Counter : public core::Object
    {
        public:
            Counter();

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

    // Store cut value and count successfuly passed objects, events
    //
    class Cut : public core::Object
    {
        public:
            Cut();
            Cut(const float &value, const std::string &name = "");
            Cut(const Cut &);

            virtual ~Cut();

            virtual const CounterPtr objects() const;
            virtual const CounterPtr events() const;

            // Get actual cut value
            //
            virtual float value() const;
            virtual void setValue(const float &);

            // Get name of the cut
            //
            virtual std::string name() const;
            virtual void setName(const std::string &);

            // apply cut: implicitly count number of success
            //
            virtual bool apply(const float &);

            virtual bool isDisabled() const;

            virtual void disable();
            virtual void enable();

            // Object interface
            //
            virtual uint32_t id() const;

            // Only cuts with the same value can be merged
            //
            virtual void merge(const ObjectPtr &);

            virtual void print(std::ostream &) const;

        private:
            // isPass is the actual application of the cut
            //
            virtual bool isPass(const float &) = 0;

            float _value;
            std::string _name;
            bool _is_disabled;

            CounterPtr _objects;
            CounterPtr _events;
    };

    // Comparator has comparison policy defined with std functors:
    //  less, greater [http://goo.gl/bh9dl]
    //
    // Example:
    //  1.  boost::shared_ptr<Cut> default_cut(new Comparator<>(10));
    //      if ((*default_cut)(energy))
    //      {
    //          // Energy > 10
    //      }
    //
    //  2.  boost::shared_ptr<Cut> my_cut(
    //          new Comparator<std::less_equal<float> >(10));
    //      if ((*my_cut)(energy))
    //      {
    //          // Energy <= 10
    //      }
    //
    //  3.  cout << "Default cut (passes): " << *default_cut << endl;
    //      cout << "Default cut (> " << default_cut->value() << ")" << endl;
    //
    template<class Compare = std::greater<float> >
        class Comparator : public Cut
        {
            public:
                Comparator(const float &value, const std::string &name = "");

                const Compare functor() const;

                // Object interface
                //
                virtual uint32_t id() const;

                virtual ObjectPtr clone() const;

                virtual void print(std::ostream &out) const;

            protected:
                virtual bool isPass(const float &number);

            private:
                Compare _functor;
        };

    template<class LowerCompare = std::greater<float>,
        class UpperCompare = std::less<float>,
        class Logic = std::logical_and<bool> >
            class RangeComparator : public Cut
        {
            public:
                RangeComparator(const float &lower_cut,
                        const float &upper_cut,
                        const std::string &name = "");

                RangeComparator(const RangeComparator &);

                CutPtr lowerCut() const;
                CutPtr upperCut() const;

                // Cut interface
                //
                virtual void setValue(const float &);   // throw exception

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
                Logic _logic;

                CutPtr _lower_cut;
                CutPtr _upper_cut;
        };

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
    bool bsm::Comparator<Compare>::isPass(const float &number)
{
    return _functor(number, value());
}

template<class Compare>
    void bsm::Comparator<Compare>::print(std::ostream &out) const
{
    out << " [+] " << std::setw(30) << std::right << name() << " " << _functor << " ";

    Cut::print(out);
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
    _lower_cut.reset(new Comparator<LowerCompare>(lower_cut, name));
    _upper_cut.reset(new Comparator<UpperCompare>(upper_cut, name));

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
    _lower_cut = boost::dynamic_pointer_cast<Cut>(object.lowerCut()->clone());
    _upper_cut = boost::dynamic_pointer_cast<Cut>(object.upperCut()->clone());

    monitor(_lower_cut);
    monitor(_upper_cut);
}

template<class LowerCompare, class UpperCompare, class Logic>
    bsm::CutPtr
        bsm::RangeComparator<LowerCompare,
            UpperCompare,
            Logic>::lowerCut() const
{
    return _lower_cut;
}

template<class LowerCompare, class UpperCompare, class Logic>
    bsm::CutPtr
        bsm::RangeComparator<LowerCompare,
            UpperCompare,
            Logic>::upperCut() const
{
    return _upper_cut;
}

template<class LowerCompare, class UpperCompare, class Logic>
    void bsm::RangeComparator<LowerCompare,
        UpperCompare,
        Logic>::setValue(const float &value)
{
    throw std::runtime_error("can not set value to RangeComparator");
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
    bool bsm::RangeComparator<LowerCompare,
        UpperCompare,
        Logic>::isPass(const float &number)
{
    bool lower_cut = lowerCut()->apply(number);
    bool upper_cut = upperCut()->apply(number);

    return _logic(lower_cut, upper_cut);
}

template<class LowerCompare, class UpperCompare, class Logic>
    void bsm::RangeComparator<LowerCompare,
        UpperCompare,
        Logic>::print(std::ostream &out) const
{
    out << " [+] " << std::setw(30) << std::right << name() << " ";

    boost::shared_ptr<Comparator<LowerCompare> > lower_cut =
        boost::dynamic_pointer_cast<Comparator<LowerCompare> >(lowerCut());

    out << lower_cut->functor() << " " << lower_cut->value()
        << " " << _logic << " ";

    boost::shared_ptr<Comparator<UpperCompare> > upper_cut =
        boost::dynamic_pointer_cast<Comparator<UpperCompare> >(upperCut());

    out << upper_cut->functor() << " " << upper_cut->value()
        << std::setw(5) << " ";

    Cut::print(out);
}

#endif
