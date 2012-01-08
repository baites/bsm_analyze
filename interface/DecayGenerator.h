// costruct TTbar Hypotheses given lepton, neutrino and jets
//
// Created by Samvel Khalatyan, Aug 14, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_TTBAR_HYPOTHESIS
#define BSM_TTBAR_HYPOTHESIS

#include <cmath>
#include <vector>

#include "bsm_input/interface/bsm_input_fwd.h"

namespace bsm
{
    template<typename T>
    class DecayGenerator
    {
        public:
            typedef std::vector<T> Objects;
            typedef std::vector<typename Objects::const_iterator> Iterators;

            struct Hypothesis
            {
                Iterators leptonic;
                Iterators hadronic;
                Iterators neutral;
            };

            DecayGenerator();

            void init(const Objects &objects);

            bool next();

            Hypothesis hypothesis() const;

        private:
            bool isValid() const;

            Objects _objects;

            uint32_t _number_of_hypotheses;
            uint32_t _current_hypothesis;
    };
}

template<typename T>
bsm::DecayGenerator<T>::DecayGenerator():
    _number_of_hypotheses(0),
    _current_hypothesis(0)
{
}

template<typename T>
void bsm::DecayGenerator<T>::init(const Objects &objects)
{
    _objects = objects;

    _number_of_hypotheses = pow(3.0, static_cast<int>(objects.size()));
    _current_hypothesis = 0;
}

template<typename T>
bool bsm::DecayGenerator<T>::next()
{
    if (!isValid())
        return false;

    ++_current_hypothesis;

    return isValid();
}

template<typename T>
typename bsm::DecayGenerator<T>::Hypothesis bsm::DecayGenerator<T>::hypothesis() const
{
    Hypothesis hypothesis;

    if (isValid())
    {
        uint32_t current_hypothesis = _current_hypothesis;
        for(typename Objects::const_iterator object = _objects.begin();
                _objects.end() != object;
                ++object)
        {
            switch(current_hypothesis % 3)
            {
                case 0: // leptonic
                    hypothesis.leptonic.push_back(object);
                    break;

                case 1: // hadronic
                    hypothesis.hadronic.push_back(object);
                    break;

                default:    // none
                    hypothesis.neutral.push_back(object);
                    break;
            }

            current_hypothesis /= 3;        
        }
    }

    return hypothesis;
}

// Private
//
template<typename T>
bool bsm::DecayGenerator<T>::isValid() const
{
    return _number_of_hypotheses > _current_hypothesis;
}

#endif
