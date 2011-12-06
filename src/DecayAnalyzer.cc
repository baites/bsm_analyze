// Decay Analyzer
//
// Generate decay table of processed events
//
// Created by Samvel Khalatyan, Jun 29, 2011
// Copyright 2011, All rights reserved

#include <ostream>

#include <boost/pointer_cast.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Electron.pb.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Jet.pb.h"
#include "bsm_input/interface/Muon.pb.h"
#include "bsm_stat/interface/H2.h"
#include "interface/DecayAnalyzer.h"
#include "interface/Selector.h"
#include "interface/StatProxy.h"

using namespace std;

using boost::dynamic_pointer_cast;

using bsm::DecayAnalyzer;

DecayAnalyzer::DecayAnalyzer()
{
    _decay_level_1.reset(new H2Proxy(200, -100, 100, 200, -100, 100));
    _decay_level_2.reset(new H2Proxy(200, -100, 100, 200, -100, 100));
    _decay_level_3.reset(new H2Proxy(200, -100, 100, 200, -100, 100));
    _decay_level_4.reset(new H2Proxy(200, -100, 100, 200, -100, 100));
    _decay_level_5.reset(new H2Proxy(200, -100, 100, 200, -100, 100));

    monitor(_decay_level_1);
    monitor(_decay_level_2);
    monitor(_decay_level_3);
    monitor(_decay_level_4);
    monitor(_decay_level_5);
}

DecayAnalyzer::DecayAnalyzer(const DecayAnalyzer &object)
{
    _decay_level_1.reset(new H2Proxy(*object._decay_level_1));
    _decay_level_2.reset(new H2Proxy(*object._decay_level_2));
    _decay_level_3.reset(new H2Proxy(*object._decay_level_3));
    _decay_level_4.reset(new H2Proxy(*object._decay_level_4));
    _decay_level_5.reset(new H2Proxy(*object._decay_level_5));

    monitor(_decay_level_1);
    monitor(_decay_level_2);
    monitor(_decay_level_3);
    monitor(_decay_level_4);
    monitor(_decay_level_5);
}

void DecayAnalyzer::onFileOpen(const std::string &filename, const Input *)
{
}

void DecayAnalyzer::process(const Event *event)
{
    if (!event->gen_particle().size())
        return;

    genParticles(event->gen_particle());
}

const bsm::H2Ptr DecayAnalyzer::decay_level_1() const
{
    return _decay_level_1->histogram();
}

const bsm::H2Ptr DecayAnalyzer::decay_level_2() const
{
    return _decay_level_2->histogram();
}

const bsm::H2Ptr DecayAnalyzer::decay_level_3() const
{
    return _decay_level_3->histogram();
}

const bsm::H2Ptr DecayAnalyzer::decay_level_4() const
{
    return _decay_level_4->histogram();
}

const bsm::H2Ptr DecayAnalyzer::decay_level_5() const
{
    return _decay_level_5->histogram();
}

uint32_t DecayAnalyzer::id() const
{
    return core::ID<DecayAnalyzer>::get();
}

DecayAnalyzer::ObjectPtr DecayAnalyzer::clone() const
{
    return ObjectPtr(new DecayAnalyzer(*this));
}

void DecayAnalyzer::print(std::ostream &out) const
{
    out << setw(15) << left << " [Decay L1]" << *decay_level_1() << endl;
    out << setw(15) << left << " [Decay L2]" << *decay_level_2() << endl;
    out << setw(15) << left << " [Decay L3]" << *decay_level_3() << endl;
    out << setw(15) << left << " [Decay L4]" << *decay_level_4() << endl;
    out << setw(15) << left << " [Decay L5]" << *decay_level_5();
}

// Privates
//
void DecayAnalyzer::genParticles(const GenParticles &particles,
        const GenParticle *parent,
        const uint32_t &level)
{
    for(GenParticles::const_iterator particle = particles.begin();
            particles.end() != particle;
            ++particle)
    {
        // Skip unstable particles
        //
        if (3 != particle->status())
            continue;

        if (parent)
        {
            H2Ptr histogram;
            switch(level)
            {
                case 1:
                    histogram = decay_level_1();
                    break;

                case 2:
                    histogram = decay_level_2();
                    break;

                case 3:
                    histogram = decay_level_3();
                    break;

                case 4:
                    histogram = decay_level_4();
                    break;

                case 5:
                    histogram = decay_level_5();
                    break;

                default:
                    continue;
            }

            histogram->fill(particle->id(), parent->id());
        }

        if (level < 5)
            genParticles(particle->child(), &*particle, level + 1);
    }
}
