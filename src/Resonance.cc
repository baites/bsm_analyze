// Generator Resonance
//
// Created by Samvel Khalatyan, Feb 16, 2012
// Copyright 2011, All rights reserved

#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/GenParticle.pb.h"

#include "interface/Resonance.h"

using namespace bsm;

GenResonance::GenResonance(const Event *event)
{
    htop = 0;
    hjet = 0;
    whad = 0;

    ltop = 0;
    ljet = 0;
    wlep = 0;
    lepton = 0;
    neutrino = 0;

    lepton_type = UNKNOWN;

    typedef ::google::protobuf::RepeatedPtrField<GenParticle> GenParticles;
    for(GenParticles::const_iterator particle = event->gen_particle().begin(),
                end = event->gen_particle().end();
            end != particle;
            ++particle)
    {
        if (TOP == abs(particle->id()))
            process_top(*particle);
    }
}

void GenResonance::process_top(const GenParticle &particle)
{
    const LorentzVector *jet_p4 = 0;
    bool is_ltop = false;

    typedef ::google::protobuf::RepeatedPtrField<GenParticle> GenParticles;
    for(GenParticles::const_iterator child = particle.child().begin(),
                end = particle.child().end();
            end != child;
            ++child)
    {
        if (WBOSON == abs(child->id()))
        {
            if (is_ltop = is_leptonic_wdecay(*child))
                wlep = &child->physics_object().p4();
            else
                whad = &child->physics_object().p4();
        }
        else
            jet_p4 = &child->physics_object().p4();
    }

    if (is_ltop)
    {
        ltop = &particle.physics_object().p4();
        ljet = jet_p4;
    }
    else
    {
        htop = &particle.physics_object().p4();
        hjet = jet_p4;
    }
}

bool GenResonance::is_leptonic_wdecay(const GenParticle &particle)
{
    bool result = false;

    typedef ::google::protobuf::RepeatedPtrField<GenParticle> GenParticles;
    for(GenParticles::const_iterator child = particle.child().begin(),
                end = particle.child().end();
            end != child;
            ++child)
    {
        switch(abs(child->id()))
        {
            case ELECTRON:
            case MUON:
            case TAU:
                lepton_type = Lepton(abs(child->id()));
                lepton = &child->physics_object().p4();

                result = true;
                break;

            case EL_NEUTRINO:
            case MU_NEUTRINO:
            case TAU_NEUTRINO:
                neutrino = &child->physics_object().p4();

                result = true;
                break;

            default:
                whtop_jets.push_back(&child->physics_object().p4());
        }
    }

    return result;
}
