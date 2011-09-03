// Mttbar reconstruction with DeltaR selection
//
// Created by Samvel Khalatyan, Jun 07, 2011
// Copyright 2011, All rights reserved

#include <cfloat>
#include <iomanip>
#include <iostream>
#include <ostream>

#include <boost/pointer_cast.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Electron.pb.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Jet.pb.h"
#include "bsm_input/interface/Muon.pb.h"
#include "bsm_stat/interface/H1.h"
#include "bsm_stat/interface/H2.h"
#include "interface/Algorithm.h"
#include "interface/Cut.h"
#include "interface/DecayGenerator.h"
#include "interface/Monitor.h"
#include "interface/Selector.h"
#include "interface/SynchSelector.h"
#include "interface/StatProxy.h"
#include "interface/Utility.h"
#include "interface/MttbarAnalyzer.h"

using namespace std;

using boost::dynamic_pointer_cast;

using bsm::MttbarAnalyzer;
using bsm::MttbarOptions;

using bsm::stat::H1;
using bsm::stat::H2;

// Synch Analyzer Options
//
MttbarOptions::MttbarOptions()
{
    _delegate = 0;

    _description.reset(new po::options_description("Mttbar Options"));
    _description->add_options()
        ("use-gen-mass",
         po::value<bool>()->implicit_value(false)->notifier(
             boost::bind(&MttbarOptions::setUseGeneratorMass, this, _1)),
         "Uge ttbar gnerator mass")
    ;
}

MttbarOptions::~MttbarOptions()
{
}

void MttbarOptions::setDelegate(MttbarDelegate *delegate)
{
    if (_delegate != delegate)
        _delegate = delegate;
}

bsm::MttbarDelegate *MttbarOptions::delegate() const
{
    return _delegate;
}

MttbarOptions::DescriptionPtr MttbarOptions::description() const
{
    return _description;
}

// Private
//
void MttbarOptions::setUseGeneratorMass(const bool &flag) const
{
    if (_delegate)
        _delegate->setUseGeneratorMass(flag);
}



// Mttbar Analyzer
//
MttbarAnalyzer::MttbarAnalyzer():
    _use_generator_mass(false)
{
    _synch_selector.reset(new SynchSelector());
    monitor(_synch_selector);

    _missing_energy_monitor.reset(new LorentzVectorMonitor());
    _lwboson_monitor.reset(new LorentzVectorMonitor());
    _ltop_monitor.reset(new LorentzVectorMonitor());
    _htop_monitor.reset(new LorentzVectorMonitor());

    _top_delta_monitor.reset(new DeltaMonitor());

    _mreco.reset(new H1Proxy(400, 0, 4000));
    _mltop_vs_mhtop.reset(new H2Proxy(400, 0, 4000, 400, 0, 4000));

    _mgen.reset(new H1Proxy(400, 0, 4000));
    _mreco_minus_mgen.reset(new H1Proxy(200, -100, 100));
    _mreco_vs_mgen.reset(new H2Proxy(400, 0, 4000, 400, 0, 4000));

    monitor(_missing_energy_monitor);
    monitor(_lwboson_monitor);
    monitor(_ltop_monitor);
    monitor(_htop_monitor);

    monitor(_top_delta_monitor);

    monitor(_mreco);
    monitor(_mltop_vs_mhtop);

    monitor(_mgen);
    monitor(_mreco_minus_mgen);
    monitor(_mreco_vs_mgen);

    _gen_events.reset(new Counter());
    monitor(_gen_events);
}

MttbarAnalyzer::MttbarAnalyzer(const MttbarAnalyzer &object):
    _use_generator_mass(object._use_generator_mass)
{
    _synch_selector = 
        dynamic_pointer_cast<SynchSelector>(object._synch_selector->clone());
    monitor(_synch_selector);

    _missing_energy_monitor =
        dynamic_pointer_cast<LorentzVectorMonitor>(object._missing_energy_monitor->clone());

    _lwboson_monitor =
        dynamic_pointer_cast<LorentzVectorMonitor>(object._lwboson_monitor->clone());

    _ltop_monitor =
        dynamic_pointer_cast<LorentzVectorMonitor>(object._ltop_monitor->clone());
    _htop_monitor =
        dynamic_pointer_cast<LorentzVectorMonitor>(object._htop_monitor->clone());

    _top_delta_monitor =
        dynamic_pointer_cast<DeltaMonitor>(object._top_delta_monitor->clone());

    _mreco = dynamic_pointer_cast<H1Proxy>(object._mreco->clone());

    _mltop_vs_mhtop =
        dynamic_pointer_cast<H2Proxy>(object._mltop_vs_mhtop->clone());

    _mgen = dynamic_pointer_cast<H1Proxy>(object._mgen->clone());

    _mreco_minus_mgen = dynamic_pointer_cast<H1Proxy>(object._mreco_minus_mgen->clone());

    _mreco_vs_mgen =
        dynamic_pointer_cast<H2Proxy>(object._mreco_vs_mgen->clone());

    monitor(_missing_energy_monitor);
    monitor(_lwboson_monitor);
    monitor(_ltop_monitor);
    monitor(_htop_monitor);

    monitor(_top_delta_monitor);

    monitor(_mreco);
    monitor(_mltop_vs_mhtop);

    monitor(_mgen);
    monitor(_mreco_minus_mgen);
    monitor(_mreco_vs_mgen);

    _gen_events =
        dynamic_pointer_cast<Counter>(object._gen_events->clone());
    monitor(_gen_events);
}

bsm::JetEnergyCorrectionDelegate *MttbarAnalyzer::getJetEnergyCorrectionDelegate() const
{
    return _synch_selector->getJetEnergyCorrectionDelegate();
}

bsm::SynchSelectorDelegate *MttbarAnalyzer::getSynchSelectorDelegate() const
{
    return _synch_selector.get();
}

const MttbarAnalyzer::H1Ptr MttbarAnalyzer::mreco() const
{
    return _mreco->histogram();
}

const MttbarAnalyzer::H2Ptr MttbarAnalyzer::mltopVsMhtop() const
{
    return _mltop_vs_mhtop->histogram();
}

const MttbarAnalyzer::H1Ptr MttbarAnalyzer::mgen() const
{
    return _mgen->histogram();
}

const MttbarAnalyzer::H1Ptr MttbarAnalyzer::mrecoMinusMgen() const
{
    return _mreco_minus_mgen->histogram();
}

const MttbarAnalyzer::H2Ptr MttbarAnalyzer::mrecoVsMgen() const
{
    return _mreco_vs_mgen->histogram();
}

const MttbarAnalyzer::P4MonitorPtr MttbarAnalyzer::missingEnergyMonitor() const
{
    return _missing_energy_monitor;
}

const MttbarAnalyzer::P4MonitorPtr MttbarAnalyzer::lwbosonMonitor() const
{
    return _lwboson_monitor;
}

const MttbarAnalyzer::P4MonitorPtr MttbarAnalyzer::ltopMonitor() const
{
    return _ltop_monitor;
}

const MttbarAnalyzer::P4MonitorPtr MttbarAnalyzer::htopMonitor() const
{
    return _htop_monitor;
}

const MttbarAnalyzer::DeltaMonitorPtr MttbarAnalyzer::topDeltaMonitor() const
{
    return _top_delta_monitor;
}

void MttbarAnalyzer::setUseGeneratorMass(const bool &flag)
{
    _use_generator_mass = flag;
}

void MttbarAnalyzer::onFileOpen(const std::string &filename, const Input *input)
{
}

void MttbarAnalyzer::process(const Event *event)
{
    if (!event->has_missing_energy())
        return;

    float mass_gen = 0;
    if (_use_generator_mass)
    {
        mass_gen = getMttbarGen(event);

        if (!mass_gen)
            return;

        _gen_events->add();
    }

    // Process only events, that pass the synch selector
    //
    if (_synch_selector->apply(event))
    {
        if (10 < _synch_selector->goodJets().size())
        {
            _out << _synch_selector->goodJets().size()
                << " good jets are found: skip hypothesis generation" << endl;

            return;
        }

        // Note: leptons are kept in a vector of pointers
        //
        const LorentzVector &lepton_p4 =
            SynchSelector::ELECTRON == _synch_selector->leptonMode()
            ? (*_synch_selector->goodElectrons().begin())->physics_object().p4()
            : (*_synch_selector->goodMuons().begin())->physics_object().p4();

        // Reconstruct the neutrino pZ and keep solutions in vector for later
        // use
        //
        NeutrinoReconstruct neutrinoReconstruct;
        NeutrinoReconstruct::Solutions neutrinos =
            neutrinoReconstruct(lepton_p4, event->missing_energy().p4());

        // Prepare generator and loop over all hypotheses of the decay
        // (different jets assignment to leptonic/hadronic legs)
        //
        typedef DecayGenerator<SynchSelector::CorrectedJet> Generator;
        Generator generator;
        generator.init(_synch_selector->goodJets());

        // Best Solution should have minimun value of the DeltaRmin:
        //
        //  DeltaRmin = DeltaR(ltop, b) + DeltaR(ltop, l) + DeltaR(ltop, nu)
        //
        // and maximum value of the DeltaR between leptonic and hadronic
        // tops in case the same DeltaRmin is found:
        //
        //  DeltaRlh = DeltaR(ltop, htop)
        //
        struct Solution
        {
            Solution(): deltaRmin(FLT_MAX), deltaRlh(0)
            {
            }

            LorentzVector ltop; // Reconstructed leptonic leg
            LorentzVector htop; // Reconstructed hadronic leg
            LorentzVector missing_energy;

            float deltaRmin;
            float deltaRlh;
        } best_solution;

        // Loop over all possible hypotheses and pick the best one
        // Note: take into account all reconstructed neutrino solutions
        //
        do
        {
            Generator::Hypothesis hypothesis = generator.hypothesis();

            // Skip hypotheses that do not have any leptonic or hadronic jets
            //
            if (hypothesis.leptonic.empty()
                    || hypothesis.hadronic.empty())
                continue;

            // Leptonic Top p4 = leptonP4 + nuP4 + bP4
            // where bP4 is the hardest jet (highest pT)
            //
            LorentzVector ltop = lepton_p4;

            const SynchSelector::CorrectedJet *hardest_jet = 0;
            float highest_pt = 0;

            // Select the hardest jet (highest pT)
            // Note: hypothesis keeps vector of iterators to Correcte Jets.
            //       Corrected jet has a pointer to the original jet and
            //       corrected P4
            //
            for(Generator::Iterators::const_iterator jet =
                        hypothesis.leptonic.begin();
                    hypothesis.leptonic.end() != jet;
                    ++jet)
            {
                const float jet_pt = pt(*(*jet)->corrected_p4);
                if (jet_pt > highest_pt)
                    hardest_jet = &*(*jet);
            }

            ltop += *hardest_jet->corrected_p4;

            // the neutrino will be taken into account later
            //

            // htop is a sum of all jet p4s assigned to the hadronic leg
            //
            LorentzVector htop;
            for(Generator::Iterators::const_iterator jet =
                        hypothesis.hadronic.begin();
                    hypothesis.hadronic.end() != jet;
                    ++jet)
            {
                htop += *(*jet)->corrected_p4;
            }

            // Take into account all neutrino solutions. Solutions are kept in
            // a vector of pointer
            //
            for(NeutrinoReconstruct::Solutions::const_iterator neutrino =
                        neutrinos.begin();
                    neutrinos.end() != neutrino;
                    ++neutrino)
            {
                const LorentzVector &neutrino_p4 = *(*neutrino);

                LorentzVector ltop_tmp = ltop;
                ltop_tmp += neutrino_p4;

                const float deltaRmin = dr(ltop_tmp, *hardest_jet->corrected_p4)
                    + dr(ltop_tmp, lepton_p4)
                    + dr(ltop_tmp, neutrino_p4);

                const float deltaRlh = dr(ltop_tmp, htop);

                if (deltaRmin < best_solution.deltaRmin
                        || (deltaRmin == best_solution.deltaRmin
                            && deltaRlh > best_solution.deltaRlh))
                {
                    best_solution.deltaRmin = deltaRmin;
                    best_solution.deltaRlh = deltaRlh;
                    best_solution.ltop = ltop_tmp;
                    best_solution.htop = htop;
                    best_solution.missing_energy = neutrino_p4;
                }
            }
        }
        while(generator.next());

        // Best Solution is found
        //
        const float mass_reco = mass(best_solution.ltop + best_solution.htop);
        mreco()->fill(mass_reco);
        mltopVsMhtop()->fill(mass(best_solution.ltop), mass(best_solution.htop));

        missingEnergyMonitor()->fill(best_solution.missing_energy);
        lwbosonMonitor()->fill(lepton_p4 + best_solution.missing_energy);
        ltopMonitor()->fill(best_solution.ltop);
        htopMonitor()->fill(best_solution.htop);
        topDeltaMonitor()->fill(best_solution.ltop, best_solution.htop);

        if (_use_generator_mass)
        {
            mgen()->fill(mass_gen);
            mrecoMinusMgen()->fill((mass_reco - mass_gen) / mass_gen);
            mrecoVsMgen()->fill(mass_reco, mass_gen);
        }
    }
}

uint32_t MttbarAnalyzer::id() const
{
    return core::ID<MttbarAnalyzer>::get();
}

MttbarAnalyzer::ObjectPtr MttbarAnalyzer::clone() const
{
    return ObjectPtr(new MttbarAnalyzer(*this));
}

void MttbarAnalyzer::merge(const ObjectPtr &pointer)
{
    if (pointer->id() != id())
        return;

    boost::shared_ptr<MttbarAnalyzer> object =
        dynamic_pointer_cast<MttbarAnalyzer>(pointer);

    if (!object)
        return;

    Object::merge(pointer);

    if (!object->_out.str().empty())
        _out << object->_out.str() << endl;
}

void MttbarAnalyzer::print(std::ostream &out) const
{
    out << *_synch_selector << endl;

    out << "Gen Events " << _synch_selector->leptonMode()
        << "+jets before any cuts" << endl;
    out << *_gen_events << endl;
    out << endl;

    out << "Missing Energy Monitor" << endl;
    out << *_missing_energy_monitor << endl;
    out << endl;

    out << "Leptonic Wboson" << endl;
    out << *_lwboson_monitor << endl;
    out << endl;

    out << "Leptonic Top monitor" << endl;
    out << *_ltop_monitor << endl;
    out << endl;

    out << "Hadronic Top monitor" << endl;
    out << *_htop_monitor << endl;
    out << endl;

    out << "Top Delta Monitor" << endl;
    out << *_top_delta_monitor << endl;
    out << endl;

    out << "Mttbar" << endl;
    out << *mreco() << endl;
    out << endl;

    out << "Mltop vs Mhtop" << endl;
    out << *mltopVsMhtop() << endl;

    out << _out.str();
}

// Private
//
float MttbarAnalyzer::getMttbarGen(const Event *event)
{
    float mass_gen = 0;

    const GenParticles &particles = event->gen_particles();
    // Search for the first top
    //
    const GenParticles::const_iterator top = find(particles, TOP);
    if (particles.end() != top)
    {
        // Search for the second top
        //
        GenParticles::const_iterator second_top = top;
        second_top = find(particles, TOP, ++second_top);
        if (particles.end() != second_top)
        {
            const bool is_leptonic = isLeptonicDecay(*top);
            const bool is_second_leptonic = isLeptonicDecay(*second_top);
            if ((is_leptonic
                    || is_second_leptonic)
                    && !(is_leptonic
                        && is_second_leptonic))
            {
                mass_gen = mass(top->physics_object().p4()
                        + second_top->physics_object().p4());
            }
        }
    }

    return mass_gen;
}

MttbarAnalyzer::GenParticles::const_iterator
    MttbarAnalyzer::find(const GenParticles &particles,
            const uint32_t &id)
{
    return find(particles, id, particles.begin());
}

MttbarAnalyzer::GenParticles::const_iterator
    MttbarAnalyzer::find(const GenParticles &particles,
            const uint32_t &id,
            const GenParticles::const_iterator &from)
{
    for(GenParticles::const_iterator particle = from;
            particles.end() != particle;
            ++particle)
    {
        if (3 == particle->status()
                && id == static_cast<uint32_t>(abs(particle->id())))
            return particle;
    }

    return particles.end();
}

bool MttbarAnalyzer::isLeptonicDecay(const GenParticle &particle)
{
    const GenParticles::const_iterator wboson = find(particle.children(), WBOSON);
    if (particle.children().end() == wboson)
        return false;

    const GenParticles::const_iterator lepton = find(wboson->children(),
        SynchSelector::ELECTRON == _synch_selector->leptonMode()
        ? ELECTRON
        : MUON);
    return wboson->children().end() != lepton;
}
