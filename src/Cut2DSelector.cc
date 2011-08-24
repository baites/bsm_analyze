// 2D Cut with sygnal and differente background regions
//
// Created by Samvel Khalatyan, Aug 24, 2011
// Copyright 2011, All rights reserved

#include <ostream>
#include <iomanip>

#include <boost/pointer_cast.hpp>

#include "bsm_input/interface/Algebra.h"
#include "interface/Cut.h"
#include "interface/Cut2DSelector.h"
#include "interface/Utility.h"

using namespace std;
using namespace boost;

using bsm::Cut2DSelector;

// Cut2DSelector
//
Cut2DSelector::Cut2DSelector(const Region &region)
{
    setRegion(region);
}

Cut2DSelector::Cut2DSelector(const Cut2DSelector &object):
    _region(object.region())
{
    _dr = dynamic_pointer_cast<Cut>(object.dr()->clone());;
    _ptrel = dynamic_pointer_cast<Cut>(object.ptrel()->clone());;

    monitor(_dr);
    monitor(_ptrel);
}

void Cut2DSelector::setRegion(const Region &new_region)
{
    if (new_region == region()
            && _dr
            && _ptrel)
        return;

    _region = new_region;

    stopMonitor(_dr);
    stopMonitor(_ptrel);

    switch(region())
    {
        case SIGNAL:
            {
                _dr.reset(new Comparator<>(.5));
                _ptrel.reset(new Comparator<>(25));

                break;
            }

        case S1:
            {
                _dr.reset(new RangeComparator<>(.1, .2));
                _ptrel.reset(new Comparator<less<float> >(25));

                break;
            }

        case S2:
            {
                _dr.reset(new RangeComparator<>(.2, .3));
                _ptrel.reset(new Comparator<less<float> >(25));

                break;
            }

        case S3:
            {
                _dr.reset(new RangeComparator<>(.3, .5));
                _ptrel.reset(new Comparator<less<float> >(25));

                break;
            }

        default:
            throw runtime_error("unsupported region");
    }

    _dr->setName("DeltaR");
    _ptrel->setName("pTrel");

    monitor(_dr);
    monitor(_ptrel);
}

Cut2DSelector::Region Cut2DSelector::region() const
{
    return _region;
}

bool Cut2DSelector::apply(const LorentzVector &lepton,
        const LorentzVector &jet)
{
    if (SIGNAL == region())
    {
        return _dr->apply(bsm::dr(lepton, jet))
            || _ptrel->apply(bsm::ptrel(lepton, jet));
    }
    else
        return _dr->apply(bsm::ptrel(lepton, jet))
            && _ptrel->apply(bsm::ptrel(lepton, jet));
}

Cut2DSelector::CutPtr Cut2DSelector::dr() const
{
    return _dr;
}

Cut2DSelector::CutPtr Cut2DSelector::ptrel() const
{
    return _ptrel;
}

void Cut2DSelector::enable()
{
    dr()->enable();
    ptrel()->enable();
}

void Cut2DSelector::disable()
{
    dr()->disable();
    ptrel()->disable();
}

uint32_t Cut2DSelector::id() const
{
    return core::ID<Cut2DSelector>::get();
}

Cut2DSelector::ObjectPtr Cut2DSelector::clone() const
{
    return ObjectPtr(new Cut2DSelector(*this));
}

void Cut2DSelector::print(ostream &out) const
{
    out << "     CUT                 " << setw(5) << " "
        << " Objects Events" << endl;
    out << setw(45) << setfill('-') << left << " " << setfill(' ') << endl;
    out << *_dr << endl;
    out << *_ptrel;
}
