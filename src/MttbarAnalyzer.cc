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
#include "interface/Algorithm.h"
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

using bsm::stat::H1;

MttbarAnalyzer::MttbarAnalyzer()
{
    _synch_selector.reset(new SynchSelector());
    monitor(_synch_selector);

    _ltop_monitor.reset(new LorentzVectorMonitor());
    _htop_monitor.reset(new LorentzVectorMonitor());

    _top_delta_monitor.reset(new DeltaMonitor());

    _mttbar.reset(new H1Proxy(25, 500, 3000));

    monitor(_ltop_monitor);
    monitor(_htop_monitor);

    monitor(_top_delta_monitor);

    monitor(_mttbar);
}

MttbarAnalyzer::MttbarAnalyzer(const MttbarAnalyzer &object)
{
    _synch_selector = 
        dynamic_pointer_cast<SynchSelector>(object._synch_selector->clone());
    monitor(_synch_selector);

    _ltop_monitor =
        dynamic_pointer_cast<LorentzVectorMonitor>(object._ltop_monitor->clone());
    _htop_monitor =
        dynamic_pointer_cast<LorentzVectorMonitor>(object._htop_monitor->clone());

    _top_delta_monitor =
        dynamic_pointer_cast<DeltaMonitor>(object._top_delta_monitor->clone());

    _mttbar = dynamic_pointer_cast<H1Proxy>(object._mttbar->clone());

    monitor(_ltop_monitor);
    monitor(_htop_monitor);

    monitor(_top_delta_monitor);

    monitor(_mttbar);
}

bsm::JetEnergyCorrectionDelegate *MttbarAnalyzer::getJetEnergyCorrectionDelegate() const
{
    return _synch_selector->getJetEnergyCorrectionDelegate();
}

bsm::SynchSelectorDelegate *MttbarAnalyzer::getSynchSelectorDelegate() const
{
    return _synch_selector.get();
}

const MttbarAnalyzer::H1Ptr MttbarAnalyzer::mttbar() const
{
    return _mttbar->histogram();
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

void MttbarAnalyzer::onFileOpen(const std::string &filename, const Input *input)
{
}

void MttbarAnalyzer::process(const Event *event)
{
    if (!event->has_missing_energy())
        return;

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
                    
                }
            }
        }
        while(generator.next());

        // Best Solution is found
        //
        mttbar()->fill(mass(best_solution.ltop + best_solution.htop));
        ltopMonitor()->fill(best_solution.ltop);
        htopMonitor()->fill(best_solution.htop);
        topDeltaMonitor()->fill(best_solution.ltop, best_solution.htop);
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
    out << *mttbar() << endl;

    out << _out.str();
}
