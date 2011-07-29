// Synchronization Exercises
//
// Different Analyzers that performan the sinchronization exercises at
// different development stages
//
// Created by Samvel Khalatyan, Jul 05, 2011
// Copyright 2011, All rights reserved

#include <ostream>
#include <string>
#include <utility>

#include <boost/pointer_cast.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Event.pb.h"
#include "JetMETObjects/interface/FactorizedJetCorrector.h"
#include "interface/Monitor.h"
#include "interface/Selector.h"
#include "interface/SynchAnalyzer.h"

using namespace std;

using boost::dynamic_pointer_cast;

using bsm::SynchJuly2011Analyzer;
using bsm::SynchJECJuly2011Analyzer;

SynchJuly2011Analyzer::SynchJuly2011Analyzer(const SynchMode &mode):
    _synch_mode(mode)
{
    _cutflow.reset(new MultiplicityCutflow(4));
    _cutflow->cut(PRESELECTION)->setName("pre-selection");
    _cutflow->cut(PRIMARY_VERTEX)->setName("Good Primary Vertex");
    _cutflow->cut(JET)->setName("2 Good Jets");
    _cutflow->cut(LEPTON)->setName(string("Good ")
            + (ELECTRON == _synch_mode ? "Electron" : "Muon"));
    _cutflow->cut(VETO_SECOND_LEPTON)->setName(string("Veto Good ")
            + (ELECTRON == _synch_mode ? "Muon" : "Electron"));
    monitor(_cutflow);

    // Selectrors
    //
    _primary_vertex_selector.reset(new PrimaryVertexSelector());
    _primary_vertex_selector->rho()->setValue(2.0);

    _jet_selector.reset(new JetSelector());
    _electron_selector.reset(new ElectronSelector());
    _muon_selector.reset(new MuonSelector());

    _electron_selector->primary_vertex()->disable();

    _muon_selector->primary_vertex()->disable();
    _muon_selector->pt()->setValue(35);

    monitor(_primary_vertex_selector);
    monitor(_jet_selector);
    monitor(_electron_selector);
    monitor(_muon_selector);

    // Monitors
    //
    _leading_jet.reset(new LorentzVectorMonitor());

    _electron_before_veto.reset(new LorentzVectorMonitor());
    _muon_to_veto.reset(new LorentzVectorMonitor());
    _electron_after_veto.reset(new LorentzVectorMonitor());

    _muon_before_veto.reset(new LorentzVectorMonitor());
    _electron_to_veto.reset(new LorentzVectorMonitor());
    _muon_after_veto.reset(new LorentzVectorMonitor());

    monitor(_leading_jet);

    monitor(_electron_before_veto);
    monitor(_muon_to_veto);
    monitor(_electron_after_veto);

    monitor(_muon_before_veto);
    monitor(_electron_to_veto);
    monitor(_muon_after_veto);
}

SynchJuly2011Analyzer::SynchJuly2011Analyzer(const SynchJuly2011Analyzer &object):
    _synch_mode(object._synch_mode)
{
    _cutflow = 
        dynamic_pointer_cast<MultiplicityCutflow>(object._cutflow->clone());

    monitor(_cutflow);

    // Selectors
    //
    _primary_vertex_selector = 
        dynamic_pointer_cast<PrimaryVertexSelector>(object._primary_vertex_selector->clone());

    _jet_selector = 
        dynamic_pointer_cast<JetSelector>(object._jet_selector->clone());

    _electron_selector = 
        dynamic_pointer_cast<ElectronSelector>(object._electron_selector->clone());

    _muon_selector = 
        dynamic_pointer_cast<MuonSelector>(object._muon_selector->clone());

    monitor(_primary_vertex_selector);
    monitor(_jet_selector);
    monitor(_electron_selector);
    monitor(_muon_selector);

    // Monitors
    //
    // Monitors
    //
    _leading_jet = 
        dynamic_pointer_cast<LorentzVectorMonitor>(object._leading_jet->clone());

    _electron_before_veto = 
        dynamic_pointer_cast<LorentzVectorMonitor>(object._electron_before_veto->clone());

    _muon_to_veto = 
        dynamic_pointer_cast<LorentzVectorMonitor>(object._muon_to_veto->clone());

    _electron_after_veto = 
        dynamic_pointer_cast<LorentzVectorMonitor>(object._electron_after_veto->clone());

    _muon_before_veto = 
        dynamic_pointer_cast<LorentzVectorMonitor>(object._muon_before_veto->clone());

    _electron_to_veto = 
        dynamic_pointer_cast<LorentzVectorMonitor>(object._electron_to_veto->clone());

    _muon_after_veto = 
        dynamic_pointer_cast<LorentzVectorMonitor>(object._muon_after_veto->clone());

    monitor(_leading_jet);

    monitor(_electron_before_veto);
    monitor(_muon_to_veto);
    monitor(_electron_after_veto);

    monitor(_muon_before_veto);
    monitor(_electron_to_veto);
    monitor(_muon_after_veto);
}

const SynchJuly2011Analyzer::P4MonitorPtr SynchJuly2011Analyzer::leadingJet() const
{
    return _leading_jet;
}

const SynchJuly2011Analyzer::P4MonitorPtr SynchJuly2011Analyzer::electronBeforeVeto() const
{
    return _electron_before_veto;
}

const SynchJuly2011Analyzer::P4MonitorPtr SynchJuly2011Analyzer::muonToVeto() const
{
    return _muon_to_veto;
}

const SynchJuly2011Analyzer::P4MonitorPtr SynchJuly2011Analyzer::electronAfterVeto() const
{
    return _electron_after_veto;
}

const SynchJuly2011Analyzer::P4MonitorPtr SynchJuly2011Analyzer::muonBeforeVeto() const
{
    return _muon_before_veto;
}

const SynchJuly2011Analyzer::P4MonitorPtr SynchJuly2011Analyzer::electronToVeto() const
{
    return _electron_to_veto;
}

const SynchJuly2011Analyzer::P4MonitorPtr SynchJuly2011Analyzer::muonAfterVeto() const
{
    return _muon_after_veto;
}


void SynchJuly2011Analyzer::onFileOpen(const std::string &filename, const Input *)
{
}

void SynchJuly2011Analyzer::process(const Event *event)
{
    _cutflow->apply(PRESELECTION);

    if (!event->primary_vertices().size()
            || !_primary_vertex_selector->apply(*event->primary_vertices().begin()))
        return;

    _cutflow->apply(PRIMARY_VERTEX);

    if (!jets(event))
        return;

    _cutflow->apply(JET);

    bool lepton_cut = false;
    switch(_synch_mode)
    {
        case ELECTRON:  lepton_cut = electron(event);
                        break;

        case MUON:      lepton_cut = muon(event);
                        break;

        default:        break;
    }

    if (!lepton_cut)
        return;

    _cutflow->apply(VETO_SECOND_LEPTON);

    _passed_events.push_back(event->extra());
}

uint32_t SynchJuly2011Analyzer::id() const
{
    return core::ID<SynchJuly2011Analyzer>::get();
}

SynchJuly2011Analyzer::ObjectPtr SynchJuly2011Analyzer::clone() const
{
    return ObjectPtr(new SynchJuly2011Analyzer(*this));
}

void SynchJuly2011Analyzer::merge(const ObjectPtr &pointer)
{
    if (pointer->id() != id())
        return;

    boost::shared_ptr<SynchJuly2011Analyzer> object =
        dynamic_pointer_cast<SynchJuly2011Analyzer>(pointer);

    if (!object)
        return;

    Object::merge(pointer);

    _passed_events.insert(_passed_events.end(),
            object->_passed_events.begin(),
            object->_passed_events.end());
}

void SynchJuly2011Analyzer::print(std::ostream &out) const
{
    out << "Survived Events" << endl;
    out << " " << setw(10) << left << "Run" << setw(10) << "Lumi" << "Event" << endl;
    for(std::vector<Event::Extra>::const_iterator extra = _passed_events.begin();
            _passed_events.end() != extra;
            ++extra)
    {
        out << " "
            << setw(10) << left << extra->run()
            << setw(10) << left << extra->lumi()
            << extra->id() << endl;
    }
    out << endl;

    out << "Cutflow [" << _synch_mode << " mode]" << endl;
    out << *_cutflow << endl;
    out << endl;

    out << "Primary Vertex Selector" << endl;
    out << *_primary_vertex_selector << endl;
    out << endl;

    out << "Jet Selector" << endl;
    out << *_jet_selector << endl;
    out << endl;

    switch(_synch_mode)
    {
        case ELECTRON: out << "Electron Selector" << endl;
                       out << *_electron_selector << endl;
                       out << endl;
                       out << "Muon Veto" << endl;
                       out << *_muon_selector << endl;
                       out << endl;
                       out << "Electron before Veto" << endl;
                       out << *electronBeforeVeto() << endl;
                       out << endl;
                       out << "Muon to Veto" << endl;
                       out << *muonToVeto() << endl;
                       out << endl;
                       out << "Electron after Veto" << endl;
                       out << *electronAfterVeto();
                       break;

        case MUON:     out << "Muon Selector" << endl;
                       out << *_muon_selector << endl;
                       out << endl;
                       out << "Electron Veto" << endl;
                       out << *_electron_selector << endl;
                       out << endl;
                       out << "Muon before Veto" << endl;
                       out << *muonBeforeVeto() << endl;
                       out << endl;
                       out << "Electron to Veto" << endl;
                       out << *electronToVeto() << endl;
                       out << endl;
                       out << "Muon after Veto" << endl;
                       out << *muonAfterVeto();
                       break;

        default: break;
    }
}

// Private
//
bool SynchJuly2011Analyzer::jets(const Event *event)
{
    if (!event->jets().size())
        return false;

    typedef ::google::protobuf::RepeatedPtrField<Jet> Jets;

    uint32_t selected_jets = 0;
    const Jet *leading_jet = 0;
    float leading_jet_pt = 0;
    LockSelectorEventCounterOnUpdate lock(*_jet_selector);
    for(Jets::const_iterator jet = event->jets().begin();
            event->jets().end() != jet
                && 2 > selected_jets;
            ++jet)
    {
        if (_jet_selector->apply(*jet))
        {
            ++selected_jets;

            const float jet_pt = pt(jet->physics_object().p4());
            if (!leading_jet
                    || jet_pt > leading_jet_pt)
            {
                leading_jet = &*jet;
                leading_jet_pt = jet_pt;
            }
        }
    }

    if (1 < selected_jets
            && leading_jet)
        _leading_jet->fill(leading_jet->physics_object().p4());

    return 1 < selected_jets;
}

bool SynchJuly2011Analyzer::electron(const Event *event)
{
    if (!event->pf_electrons().size())
        return false;

    typedef ::google::protobuf::RepeatedPtrField<Electron> Electrons;
    typedef ::google::protobuf::RepeatedPtrField<Muon> Muons;

    const PrimaryVertex &pv = *event->primary_vertices().begin();

    uint32_t selected_electrons = 0;
    const Electron *selected_electron = 0;
    LockSelectorEventCounterOnUpdate electron_lock(*_electron_selector);
    for(Electrons::const_iterator electron = event->pf_electrons().begin();
            event->pf_electrons().end() != electron
                && 2 > selected_electrons;
            ++electron)
    {
        if (_electron_selector->apply(*electron, pv))
        {
            ++selected_electrons;

            if (!selected_electron)
                selected_electron = &*electron;
        }
    }

    if (!selected_electrons)
        return false;

    _cutflow->apply(LEPTON);

    if (selected_electron)
        _electron_before_veto->fill(selected_electron->physics_object().p4());

    uint32_t selected_muons = 0;
    const Muon *selected_muon = 0;
    LockSelectorEventCounterOnUpdate muon_lock(*_muon_selector);
    for(Muons::const_iterator muon = event->pf_muons().begin();
            event->pf_muons().end() != muon
                &&  !selected_muons;
            ++muon)
    {
        if (_muon_selector->apply(*muon, pv))
        {
            ++selected_muons;

            if (!selected_muon)
                selected_muon = &*muon;
        }
    }

    if (selected_muon)
        _muon_to_veto->fill(selected_muon->physics_object().p4());
    else if (selected_electron)
        _electron_after_veto->fill(selected_electron->physics_object().p4());

    return !selected_muons
        || 1 < selected_electrons;
}

bool SynchJuly2011Analyzer::muon(const Event *event)
{
    if (!event->pf_muons().size())
        return false;

    typedef ::google::protobuf::RepeatedPtrField<Electron> Electrons;
    typedef ::google::protobuf::RepeatedPtrField<Muon> Muons;

    const PrimaryVertex &pv = *event->primary_vertices().begin();

    uint32_t selected_muons = 0;
    const Muon *selected_muon = 0;
    LockSelectorEventCounterOnUpdate muon_lock(*_muon_selector);
    for(Muons::const_iterator muon = event->pf_muons().begin();
            event->pf_muons().end() != muon
                && 2 > selected_muons;
            ++muon)
    {
        if (_muon_selector->apply(*muon, pv))
        {
            ++selected_muons;

            if (!selected_muon)
                selected_muon = &*muon;
        }
    }

    if (!selected_muons)
        return false;

    _cutflow->apply(LEPTON);
    
    if (selected_muon)
        _muon_before_veto->fill(selected_muon->physics_object().p4());

    uint32_t selected_electrons = 0;
    const Electron *selected_electron = 0;
    LockSelectorEventCounterOnUpdate electron_lock(*_electron_selector);
    for(Electrons::const_iterator electron = event->pf_electrons().begin();
            event->pf_electrons().end() != electron
                &&  !selected_electrons;
            ++electron)
    {
        if (_electron_selector->apply(*electron, pv))
        {
            ++selected_electrons;

            if (!selected_electron)
                selected_electron = &*electron;
        }
    }

    if (selected_electron)
        _electron_to_veto->fill(selected_electron->physics_object().p4());
    else if (selected_muon)
        _muon_after_veto->fill(selected_muon->physics_object().p4());

    return !selected_electrons
        || 1 < selected_muons;
}



// Synch with Jet Energy corrections
//
SynchJECJuly2011Analyzer::SynchJECJuly2011Analyzer(const SynchMode &synch_mode,
        const SynchCut &synch_cut):
    _synch_mode(synch_mode),
    _synch_cut(synch_cut)
{
    _cutflow.reset(new MultiplicityCutflow(SELECTIONS - 1));
    _cutflow->cut(PRESELECTION)->setName("pre-selection");
    _cutflow->cut(SCRAPING)->setName("Scraping Veto");
    _cutflow->cut(HBHENOISE)->setName("HBHE Noise");
    _cutflow->cut(PRIMARY_VERTEX)->setName("Good Primary Vertex");
    _cutflow->cut(JET)->setName("2 Good Jets");

    string lepton = ELECTRON == _synch_mode
        ? "Electron"
        : "Muon";

    _cutflow->cut(LEPTON)->setName(string("Good ") + lepton);
    _cutflow->cut(VETO_SECOND_LEPTON)->setName("Veto 2nd lepton");
    _cutflow->cut(CUT_LEPTON)->setName(lepton + " " +
            (CUT_2D == _synch_cut
             ? "2D-cut"
             : (ISOLATION == _synch_cut
                    ? "Isolation"
                    : "unknown")));
    _cutflow->cut(LEADING_JET)->setName("Leading Jet");
    _cutflow->cut(HTLEP)->setName("hTlep");
    monitor(_cutflow);

    // Selectrors
    //
    _primary_vertex_selector.reset(new PrimaryVertexSelector());
    _primary_vertex_selector->rho()->setValue(2.0);

    _nice_jet_selector.reset(new JetSelector());
    _nice_jet_selector->pt()->setValue(25);

    _good_jet_selector.reset(new JetSelector());

    _electron_selector.reset(new ElectronSelector());
    _muon_selector.reset(new MuonSelector());

    _electron_selector->primary_vertex()->disable();

    _muon_selector->primary_vertex()->disable();

    _muon_selector->pt()->setValue(35);

    monitor(_primary_vertex_selector);
    monitor(_nice_jet_selector);
    monitor(_good_jet_selector);
    monitor(_electron_selector);
    monitor(_muon_selector);
}

SynchJECJuly2011Analyzer::SynchJECJuly2011Analyzer(const SynchJECJuly2011Analyzer &object):
    _synch_mode(object._synch_mode),
    _synch_cut(object._synch_cut)
{
    setJetEnergyCorrections(object._corrections);

    _cutflow = 
        dynamic_pointer_cast<MultiplicityCutflow>(object._cutflow->clone());

    monitor(_cutflow);

    // Selectors
    //
    _primary_vertex_selector = 
        dynamic_pointer_cast<PrimaryVertexSelector>(object._primary_vertex_selector->clone());

    _nice_jet_selector = 
        dynamic_pointer_cast<JetSelector>(object._nice_jet_selector->clone());

    _good_jet_selector = 
        dynamic_pointer_cast<JetSelector>(object._good_jet_selector->clone());

    _electron_selector = 
        dynamic_pointer_cast<ElectronSelector>(object._electron_selector->clone());

    _muon_selector = 
        dynamic_pointer_cast<MuonSelector>(object._muon_selector->clone());

    monitor(_primary_vertex_selector);
    monitor(_nice_jet_selector);
    monitor(_good_jet_selector);
    monitor(_electron_selector);
    monitor(_muon_selector);
}

void SynchJECJuly2011Analyzer::setJetEnergyCorrections(const Corrections &corrections)
{
    copyCorrections(corrections);

    _jec.reset(new FactorizedJetCorrector(_corrections));
}

void SynchJECJuly2011Analyzer::onFileOpen(const std::string &filename, const Input *)
{
}

void SynchJECJuly2011Analyzer::process(const Event *event)
{
    if (!_jec)
        return;

    _cutflow->apply(PRESELECTION);

    if (event->has_filters()
            && !event->filters().scraping_veto())
        return;

    _cutflow->apply(SCRAPING);

    _cutflow->apply(HBHENOISE);

    if (!event->primary_vertices().size()
            || !_primary_vertex_selector->apply(*event->primary_vertices().begin()))
        return;

    _cutflow->apply(PRIMARY_VERTEX);

    typedef ::google::protobuf::RepeatedPtrField<Electron> Electrons;

    const PrimaryVertex &pv = *event->primary_vertices().begin();
    GoodElectrons good_electrons;

    LockSelectorEventCounterOnUpdate electron_lock(*_electron_selector);
    for(Electrons::const_iterator electron = event->pf_electrons().begin();
            event->pf_electrons().end() != electron;
            ++electron)
    {
        if (_electron_selector->apply(*electron, pv))
            good_electrons.push_back(&*electron);
    }

    typedef ::google::protobuf::RepeatedPtrField<Muon> Muons;

    GoodMuons good_muons;

    LockSelectorEventCounterOnUpdate muon_lock(*_muon_selector);
    for(Muons::const_iterator muon = event->pf_muons().begin();
            event->pf_muons().end() != muon;
            ++muon)
    {
        if (_muon_selector->apply(*muon, pv))
            good_muons.push_back(&*muon);
    }

    GoodJets nice_jets;
    GoodJets good_jets;
    jets(event, good_electrons, good_muons, nice_jets, good_jets);

    if (2 > good_jets.size())
        return;

    _cutflow->apply(JET);

    const LorentzVector *lepton_p4 = 0;
    const PFIsolation *lepton_isolation = 0;

    if (ELECTRON == _synch_mode)
    {
        if (!good_electrons.size())
            return;

        _cutflow->apply(LEPTON);

        if (good_muons.size()
            || 1 < good_electrons.size())
            return;

        const Electron *electron = *good_electrons.begin();

        lepton_p4 = &(electron->physics_object().p4());

        if (electron->has_pf_isolation())
            lepton_isolation = &(electron->pf_isolation());
    }
    else if (MUON == _synch_mode)
    {
        if (!good_muons.size())
            return;

        _cutflow->apply(LEPTON);

        if (good_electrons.size()
            || 1 < good_muons.size())
            return;

        const Muon *muon = *good_muons.begin();

        lepton_p4 = &(muon->physics_object().p4());

        if (muon->has_pf_isolation())
            lepton_isolation = &(muon->pf_isolation());
    }
    else
        return;

    _cutflow->apply(VETO_SECOND_LEPTON);

    if (CUT_2D == _synch_cut)
    {
        if (!cut2D(*lepton_p4, nice_jets))
            return;
    }
    else if (ISOLATION == _synch_cut)
    {
        if (lepton_isolation
            && !isolation(*lepton_p4, *lepton_isolation))

            return;
    }
    else // Unknown cut type
        return;

    _cutflow->apply(CUT_LEPTON);

    float max_pt = 0;
    for(GoodJets::const_iterator jet = good_jets.begin();
            good_jets.end() != jet;
            ++jet)
    {
        const float jet_pt = pt(jet->corrected_p4);
        if (jet_pt > max_pt)
            max_pt = jet_pt;
    }

    if (250 >= max_pt)
        return;

    _cutflow->apply(LEADING_JET);

    if (150 >= (pt(event->missing_energy().p4()) + pt(*lepton_p4)))
        return;

    _cutflow->apply(HTLEP);

    _passed_events.push_back(event->extra());
}

uint32_t SynchJECJuly2011Analyzer::id() const
{
    return core::ID<SynchJECJuly2011Analyzer>::get();
}

SynchJECJuly2011Analyzer::ObjectPtr SynchJECJuly2011Analyzer::clone() const
{
    return ObjectPtr(new SynchJECJuly2011Analyzer(*this));
}

void SynchJECJuly2011Analyzer::merge(const ObjectPtr &pointer)
{
    if (pointer->id() != id())
        return;

    boost::shared_ptr<SynchJECJuly2011Analyzer> object =
        dynamic_pointer_cast<SynchJECJuly2011Analyzer>(pointer);

    if (!object)
        return;

    Object::merge(pointer);

    _passed_events.insert(_passed_events.end(),
            object->_passed_events.begin(),
            object->_passed_events.end());
}

void SynchJECJuly2011Analyzer::print(std::ostream &out) const
{
    out << "Cutflow [" << _synch_mode << " mode]" << endl;
    out << *_cutflow << endl;
    out << endl;

    out << "Primary Vertex Selector" << endl;
    out << *_primary_vertex_selector << endl;
    out << endl;

    out << "Jet Selector" << endl;
    out << *_nice_jet_selector << endl;
    out << endl;

    switch(_synch_mode)
    {
        case ELECTRON: out << "Electron Selector" << endl;
                       out << *_electron_selector << endl;
                       out << endl;
                       out << "Muon Veto" << endl;
                       out << *_muon_selector << endl;
                       out << endl;
                       break;

        case MUON:     out << "Muon Selector" << endl;
                       out << *_muon_selector << endl;
                       out << endl;
                       out << "Electron Veto" << endl;
                       out << *_electron_selector << endl;
                       out << endl;
                       break;

        default: break;
    }

    out << endl;
    out << _out.str();
}

// Private
//
void SynchJECJuly2011Analyzer::copyCorrections(const Corrections &corrections)
{
    _corrections.clear();

    if (!corrections.size())
        return;

    _corrections.resize(corrections.size());
    copy(corrections.begin(), corrections.end(), _corrections.begin());
}

void SynchJECJuly2011Analyzer::jets(const Event *event,
            const GoodElectrons &electrons,
            const GoodMuons &muons,
            GoodJets &nice_jets,
            GoodJets &good_jets)
{
    typedef ::google::protobuf::RepeatedPtrField<Jet> Jets;
    typedef ::google::protobuf::RepeatedPtrField<Jet::Child> Children;

    const float rho = event->extra().rho();

    LockSelectorEventCounterOnUpdate lock(*_nice_jet_selector);
    LockSelectorEventCounterOnUpdate lock(*_good_jet_selector);
    for(Jets::const_iterator jet = event->jets().begin();
            event->jets().end() != jet;
            ++jet)
    {
        // Skip jet is required information is not available
        //
        if (!jet->has_uncorrected_p4())
            continue;

        // Remove Leptons
        //
        LorentzVector corrected_p4 = jet->uncorrected_p4();
        for(Children::const_iterator child = jet->children().begin();
                jet->children().end() != child;
                ++child)
        {
            const LorentzVector &child_p4 = child->physics_object().p4();

            // Electrons
            //
            for(GoodElectrons::const_iterator electron = electrons.begin();
                    electrons.end() != electron;
                    ++electron)
            {
                const LorentzVector &electron_p4 = (*electron)->physics_object().p4();
                if (electron_p4 == child_p4)
                    corrected_p4 -= electron_p4;
            }

            // Muons
            //
            for(GoodMuons::const_iterator muon = muons.begin();
                    muons.end() != muon;
                    ++muon)
            {
                const LorentzVector &muon_p4 = (*muon)->physics_object().p4();
                if (muon_p4 == child_p4)
                    corrected_p4 -= muon_p4;
            }
        }

        // fix p4
        //
        _jec->setJetEta(eta(corrected_p4));
        _jec->setJetPt(pt(corrected_p4));
        _jec->setJetE(corrected_p4.e());
        _jec->setNPV(event->primary_vertices().size());
        _jec->setJetA(jet->extra().area());
        _jec->setRho(rho);

        float correction = _jec->getCorrection();
        corrected_p4 *= correction;

        // Original jet in the event can not be modified and Jet Selector can
        // only be applied to jet: therefore copy jet, set corrected p4 and
        // apply selector
        //
        Jet corrected_jet = *jet;
        *corrected_jet.mutable_physics_object()->mutable_p4() = corrected_p4;

        if (!_nice_jet_selector->apply(corrected_jet))
            continue;

        // Store original jet and corrected p4
        //
        CorrectedJet tmp;
        tmp.jet = &*jet;
        tmp.corrected_p4 = corrected_p4;

        nice_jets.push_back(tmp);

        if (!_good_jet_selector->apply(corrected_jet))
            continue;

        good_jets.push_back(tmp);
    }
}

bool SynchJECJuly2011Analyzer::cut2D(const LorentzVector &p4, const GoodJets &jets)
{
    if (jets.empty())
        return false;

    GoodJets::const_iterator closest_jet = jets.end();
    float deltar_min = 999999;

    for(GoodJets::const_iterator jet = jets.begin();
            jets.end() != jet;
            ++jet)
    {
        const float deltar = dr(p4, jet->corrected_p4);
        if (deltar < deltar_min)
        {
            deltar_min = deltar;
            closest_jet = jet;
        }
    }

    if (jets.end() == closest_jet)
        return false;

    return 0.5 < deltar_min
        || 25 < ptrel(p4, closest_jet->corrected_p4);
}

bool SynchJECJuly2011Analyzer::isolation(const LorentzVector &p4,
        const PFIsolation &isolation)
{
    return (isolation.charged_hadron() + isolation.neutral_hadron() + isolation.photon())
        / pt(p4);
}

void SynchJECJuly2011Analyzer::printP4(std::ostream &out, const LorentzVector &p4)
{
    out << "pT: " << pt(p4)
    << " eta: " << eta(p4)
    << " phi: " << phi(p4);
}


// Helpers
//
std::ostream &bsm::operator <<(std::ostream &out, const SynchMode &mode)
{
    switch(mode)
    {
        case ELECTRON:   out << "electron";
                         break;

        case MUON:       out << "muon";
                         break;

        default:         out << "unknown";
                         break;
    }

    return out;
}
