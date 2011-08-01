// Synchronization exercise selector that is used to select "good"
// events with muons, electrons, jetc, etc.
//
// Created by Samvel Khalatyan, Aug 01, 2011
// Copyright 2011, All rights reserved

#include <boost/algorithm/string.hpp>
#include <boost/pointer_cast.hpp>

#include "bsm_input/interface/Electron.pb.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Muon.pb.h"
#include "bsm_input/interface/PrimaryVertex.pb.h"
#include "interface/JetEnergyCorrections.h"
#include "interface/SynchSelector.h"

using namespace std;

using boost::to_lower;
using boost::dynamic_pointer_cast;

using bsm::SynchSelectorDelegate;
using bsm::SynchSelectorOptions;
using bsm::SynchSelector;

// Synch Selector Options
//
SynchSelectorOptions::SynchSelectorOptions()
{
    _delegate = 0;

    _options.reset(new po::options_description("Jet Energy Correction Options"));
    _options->add_options()
        ("lepton-mode",
         po::value<string>()->notifier(
             boost::bind(&SynchSelectorOptions::setLeptonMode, this, _1)),
         "synchronization selector lepton mode: muon, electron")

        ("cut-mode",
         po::value<string>()->notifier(
             boost::bind(&SynchSelectorOptions::setCutMode, this, _1)),
         "synchroniation selector cut mode: 2dcut, isolation")
    ;
}

SynchSelectorOptions::~SynchSelectorOptions()
{
}

SynchSelectorOptions::OptionsPtr SynchSelectorOptions::options() const
{
    return _options;
}

void SynchSelectorOptions::setLeptonMode(std::string mode)
{
    if (!_delegate)
        return;

    to_lower(mode);

    if ("electron" == mode)
        _delegate->setLeptonMode(SynchSelectorDelegate::ELECTRON);
    else if ("muon" == mode)
        _delegate->setLeptonMode(SynchSelectorDelegate::MUON);
    else
        cerr << "unsupported synchronization selector lepton mode" << endl;
}

void SynchSelectorOptions::setCutMode(std::string mode)
{
    if (!_delegate)
        return;

    to_lower(mode);

    if ("2dcut" == mode)
        _delegate->setCutMode(SynchSelectorDelegate::CUT_2D);
    else if ("isolation" == mode)
        _delegate->setCutMode(SynchSelectorDelegate::ISOLATION);
    else
        cerr << "unsupported synchronization selector cut mode" << endl;
}

void SynchSelectorOptions::setDelegate(SynchSelectorDelegate *delegate)
{
    if (_delegate != delegate)
        _delegate = delegate;
}

SynchSelectorDelegate *SynchSelectorOptions::delegate() const
{
    return _delegate;
}



// Synchronization Exercise Selector
//
SynchSelector::SynchSelector(const LeptonMode &lepton_mode,
        const CutMode &cut_mode):
    _lepton_mode(lepton_mode),
    _cut_mode(cut_mode)
{
    // Cutflow table
    //
    _cutflow.reset(new MultiplicityCutflow(SELECTIONS - 1));
    monitor(_cutflow);

    // Selectors
    //
    _primary_vertex_selector.reset(new PrimaryVertexSelector());
    _primary_vertex_selector->rho()->setValue(2.0);
    monitor(_primary_vertex_selector);

    _electron_selector.reset(new ElectronSelector());
    _electron_selector->primary_vertex()->disable();
    monitor(_electron_selector);

    _muon_selector.reset(new MuonSelector());
    _muon_selector->pt()->setValue(35);
    monitor(_muon_selector);

    // Nice jets have pT > 25
    //
    _nice_jet_selector.reset(new JetSelector());
    _nice_jet_selector->pt()->setValue(25);
    monitor(_nice_jet_selector);

    // Good jets have pT > 50
    //
    _good_jet_selector.reset(new JetSelector());
    monitor(_good_jet_selector);

    // Jet Energy Corrections
    //
    _jec.reset(new JetEnergyCorrections());
    monitor(_jec);
}

SynchSelector::SynchSelector(const SynchSelector &object):
    _lepton_mode(object._lepton_mode),
    _cut_mode(object._cut_mode)
{
    // Cutflow Table
    //
    _cutflow = 
        dynamic_pointer_cast<MultiplicityCutflow>(object._cutflow->clone());

    monitor(_cutflow);

    // Selectors
    //
    _primary_vertex_selector = 
        dynamic_pointer_cast<PrimaryVertexSelector>(object._primary_vertex_selector->clone());

    monitor(_primary_vertex_selector);

    _electron_selector = 
        dynamic_pointer_cast<ElectronSelector>(object._electron_selector->clone());

    monitor(_electron_selector);

    _muon_selector = 
        dynamic_pointer_cast<MuonSelector>(object._muon_selector->clone());

    monitor(_muon_selector);

    _nice_jet_selector = 
        dynamic_pointer_cast<JetSelector>(object._nice_jet_selector->clone());

    monitor(_nice_jet_selector);

    _good_jet_selector = 
        dynamic_pointer_cast<JetSelector>(object._good_jet_selector->clone());

    monitor(_good_jet_selector);

    // Jet Energy Corrections
    //
    _jec = dynamic_pointer_cast<JetEnergyCorrections>(object._jec->clone());
    monitor(_jec);
}

bool SynchSelector::apply(const Event *event)
{
    _cutflow->apply(PRESELECTION);

    _good_electrons.clear();
    _good_muons.clear();
    _nice_jets.clear();
    _good_jets.clear();

    return primaryVertices(event)
        && jets(event)
        && lepton(event)
        && secondaryLeptonVeto(event);
}

bsm::JetEnergyCorrectionDelegate *SynchSelector::getJetEnergyCorrectionDelegate() const
{
    return _jec.get();
}

void SynchSelector::setLeptonMode(const LeptonMode &lepton_mode)
{
    _lepton_mode = lepton_mode;
}

void SynchSelector::setCutMode(const CutMode &cut_mode)
{
    _cut_mode = cut_mode;
}

SynchSelector::CutflowPtr SynchSelector::cutflow() const
{
    return _cutflow;
}

void SynchSelector::enable()
{
    _primary_vertex_selector->enable();
}

void SynchSelector::disable()
{
    _primary_vertex_selector->disable();
}

uint32_t SynchSelector::id() const
{
    return core::ID<SynchSelector>::get();
}

SynchSelector::ObjectPtr SynchSelector::clone() const
{
    return ObjectPtr(new SynchSelector(*this));
}

void SynchSelector::print(std::ostream &out) const
{
    _cutflow->cut(PRESELECTION)->setName("pre-selection");
    _cutflow->cut(SCRAPING)->setName("Scraping Veto");
    _cutflow->cut(HBHENOISE)->setName("HBHE Noise");
    _cutflow->cut(PRIMARY_VERTEX)->setName("Good Primary Vertex");
    _cutflow->cut(JET)->setName("2 Good Jets");

    ostringstream lepton;
    lepton << _lepton_mode;

    _cutflow->cut(LEPTON)->setName(string("Good ") + lepton.str());
    _cutflow->cut(VETO_SECOND_LEPTON)->setName("Veto 2nd lepton");

    lepton << " " << _cut_mode;
    _cutflow->cut(CUT_LEPTON)->setName(lepton.str());

    _cutflow->cut(LEADING_JET)->setName("Leading Jet");
    _cutflow->cut(HTLEP)->setName("hTlep");

    out << "Cutflow [" << _lepton_mode << ": " << _cut_mode << "]" << endl;
    out << *_cutflow << endl;
    out << endl;
}

// Private
//
bool SynchSelector::primaryVertices(const Event *event)
{
    return event->primary_vertices().size()
        && _primary_vertex_selector->apply(*event->primary_vertices().begin())
        && (_cutflow->apply(PRIMARY_VERTEX), true);
}

bool SynchSelector::jets(const Event *event)
{
    selectGoodElectrons(event);
    selectGoodMuons(event);

    typedef ::google::protobuf::RepeatedPtrField<Jet> Jets;

    LockSelectorEventCounterOnUpdate lock_nice_jets(*_nice_jet_selector);
    LockSelectorEventCounterOnUpdate lock_good_jets(*_good_jet_selector);
    for(Jets::const_iterator jet = event->jets().begin();
            event->jets().end() != jet;
            ++jet)
    {
        LorentzVector corrected_p4 = _jec->correctJet(&*jet,
                event,
                _good_electrons,
                _good_muons);

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

        _nice_jets.push_back(tmp);

        if (!_good_jet_selector->apply(corrected_jet))
            continue;

        _good_jets.push_back(tmp);
    }

    return 1 < _good_jets.size()
        && (_cutflow->apply(JET), true);
}

bool SynchSelector::lepton(const Event *event)
{
    return (ELECTRON == _lepton_mode
        ? !_good_electrons.empty()
        : !_good_muons.empty())

        && (_cutflow->apply(LEPTON), true);
}

bool SynchSelector::secondaryLeptonVeto(const Event *event)
{
    return (ELECTRON == _lepton_mode
        ? (1 == _good_electrons.size()
            && _good_muons.empty())

        : (1 == _good_muons.size()
            && _good_electrons.empty()))

        && (_cutflow->apply(VETO_SECOND_LEPTON), true);
}

void SynchSelector::selectGoodElectrons(const Event *event)
{
    _good_electrons.clear();

    typedef ::google::protobuf::RepeatedPtrField<Electron> Electrons;

    const PrimaryVertex &pv = *event->primary_vertices().begin();

    LockSelectorEventCounterOnUpdate electron_lock(*_electron_selector);
    for(Electrons::const_iterator electron = event->pf_electrons().begin();
            event->pf_electrons().end() != electron;
            ++electron)
    {
        if (_electron_selector->apply(*electron, pv))
            _good_electrons.push_back(&*electron);
    }
}

void SynchSelector::selectGoodMuons(const Event *event)
{
    typedef ::google::protobuf::RepeatedPtrField<Muon> Muons;

    const PrimaryVertex &pv = *event->primary_vertices().begin();

    LockSelectorEventCounterOnUpdate muon_lock(*_muon_selector);
    for(Muons::const_iterator muon = event->pf_muons().begin();
            event->pf_muons().end() != muon;
            ++muon)
    {
        if (_muon_selector->apply(*muon, pv))
            _good_muons.push_back(&*muon);
    }
}



// Helpers
//
std::ostream &bsm::operator <<(std::ostream &out,
        const SynchSelectorDelegate::LeptonMode &lepton_mode)
{
    switch(lepton_mode)
    {
        case SynchSelector::ELECTRON:   out << "Electron";
                                        break;

        case SynchSelector::MUON:       out << "Muon";
                                        break;

        default:                        out << "unknown";
                                        break;
    }

    return out;
}

std::ostream &bsm::operator <<(std::ostream &out,
        const SynchSelectorDelegate::CutMode &cut_mode)
{
    switch(cut_mode)
    {
        case SynchSelector::CUT_2D:     out << "2D Cut";
                                        break;

        case SynchSelector::ISOLATION:  out << "Isolation";
                                        break;

        default:                        out << "unknown";
                                        break;
    }

    return out;
}
