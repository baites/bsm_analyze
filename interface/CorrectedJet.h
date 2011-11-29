// Corrected jet structure: reference to the original jet, subtracted leptons,
// p4 after subtractions and corrected p4.
//
// Created by Samvel Khalatyan, Jul 31, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_CORRECTED_JET
#define BSM_CORRECTED_JET

#include <vector>

#include <boost/shared_ptr.hpp>

#include "bsm_input/interface/bsm_input_fwd.h"

namespace bsm
{
    struct CorrectedJet
    {
        typedef boost::shared_ptr<LorentzVector> LorentzVectorPtr;

        typedef std::vector<const Electron *> Electrons;
        typedef std::vector<const Muon *> Muons;

        CorrectedJet():
            correction(0)
        {
            jet = 0;
        }

        // reference to the origal jet
        //
        const Jet *jet;

        // Corrected jet p4
        //
        LorentzVectorPtr corrected_p4;

        // Corrected MET p4
        //
        LorentzVectorPtr corrected_met;

        // jet uncorrected p4 after leptons subtraction
        //
        LorentzVectorPtr subtracted_p4;

        // electrons that were subtracted
        //
        Electrons subtracted_electrons;

        // muons that were subtracted
        //
        Muons subtracted_muons;

        // Jet energy correction that was applied
        //
        float correction;
    };
}

#endif
