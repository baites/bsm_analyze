// Generator Resonance
//
// Created by Samvel Khalatyan, Feb 17, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_GEN_RESONANCE
#define BSM_GEN_RESONANCE

#include <vector>

#include <boost/shared_ptr.hpp>

#include "bsm_input/interface/bsm_input_fwd.h"

namespace bsm
{
    class GenParticleReference
    {
        public:
            GenParticleReference(const GenParticle *);

            const GenParticle *gen_particle() const;

        private:
            const GenParticle *_gen_particle;
    };

    class Wboson: public GenParticleReference
    {
        public:
            Wboson(const GenParticle *);

            bool is_leptonic_decay() const;

        private:
            bool _is_leptonic_decay;

            enum
            {
                ELECTRON = 11,
                EL_NEUTRINO = 12,
                MUON = 13,
                MU_NEUTRINO = 14,
                TAU = 15,
                TAU_NEUTRINO = 16
            };
    };

    class TopQuark: public GenParticleReference
    {
        public:
            TopQuark(const GenParticle *);

            const Wboson *wboson() const;

        private:
            boost::shared_ptr<Wboson> _wboson;

            enum
            {
                TOP = 6,
                WBOSON = 24
            };
    };

    std::vector<TopQuark> gen_resonance(const Event *);
}

#endif
