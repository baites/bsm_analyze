// Jet Energy Correction Analyzer
//
// Given Jet uncorrected P4 apply Jet Energy Corrections and
// plot comparison histograms of the corrected in CMSSW P4 and offline
// corrections applied P4
//
// Created by Samvel Khalatyan, Jul 12, 2011
// Copyright 2011, All rights reserved

#include <ostream>
#include <string>
#include <vector>

#include <boost/pointer_cast.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Event.pb.h"
#include "interface/JetEnergyCorrections.h"
#include "interface/JetEnergyCorrectionsAnalyzer.h"
#include "interface/Monitor.h"
#include "interface/Selector.h"

using namespace std;

using boost::dynamic_pointer_cast;

using bsm::JetEnergyCorrectionsAnalyzer;
using bsm::JetEnergyCorrectionDelegate;

JetEnergyCorrectionsAnalyzer::JetEnergyCorrectionsAnalyzer()
{
    // Jet Energy Corrections
    //
    _jec.reset(new JetEnergyCorrections());
    monitor(_jec);

    // Selectrors
    //
    _jet_selector.reset(new JetSelector());
    monitor(_jet_selector);

    // Monitors
    //
    _jet_cmssw_corrected_p4.reset(new LorentzVectorMonitor());
    _jet_uncorrected_p4.reset(new LorentzVectorMonitor());
    _jet_offline_corrected_p4.reset(new LorentzVectorMonitor());

    monitor(_jet_cmssw_corrected_p4);
    monitor(_jet_uncorrected_p4);
    monitor(_jet_offline_corrected_p4);
}

JetEnergyCorrectionsAnalyzer::JetEnergyCorrectionsAnalyzer(const JetEnergyCorrectionsAnalyzer &object)
{
    // Jet Energy Corrections
    //
    _jec = dynamic_pointer_cast<JetEnergyCorrections>(object._jec->clone());
    monitor(_jec);

    // Selectors
    //
    _jet_selector = 
        dynamic_pointer_cast<JetSelector>(object._jet_selector->clone());

    monitor(_jet_selector);

    // Monitors
    //
    // Monitors
    //
    _jet_cmssw_corrected_p4 =
        dynamic_pointer_cast<LorentzVectorMonitor>(object._jet_cmssw_corrected_p4->clone());

    _jet_uncorrected_p4 =
        dynamic_pointer_cast<LorentzVectorMonitor>(object._jet_uncorrected_p4->clone());

    _jet_offline_corrected_p4 =
        dynamic_pointer_cast<LorentzVectorMonitor>(object._jet_offline_corrected_p4->clone());

    monitor(_jet_cmssw_corrected_p4);
    monitor(_jet_uncorrected_p4);
    monitor(_jet_offline_corrected_p4);
}

JetEnergyCorrectionDelegate *JetEnergyCorrectionsAnalyzer::getJetEnergyCorrectionDelegate() const
{
    return _jec.get();
}

const JetEnergyCorrectionsAnalyzer::P4MonitorPtr
    JetEnergyCorrectionsAnalyzer::jetCmsswCorrectedP4() const
{ 
    return _jet_cmssw_corrected_p4;
}

const JetEnergyCorrectionsAnalyzer::P4MonitorPtr
    JetEnergyCorrectionsAnalyzer::jetUncorrectedP4() const
{
    return _jet_uncorrected_p4;
}

const JetEnergyCorrectionsAnalyzer::P4MonitorPtr
    JetEnergyCorrectionsAnalyzer::jetOfflineCorrectedP4() const
{
    return _jet_offline_corrected_p4;
}

void JetEnergyCorrectionsAnalyzer::onFileOpen(const std::string &filename, const Input *)
{
}

void JetEnergyCorrectionsAnalyzer::process(const Event *event)
{
    _out << "Event: " << event->extra().id()
        << " Lumi: " << event->extra().lumi()
        << " Run: " << event->extra().run() << endl;

    jets(event);
}

uint32_t JetEnergyCorrectionsAnalyzer::id() const
{
    return core::ID<JetEnergyCorrectionsAnalyzer>::get();
}

JetEnergyCorrectionsAnalyzer::ObjectPtr JetEnergyCorrectionsAnalyzer::clone() const
{
    return ObjectPtr(new JetEnergyCorrectionsAnalyzer(*this));
}

void JetEnergyCorrectionsAnalyzer::merge(const ObjectPtr &pointer)
{
    if (id() != pointer->id());

    boost::shared_ptr<JetEnergyCorrectionsAnalyzer> object =
        dynamic_pointer_cast<JetEnergyCorrectionsAnalyzer>(pointer);

    if (!object)
        return;

    Object::merge(pointer);

    _out << endl;
    _out << object->_out.str();
}

void JetEnergyCorrectionsAnalyzer::print(std::ostream &out) const
{
    out << _out.str();
}

// Private
//
void JetEnergyCorrectionsAnalyzer::jets(const Event *event)
{
    if (!event->jets().size()
            || !event->has_extra())
        return;

    _out << "Jets" << endl;

    typedef ::google::protobuf::RepeatedPtrField<Jet> Jets;

    JetEnergyCorrections::Electrons electrons;
    typedef ::google::protobuf::RepeatedPtrField<Electron> Electrons;
    for(Electrons::const_iterator electron = event->pf_electrons().begin();
            event->pf_electrons().end() != electron;
            ++electron)
    {
        electrons.push_back(&*electron);
    }

    JetEnergyCorrections::Muons muons;
    typedef ::google::protobuf::RepeatedPtrField<Muon> Muons;
    for(Muons::const_iterator muon = event->pf_muons().begin();
            event->pf_muons().end() != muon;
            ++muon)
    {
        muons.push_back(&*muon);
    }

    LockSelectorEventCounterOnUpdate lock(*_jet_selector);
    uint32_t id = 1;
    for(Jets::const_iterator jet = event->jets().begin();
            event->jets().end() != jet;
            ++jet, ++id)
    {
        if (!jet->has_extra())
            continue;

        if (_jet_selector->apply(*jet))
        {
            const LorentzVector &cmssw_p4 = jet->physics_object().p4();
            _jet_cmssw_corrected_p4->fill(cmssw_p4);

            const LorentzVector &uncorrected_p4 = jet->uncorrected_p4();
            _jet_uncorrected_p4->fill(uncorrected_p4);

            LorentzVectorPtr corrected_p4 = _jec->correctJet(&*jet, event, electrons, muons);
            if (!corrected_p4)
                continue;

            _jet_offline_corrected_p4->fill(*corrected_p4);

            _out << "[" << setw(2) << right << id << "]"
                << endl;

            _out << setw(5) << " "
                << "  CMSSW JEC "
                << "pT: " << pt(cmssw_p4)
                << " eta: " << eta(cmssw_p4)
                << endl;

            _out << setw(5) << " "
                << "     NO JEC "
                << "pT: " << pt(uncorrected_p4)
                << " eta: " << eta(uncorrected_p4)
                << endl;

            _out << setw(5) << " "
                << "Offline JEC "
                << "pT: " << pt(*corrected_p4)
                << " eta: " << eta(*corrected_p4)
                << endl;
        }
    }

    _out << endl;
}
