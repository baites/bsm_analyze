// PileUp re-weighting for Summer11 MC/Data
//
// Created by Samvel Khalatyan, Oct 21, 2011
// Copyright 2011, All rights reserved

#include "bsm_input/interface/Event.pb.h"
#include "interface/PileupCorrections.h"

using namespace bsm;

PileupCorrections::PileupCorrections()
{
    _scale.push_back(0);
    _scale.push_back(0.216263);
    _scale.push_back(0.731377);
    _scale.push_back(1.1416);
    _scale.push_back(1.4592);
    _scale.push_back(1.58665);
    _scale.push_back(1.53217);
    _scale.push_back(1.37603);
    _scale.push_back(1.15313);
    _scale.push_back(0.950982);
    _scale.push_back(0.753041);
    _scale.push_back(0.591861);
    _scale.push_back(0.453312);
    _scale.push_back(0.344267);
    _scale.push_back(0.256831);
    _scale.push_back(0.187768);
    _scale.push_back(0.147568);
    _scale.push_back(0.110464);
    _scale.push_back(0.0824476);
    _scale.push_back(0.0645739);
    _scale.push_back(0.0480766);
    _scale.push_back(0.0308829);
    _scale.push_back(0.0248396);
    _scale.push_back(0.0160992);
    _scale.push_back(0.0171581);
}

const float PileupCorrections::scale(const Event *event) const
{
    return static_cast<std::size_t>(event->primary_vertex().size()) < _scale.size()
        ? _scale[event->primary_vertex().size()]
        : 0;
}
