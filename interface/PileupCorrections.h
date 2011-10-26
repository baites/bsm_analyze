// PileUp re-weighting for Summer11 MC/Data
//
// Created by Samvel Khalatyan, Oct 21, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_PILEUP_CORRECTIONS
#define BSM_PILEUP_CORRECTIONS

#include <vector>

#include "bsm_input/interface/bsm_input_fwd.h"

namespace bsm
{
    class PileupCorrections
    {
        public:
            PileupCorrections();

            const float scale(const Event *) const;

        private:
            std::vector<float> _scale;
    };
}

#endif
