// Generate Template plots and comparison:
//
//  [MC]        hTlep, Mttbar in regions S1, S2, Signal
//  [Data]      hTlep, Mttbar in regions S1, S2
//  [MC/Data]   compare hTlep, Mttbar in S1 + S2
//
// Created by Samvel Khalatyan, Aug 29, 2011
// Copyright 2011, All rights reserved

#include <cfloat>
#include <iostream>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Electron.pb.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Jet.pb.h"
#include "bsm_input/interface/Muon.pb.h"
#include "bsm_input/interface/Physics.pb.h"
#include "bsm_stat/interface/H1.h"
#include "bsm_stat/interface/H2.h"
#include "interface/Algorithm.h"
#include "interface/DecayGenerator.h"
#include "interface/StatProxy.h"
#include "interface/SynchSelector.h"
#include "interface/TemplateAnalyzer.h"

using namespace std;
using namespace boost;

using bsm::TemplateAnalyzer;

TemplateAnalyzer::TemplateAnalyzer()
{
    _synch_selector.reset(new SynchSelector());
    _synch_selector->htlep()->disable();
    _synch_selector->cutflow()->cut(SynchSelector::VETO_SECOND_LEPTON)->objects()->setDelegate(this);
    monitor(_synch_selector);

    _htlep.reset(new H1Proxy(400, 0, 4000));
    monitor(_htlep);

    _mttbar.reset(new H1Proxy(400, 0, 4000));
    monitor(_mttbar);

    _dr_vs_ptrel.reset(new H2Proxy(50, 0, 50, 15, 0, 1.5));
    monitor(_dr_vs_ptrel);
}

TemplateAnalyzer::TemplateAnalyzer(const TemplateAnalyzer &object)
{
    _synch_selector = 
        dynamic_pointer_cast<SynchSelector>(object._synch_selector->clone());
    _synch_selector->cutflow()->cut(SynchSelector::VETO_SECOND_LEPTON)->objects()->setDelegate(this);
    monitor(_synch_selector);

    _htlep = dynamic_pointer_cast<H1Proxy>(object._htlep->clone());
    monitor(_htlep);

    _mttbar = dynamic_pointer_cast<H1Proxy>(object._mttbar->clone());
    monitor(_mttbar);

    _dr_vs_ptrel = dynamic_pointer_cast<H2Proxy>(object._dr_vs_ptrel->clone());
    monitor(_dr_vs_ptrel);
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::htlep() const
{
    return _htlep->histogram();
}

const TemplateAnalyzer::H1Ptr TemplateAnalyzer::mttbar() const
{
    return _mttbar->histogram();
}

const TemplateAnalyzer::H2Ptr TemplateAnalyzer::drVsPtrel() const
{
    return _dr_vs_ptrel->histogram();
}

bsm::JetEnergyCorrectionDelegate *TemplateAnalyzer::getJetEnergyCorrectionDelegate() const
{
    return _synch_selector->getJetEnergyCorrectionDelegate();
}

bsm::SynchSelectorDelegate *TemplateAnalyzer::getSynchSelectorDelegate() const
{
    return _synch_selector.get();
}

bsm::Cut2DSelectorDelegate *TemplateAnalyzer::getCut2DSelectorDelegate() const
{
    return _synch_selector->getCut2DSelectorDelegate();
}

void TemplateAnalyzer::didCounterAdd()
{
    // Secondary lepton veto cut passed
    //
    const LorentzVector &jet_p4 = *_synch_selector->closestJet()->corrected_p4;
    const LorentzVector &lepton_p4 =
        (SynchSelector::ELECTRON == _synch_selector->leptonMode()
            ? (*_synch_selector->goodElectrons().begin())->physics_object().p4()
            : (*_synch_selector->goodMuons().begin())->physics_object().p4());

    drVsPtrel()->fill(ptrel(lepton_p4, jet_p4), dr(lepton_p4, jet_p4));
}

void TemplateAnalyzer::onFileOpen(const std::string &filename, const Input *input)
{
}

void TemplateAnalyzer::process(const Event *event)
{
    if (!event->has_missing_energy())
        return;

    // Process only events, that pass the synch selector
    //
    if (_synch_selector->apply(event))
    {
        // Note: leptons are kept in a vector of pointers
        //
        const LorentzVector &lepton_p4 =
            SynchSelector::ELECTRON == _synch_selector->leptonMode()
            ? (*_synch_selector->goodElectrons().begin())->physics_object().p4()
            : (*_synch_selector->goodMuons().begin())->physics_object().p4();

        htlep()->fill(pt(event->missing_energy().p4()) + pt(lepton_p4));

        if (10 < _synch_selector->goodJets().size())
        {
            clog << _synch_selector->goodJets().size()
                << " good jets are found: skip hypothesis generation" << endl;

            return;
        }

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
        mttbar()->fill(mass_reco);
    }
}

uint32_t TemplateAnalyzer::id() const
{
    return core::ID<TemplateAnalyzer>::get();
}

TemplateAnalyzer::ObjectPtr TemplateAnalyzer::clone() const
{
    return ObjectPtr(new TemplateAnalyzer(*this));
}

void TemplateAnalyzer::print(std::ostream &out) const
{
    out << *_synch_selector << endl;
}
