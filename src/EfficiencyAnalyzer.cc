// Efficiency Plots
//
// Apply lepton removal to jets, Jet Energy Corrections, select events
// based on the corrected P4, leptons and then aaply 2D cut:
//      DeltaR > CUT
//      pT,rel > CUT
//
// between the muon/electron and closest jet
//
// Created by Samvel Khalatyan, Jul 19, 2011
// Copyright 2011, All rights reserved

#include <ostream>

#include <boost/pointer_cast.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Event.pb.h"
#include "interface/Selector.h"
#include "interface/SynchSelector.h"
#include "interface/EfficiencyAnalyzer.h"

using namespace std;

using boost::dynamic_pointer_cast;

using bsm::EfficiencyAnalyzer;

// Synch with Jet Energy corrections
//
EfficiencyAnalyzer::EfficiencyAnalyzer()
{
    _synch_selector.reset(new SynchSelector());
    monitor(_synch_selector);
}

EfficiencyAnalyzer::EfficiencyAnalyzer(const EfficiencyAnalyzer &object)
{
    _synch_selector = 
        dynamic_pointer_cast<SynchSelector>(object._synch_selector->clone());
    monitor(_synch_selector);
}

bsm::JetEnergyCorrectionDelegate *EfficiencyAnalyzer::getJetEnergyCorrectionDelegate() const
{
    return _synch_selector->getJetEnergyCorrectionDelegate();
}

bsm::SynchSelectorDelegate *EfficiencyAnalyzer::getSynchSelectorDelegate() const
{
    return _synch_selector.get();
}

void EfficiencyAnalyzer::onFileOpen(const std::string &filename, const Input *)
{
}

void EfficiencyAnalyzer::process(const Event *event)
{
    _synch_selector->apply(event);
}

uint32_t EfficiencyAnalyzer::id() const
{
    return core::ID<EfficiencyAnalyzer>::get();
}

EfficiencyAnalyzer::ObjectPtr EfficiencyAnalyzer::clone() const
{
    return ObjectPtr(new EfficiencyAnalyzer(*this));
}

void EfficiencyAnalyzer::print(std::ostream &out) const
{
    out << *_synch_selector << endl;

    SynchSelector::CutflowPtr cutflow = _synch_selector->cutflow();

    const uint32_t good_leptons =
        *(cutflow->cut(SynchSelector::HTLEP)->objects());

    out << _synch_selector->cutMode() << " Efficiency: "
        << (good_leptons
            ? (1.0 * *(cutflow->cut(SynchSelector::CUT_LEPTON)->objects()))
                / good_leptons
            : 0)
        << endl;
}
