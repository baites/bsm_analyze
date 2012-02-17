// Generator Resonance
//
// Created by Samvel Khalatyan, Feb 17, 2011
// Copyright 2011, All rights reserved

#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/GenParticle.pb.h"

#include "interface/GenResonance.h"

using namespace bsm;
using namespace std;

GenParticleReference::GenParticleReference(const GenParticle *gen_particle)
{
    _gen_particle = gen_particle;
}

const GenParticle *GenParticleReference::gen_particle() const
{
    return _gen_particle;
}



// W-boson
//
Wboson::Wboson(const GenParticle *gen_particle):
    GenParticleReference(gen_particle),
    _is_leptonic_decay(gen_particle->child().size())
{
    // Find out if Wboson decay is leptonic
    //
    typedef ::google::protobuf::RepeatedPtrField<GenParticle> GenParticles;
    for(GenParticles::const_iterator child = gen_particle->child().begin();
            is_leptonic_decay() && gen_particle->child().end() != child;
            ++child)
    {
        switch(abs(child->id()))
        {
            case ELECTRON:      // fall through
            case EL_NEUTRINO:   // fall through
            case MUON:          // fall through
            case MU_NEUTRINO:   // fall through
            case TAU:           // fall through
            case TAU_NEUTRINO:
                break;

            default:
                _is_leptonic_decay = false;
                break;
        }
    }
}

bool Wboson::is_leptonic_decay() const
{
    return _is_leptonic_decay;
}



// Top quark
//
TopQuark::TopQuark(const GenParticle *gen_particle):
    GenParticleReference(gen_particle)
{
    // Find W-boson in the Top quark
    //
    typedef ::google::protobuf::RepeatedPtrField<GenParticle> GenParticles;
    for(GenParticles::const_iterator child = gen_particle->child().begin();
            gen_particle->child().end() != child;
            ++child)
    {
        if (WBOSON == abs(child->id()))
        {
            _wboson.reset(new Wboson(&*child));

            break;
        }
    }
}

const Wboson *TopQuark::wboson() const
{
    return _wboson.get();
}



// Helpers
//
vector<TopQuark> bsm::gen_resonance(const Event *event)
{
    enum
    {
        TOP = 6
    };

    vector<TopQuark> top_quarks;

    typedef ::google::protobuf::RepeatedPtrField<GenParticle> GenParticles;
    for(GenParticles::const_iterator child = event->gen_particle().begin();
            event->gen_particle().end() != child;
            ++child)
    {
        if (TOP == abs(child->id()))
            top_quarks.push_back(TopQuark(&*child));
    }

    return top_quarks;
}
