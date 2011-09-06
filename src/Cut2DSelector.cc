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
         "2D cut region: signal, s1, s2, s3, s1s2, s2s3, s1s2s3")
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
    else if ("s1s2" == region)
        delegate()->setRegion(Cut2DSelectorDelegate::S1S2);
    else if ("s2s3" == region)
        delegate()->setRegion(Cut2DSelectorDelegate::S2S3);
    else if ("s1s2s3" == region)
        delegate()->setRegion(Cut2DSelectorDelegate::S1S2S3);
    else
        cerr << "unsupported 2D cut selector region" << endl;
}



// Cut2DSelector
//
Cut2DSelector::Cut2DSelector(const Region &region)
{
    setRegion(region);
}

Cut2DSelector::Region Cut2DSelector::region() const
{
    return _region;
}

bsm::CutPtr Cut2DSelector::cut(const Cut &cut_id) const
{
    return getCut(cut_id);
}

void Cut2DSelector::setRegion(const Region &new_region)
{
    if (new_region == region()
            && cuts())
        return;

    if (cuts())
    {
        removeCut(DELTA_R);
        removeCut(PTREL);
    }

    _region = new_region;

    CutPtr dr;
    CutPtr ptrel;

    switch(region())
    {
        case SIGNAL:
            {
                dr.reset(new Comparator<>(.5));
                ptrel.reset(new Comparator<>(25));

                break;
            }

        case S1:
            {
                dr.reset(new RangeComparator<>(.1, .2));
                ptrel.reset(new Comparator<less<float> >(25));

                break;
            }

        case S2:
            {
                dr.reset(new RangeComparator<>(.2, .3));
                ptrel.reset(new Comparator<less<float> >(25));

                break;
            }

        case S3:
            {
                dr.reset(new RangeComparator<>(.3, .5));
                ptrel.reset(new Comparator<less<float> >(25));

                break;
            }

        case S1S2:
            {
                dr.reset(new RangeComparator<>(.1, .3));
                ptrel.reset(new Comparator<less<float> >(25));

                break;
            }

        case S2S3:
            {
                dr.reset(new RangeComparator<>(.2, .5));
                ptrel.reset(new Comparator<less<float> >(25));

                break;
            }

        case S1S2S3:
            {
                dr.reset(new RangeComparator<>(.1, .5));
                ptrel.reset(new Comparator<less<float> >(25));

                break;
            }

        default:
            throw runtime_error("unsupported region");
    }

    dr->setName("DeltaR");
    ptrel->setName("pTrel");

    addCut(DELTA_R, dr);
    addCut(PTREL, ptrel);
}

bool Cut2DSelector::apply(const LorentzVector &lepton,
        const LorentzVector &jet)
{
    if (SIGNAL == region())
    {
        return cut(DELTA_R)->apply(bsm::dr(lepton, jet))
            || cut(PTREL)->apply(bsm::ptrel(lepton, jet));
    }
    else
        return cut(DELTA_R)->apply(bsm::dr(lepton, jet))
            && cut(PTREL)->apply(bsm::ptrel(lepton, jet));
}

void Cut2DSelector::enable()
{
    cut(DELTA_R)->enable();
    cut(PTREL)->enable();
}

void Cut2DSelector::disable()
{
    cut(DELTA_R)->disable();
    cut(PTREL)->disable();
}

uint32_t Cut2DSelector::id() const
{
    return core::ID<Cut2DSelector>::get();
}

Cut2DSelector::ObjectPtr Cut2DSelector::clone() const
{
    return ObjectPtr(new Cut2DSelector(*this));
}
