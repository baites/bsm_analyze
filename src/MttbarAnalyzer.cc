// Mttbar reconstruction with DeltaR selection
//
// Created by Samvel Khalatyan, Jun 07, 2011
// Copyright 2011, All rights reserved

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
    if (_synch_selector->apply(event))
    {
        DecayGenerator generator;
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

void MttbarAnalyzer::print(std::ostream &out) const
{
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
}
