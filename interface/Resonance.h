// Generator Resonance
//
// Created by Samvel Khalatyan, Feb 16, 2012
// Copyright 2011, All rights reserved

#ifndef BSM_RESONANCE
#define BSM_RESONANCE

#include "bsm_input/interface/bsm_input_fwd.h"

namespace bsm
{
    struct GenResonance
    {
        typedef std::vector<const LorentzVector *> LorentzVectors;

        enum Lepton
        {
            UNKNOWN = 0,
            ELECTRON = 11,
            MUON = 13,
            TAU = 15
        };

        GenResonance(const Event *);

        // Hadronic top
        const LorentzVector *htop;
        const LorentzVector *hjet;
        const LorentzVector *whad;
        LorentzVectors whtop_jets;

        // Leptonic top
        const LorentzVector *ltop;
        const LorentzVector *ljet;
        const LorentzVector *wlep;
        const LorentzVector *lepton;
        const LorentzVector *neutrino;

        Lepton lepton_type;

        private:
            enum {
                BOTTOM = 5,
                TOP = 6,
                EL_NEUTRINO = 12,
                MU_NEUTRINO = 14,
                TAU_NEUTRINO = 16,
                WBOSON = 24
            };

            void process_top(const GenParticle &);

            bool is_leptonic_wdecay(const GenParticle &);
    };
}

#endif
