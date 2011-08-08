// Synchronization exercise selector that is used to select "good"
// events with muons, electrons, jetc, etc.
//
// Created by Samvel Khalatyan, Aug 01, 2011
// Copyright 2011, All rights reserved

#include <boost/algorithm/string.hpp>
#include <boost/pointer_cast.hpp>

#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Electron.pb.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Isolation.pb.h"
#include "bsm_input/interface/Muon.pb.h"
#include "bsm_input/interface/PrimaryVertex.pb.h"
#include "bsm_input/interface/Physics.pb.h"
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

    _description.reset(new po::options_description("Synchronization Selector Options"));
    _description->add_options()
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

void SynchSelectorOptions::setDelegate(SynchSelectorDelegate *delegate)
{
    if (_delegate != delegate)
        _delegate = delegate;
}

SynchSelectorDelegate *SynchSelectorOptions::delegate() const
{
    return _delegate;
}

// Options interface
//
SynchSelectorOptions::DescriptionPtr SynchSelectorOptions::description() const
{
    return _description;
}

// Privates
//
void SynchSelectorOptions::setLeptonMode(std::string mode)
{
    if (!delegate())
        return;

    to_lower(mode);

    if ("electron" == mode)
        delegate()->setLeptonMode(SynchSelectorDelegate::ELECTRON);
    else if ("muon" == mode)
        delegate()->setLeptonMode(SynchSelectorDelegate::MUON);
    else
        cerr << "unsupported synchronization selector lepton mode" << endl;
}

void SynchSelectorOptions::setCutMode(std::string mode)
{
    if (!delegate())
        return;

    to_lower(mode);

    if ("2dcut" == mode)
        delegate()->setCutMode(SynchSelectorDelegate::CUT_2D);
    else if ("isolation" == mode)
        delegate()->setCutMode(SynchSelectorDelegate::ISOLATION);
    else
        cerr << "unsupported synchronization selector cut mode" << endl;
}



// Synchronization Exercise Selector
//
SynchSelector::SynchSelector():
    _lepton_mode(ELECTRON),
    _cut_mode(CUT_2D)
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

SynchSelector::~SynchSelector()
{
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
        && lepton()
        && secondaryLeptonVeto()
        && cut()
        && leadingJet()
        && htlep(event);
}

SynchSelector::CutflowPtr SynchSelector::cutflow() const
{
    return _cutflow;
}

const SynchSelector::GoodElectrons &SynchSelector::goodElectrons() const
{
    return _good_electrons;
}

const SynchSelector::GoodMuons &SynchSelector::goodMuons() const
{
    return _good_muons;
}

const SynchSelector::GoodJets &SynchSelector::niceJets() const
{
    return _nice_jets;
}

const SynchSelector::GoodJets &SynchSelector::goodJets() const
{
    return _good_jets;
}

SynchSelector::LeptonMode SynchSelector::leptonMode() const
{
    return _lepton_mode;
}

SynchSelector::CutMode SynchSelector::cutMode() const
{
    return _cut_mode;
}

bsm::JetEnergyCorrectionDelegate *SynchSelector::getJetEnergyCorrectionDelegate() const
{
    return _jec.get();
}

// Synch Selector Delegate interface
//
void SynchSelector::setLeptonMode(const LeptonMode &lepton_mode)
{
    _lepton_mode = lepton_mode;
}

void SynchSelector::setCutMode(const CutMode &cut_mode)
{
    _cut_mode = cut_mode;
}

// Selector interface
//
void SynchSelector::enable()
{
}

void SynchSelector::disable()
{
}

// Object inteface
//
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
        LorentzVectorPtr corrected_p4 = _jec->correctJet(&*jet,
                event,
                _good_electrons,
                _good_muons);

        // Skip jet if energy corrections failed
        //
        if (!corrected_p4)
            continue;

        // Original jet in the event can not be modified and Jet Selector can
        // only be applied to jet: therefore copy jet, set corrected p4 and
        // apply selector
        //
        Jet corrected_jet = *jet;
        *corrected_jet.mutable_physics_object()->mutable_p4() = *corrected_p4;

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

bool SynchSelector::lepton()
{
    return (ELECTRON == _lepton_mode
        ? !_good_electrons.empty()
        : !_good_muons.empty())

        && (_cutflow->apply(LEPTON), true);
}

bool SynchSelector::secondaryLeptonVeto()
{
    return (ELECTRON == _lepton_mode
        ? (1 == _good_electrons.size()
            && _good_muons.empty())

        : (1 == _good_muons.size()
            && _good_electrons.empty()))

        && (_cutflow->apply(VETO_SECOND_LEPTON), true);
}

bool SynchSelector::cut()
{
    const LorentzVector *lepton_p4 = 0;
    const PFIsolation *lepton_isolation = 0;

    if (ELECTRON == _lepton_mode)
    {
        const Electron *electron = *_good_electrons.begin();

        lepton_p4 = &(electron->physics_object().p4());

        if (electron->has_pf_isolation())
            lepton_isolation = &(electron->pf_isolation());
    }
    else
    {
        const Muon *muon = *_good_muons.begin();

        lepton_p4 = &(muon->physics_object().p4());

        if (muon->has_pf_isolation())
            lepton_isolation = &(muon->pf_isolation());
    }

    bool result = false;
    if (CUT_2D == _cut_mode)
        result = cut2D(lepton_p4);
    else
    {
        if (lepton_isolation)
            result = isolation(lepton_p4, lepton_isolation);
    }

    return result
        && (_cutflow->apply(CUT_LEPTON), true);
}

bool SynchSelector::leadingJet()
{
    float max_pt = 0;
    for(GoodJets::const_iterator jet = _good_jets.begin();
            _good_jets.end() != jet;
            ++jet)
    {
        const float jet_pt = pt(*jet->corrected_p4);
        if (jet_pt > max_pt)
            max_pt = jet_pt;
    }

    return 250 < max_pt
        && (_cutflow->apply(LEADING_JET), true);
}

bool SynchSelector::htlep(const Event *event)
{
    const LorentzVector &lepton_p4 = (ELECTRON == _lepton_mode 
        ? (*_good_electrons.begin())->physics_object().p4()
        : (*_good_muons.begin())->physics_object().p4());

    return event->has_missing_energy()
        && 150 < (pt(event->missing_energy().p4()) + pt(lepton_p4))
        && (_cutflow->apply(HTLEP), true);
}

bool SynchSelector::cut2D(const LorentzVector *lepton_p4)
{
    if (_nice_jets.empty())
        return true;

    GoodJets::const_iterator closest_jet = _nice_jets.end();
    float deltar_min = 999999;

    for(GoodJets::const_iterator jet = _nice_jets.begin();
            _nice_jets.end() != jet;
            ++jet)
    {
        const float deltar = dr(*lepton_p4, *jet->corrected_p4);
        if (deltar < deltar_min)
        {
            deltar_min = deltar;
            closest_jet = jet;
        }
    }

    if (_nice_jets.end() == closest_jet)
        return true;

    return 0.5 < deltar_min
        || 25 < ptrel(*lepton_p4, *closest_jet->corrected_p4);
}

bool SynchSelector::isolation(const LorentzVector *p4, const PFIsolation *isolation)
{
    return 0.5 < (isolation->charged_hadron()
            + isolation->neutral_hadron()
            + isolation->photon())
        / pt(*p4);
}

void SynchSelector::selectGoodElectrons(const Event *event)
{
    _good_electrons.clear();

    typedef ::google::protobuf::RepeatedPtrField<Electron> Electrons;

    const PrimaryVertex &pv = *event->primary_vertices().begin();

    LockSelectorEventCounterOnUpdate lock(*_electron_selector);
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

    LockSelectorEventCounterOnUpdate lock(*_muon_selector);
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
