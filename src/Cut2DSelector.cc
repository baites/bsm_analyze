// 2D Cut with sygnal and differente background regions
//
// Created by Samvel Khalatyan, Aug 24, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <iomanip>
#include <ostream>

#include <boost/algorithm/string.hpp>
#include <boost/pointer_cast.hpp>

#include "bsm_input/interface/Algebra.h"
#include "interface/Cut.h"
#include "interface/Cut2DSelector.h"
#include "interface/Utility.h"

using namespace std;
using namespace boost;

using bsm::Cut2DSelector;
using bsm::Cut2DSelectorDelegate;
using bsm::Cut2DSelectorOptions;

// Cut2D Selector Options
//
Cut2DSelectorOptions::Cut2DSelectorOptions()
{
    _delegate = 0;

    _description.reset(new po::options_description("Cut2D Selector Options"));
    _description->add_options()
        ("region",
         po::value<string>()->notifier(
             boost::bind(&Cut2DSelectorOptions::setRegion, this, _1)),
         "2D cut region: signal, s1, s2, s3")
    ;
}

Cut2DSelectorOptions::~Cut2DSelectorOptions()
{
}

void Cut2DSelectorOptions::setDelegate(Cut2DSelectorDelegate *delegate)
{
    if (_delegate != delegate)
        _delegate = delegate;
}

Cut2DSelectorDelegate *Cut2DSelectorOptions::delegate() const
{
    return _delegate;
}

// Options interface
//
Cut2DSelectorOptions::DescriptionPtr Cut2DSelectorOptions::description() const
{
    return _description;
}

// Privates
//
void Cut2DSelectorOptions::setRegion(std::string region)
{
    if (!delegate())
        return;

    to_lower(region);

    if ("signal" == region)
        delegate()->setRegion(Cut2DSelectorDelegate::SIGNAL);
    else if ("s1" == region)
        delegate()->setRegion(Cut2DSelectorDelegate::S1);
    else if ("s2" == region)
        delegate()->setRegion(Cut2DSelectorDelegate::S2);
    else if ("s3" == region)
        delegate()->setRegion(Cut2DSelectorDelegate::S3);
    else
        cerr << "unsupported 2D cut selector region" << endl;
}



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
