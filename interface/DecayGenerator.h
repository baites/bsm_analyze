// costruct TTbar Hypotheses given lepton, neutrino and jets
//
// Created by Samvel Khalatyan, Aug 14, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_TTBAR_HYPOTHESIS
#define BSM_TTBAR_HYPOTHESIS

#include <vector>

#include "bsm_input/interface/bsm_input_fwd.h"

namespace bsm
{
    class DecayGenerator
    {
        public:
            typedef std::vector<const Jet *> Jets;

            struct Hypothesis
            {
                Jets leptonic;
                Jets hadronic;
            };

            DecayGenerator();

            void init(const Jets &jets);

            bool next();

            Hypothesis hypothesis() const;

        private:
            bool isValid() const;

            Jets _jets;

            uint32_t _number_of_hypotheses;
            uint32_t _current_hypothesis;
    };
}

#endif
