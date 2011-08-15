// costruct TTbar Hypotheses given lepton, neutrino and jets
//
// Created by Samvel Khalatyan, Aug 14, 2011
// Copyright 2011, All rights reserved

/*
#include <cmath>

#include "interface/DecayGenerator.h"

using namespace std;

using bsm::DecayGenerator;

DecayGenerator::DecayGenerator():
    _number_of_hypotheses(0),
    _current_hypothesis(0)
{
}

void DecayGenerator::init(const Jets &jets)
{
    _jets = jets;

    _number_of_hypotheses = pow(3.0, static_cast<int>(jets.size()));
    _current_hypothesis = 0;
}

bool DecayGenerator::next()
{
    if (!isValid())
        return false;

    ++_current_hypothesis;

    return isValid();
}

DecayGenerator::Hypothesis DecayGenerator::hypothesis() const
{
    Hypothesis hypothesis;

    if (isValid())
    {
        uint32_t current_hypothesis = _current_hypothesis;
        for(Jets::const_iterator jet = _jets.begin();
                _jets.end() != jet;
                ++jet)
        {
            switch(current_hypothesis % 3)
            {
                case 0: // hadronic
                    hypothesis.leptonic.push_back(*jet);
                    break;

                case 1: // leptonic
                    hypothesis.hadronic.push_back(*jet);
                    break;

                default:    // none
                    break;
            }

            current_hypothesis /= 3;        
        }
    }

    return hypothesis;
}

// Private
//
bool DecayGenerator::isValid() const
{
    return _number_of_hypotheses > _current_hypothesis;
}
*/
