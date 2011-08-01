// Common Selectors Classes/functions
//
// Physics objects selectors: muons, electrons, jets
//
// Created by Samvel Khalatyan, May 16, 2011
// Copyright 2011, All rights reserved

#include <cmath>
#include <functional>
#include <iomanip>
#include <ostream>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/pointer_cast.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Electron.pb.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Jet.pb.h"
#include "bsm_input/interface/Muon.pb.h"
#include "bsm_input/interface/Physics.pb.h"
#include "bsm_input/interface/PrimaryVertex.pb.h"
#include "interface/Cut.h"
#include "interface/JetEnergyCorrections.h"
#include "interface/Selector.h"
#include "interface/Utility.h"

using namespace std;

using boost::dynamic_pointer_cast;
using boost::to_lower;

using bsm::CutPtr;
using bsm::ElectronSelector;
using bsm::JetEnergyCorrectionDelegate;
using bsm::JetSelector;
using bsm::MultiplicityCutflow;
using bsm::MuonSelector;
using bsm::PrimaryVertexSelector;
using bsm::SynchSelectorDelegate;
using bsm::SynchSelectorOptions;
using bsm::SynchSelector;
using bsm::WJetSelector;
using bsm::LockSelectorEventCounterOnUpdate;

// ElectronSelector
//
ElectronSelector::ElectronSelector()
{
    _pt.reset(new Comparator<>(30, "Pt"));
    _eta.reset(new Comparator<std::less<float> >(2.5, "|eta|"));
    _primary_vertex.reset(new Comparator<std::less<float> >(1, "|el.z() - pv.z()|"));

    monitor(_pt);
    monitor(_eta);
    monitor(_primary_vertex);
}

ElectronSelector::ElectronSelector(const ElectronSelector &object)
{
    _pt = dynamic_pointer_cast<Cut>(object._pt->clone());
    _eta = dynamic_pointer_cast<Cut>(object._eta->clone());
    _primary_vertex = dynamic_pointer_cast<Cut>(object._primary_vertex->clone());

    monitor(_pt);
    monitor(_eta);
    monitor(_primary_vertex);
}

bool ElectronSelector::apply(const Electron &electron, const PrimaryVertex &pv)
{
    return _pt->apply(bsm::pt(electron.physics_object().p4()))
        && _eta->apply(fabs(bsm::eta(electron.physics_object().p4())))
        && _primary_vertex->apply(fabs(electron.physics_object().vertex().z()
                    - pv.vertex().z()));
}

CutPtr ElectronSelector::pt() const
{
    return _pt;
}

CutPtr ElectronSelector::eta() const
{
    return _eta;
}

CutPtr ElectronSelector::primary_vertex() const
{
    return _primary_vertex;
}

void ElectronSelector::enable()
{
    pt()->enable();
    eta()->enable();
    primary_vertex()->enable();
}

void ElectronSelector::disable()
{
    pt()->disable();
    eta()->disable();
    primary_vertex()->disable();
}

uint32_t ElectronSelector::id() const
{
    return core::ID<ElectronSelector>::get();
}

ElectronSelector::ObjectPtr ElectronSelector::clone() const
{
    return ObjectPtr(new ElectronSelector(*this));
}

void ElectronSelector::print(std::ostream &out) const
{
    out << "     CUT                 " << setw(5) << " "
        << " Objects Events" << endl;
    out << setw(45) << setfill('-') << left << " " << setfill(' ') << endl;
    out << *_pt << endl;
    out << *_eta << endl;
    out << *_primary_vertex;
}



// JetSelector
//
JetSelector::JetSelector()
{
    _pt.reset(new Comparator<>(50, "Pt"));
    _eta.reset(new Comparator<std::less<float> >(2.4, "|eta|"));

    monitor(_pt);
    monitor(_eta);
}

JetSelector::JetSelector(const JetSelector &object)
{
    _pt = dynamic_pointer_cast<Cut>(object._pt->clone());;
    _eta = dynamic_pointer_cast<Cut>(object._eta->clone());;

    monitor(_pt);
    monitor(_eta);
}

bool JetSelector::apply(const Jet &jet)
{
    return _pt->apply(bsm::pt(jet.physics_object().p4()))
        && _eta->apply(fabs(bsm::eta(jet.physics_object().p4())));
}

CutPtr JetSelector::pt() const
{
    return _pt;
}

CutPtr JetSelector::eta() const
{
    return _eta;
}

void JetSelector::enable()
{
    pt()->enable();
    eta()->enable();
}

void JetSelector::disable()
{
    pt()->disable();
    eta()->disable();
}

uint32_t JetSelector::id() const
{
    return core::ID<JetSelector>::get();
}

JetSelector::ObjectPtr JetSelector::clone() const
{
    return ObjectPtr(new JetSelector(*this));
}

void JetSelector::print(std::ostream &out) const
{
    out << "     CUT                 " << setw(5) << " "
        << " Objects Events" << endl;
    out << setw(45) << setfill('-') << left << " " << setfill(' ') << endl;
    out << *_pt << endl;
    out << *_eta;
}



// Multiplicity Cutflow
//
MultiplicityCutflow::MultiplicityCutflow(const uint32_t &max)
{
    for(uint32_t i = 0; max > i; ++i)
    {
        _cuts.push_back(CutPtr(new Comparator<std::equal_to<uint32_t> >(i)));
    }
    _cuts.push_back(CutPtr(new Comparator<std::greater_equal<uint32_t> >(max)));

    for(Cuts::const_iterator cut = _cuts.begin();
            _cuts.end() != cut;
            ++cut)
    {
        monitor(*cut);
    }
}

MultiplicityCutflow::MultiplicityCutflow(const MultiplicityCutflow &object)
{
    for(Cuts::const_iterator cut = object._cuts.begin();
            object._cuts.end() != cut;
            ++cut)
    {
        const CutPtr clone = dynamic_pointer_cast<Cut>((*cut)->clone());

        _cuts.push_back(clone);
        monitor(clone);
    }
}

void MultiplicityCutflow::apply(const uint32_t &number)
{
    // It does not make sense to apply all cuts. Only Nth one:
    //
    if (_cuts.size() > number)
        _cuts[number]->apply(number);
    else
        (*--_cuts.end())->apply(number);
}

bsm::CutPtr MultiplicityCutflow::cut(const uint32_t &cut) const
{
    return cut >= _cuts.size()
        ? *(--_cuts.end())
        : _cuts[cut];
}

void MultiplicityCutflow::enable()
{
    for(Cuts::const_iterator cut = _cuts.begin();
            _cuts.end() != cut;
            ++cut)
    {
        (*cut)->enable();
    }
}

void MultiplicityCutflow::disable()
{
    for(Cuts::const_iterator cut = _cuts.begin();
            _cuts.end() != cut;
            ++cut)
    {
        (*cut)->disable();
    }
}

uint32_t MultiplicityCutflow::id() const
{
    return core::ID<MultiplicityCutflow>::get();
}

MultiplicityCutflow::ObjectPtr MultiplicityCutflow::clone() const
{
    return ObjectPtr(new MultiplicityCutflow(*this));
}

void MultiplicityCutflow::print(std::ostream &out) const
{
    out << "     CUT                 " << setw(5) << " "
        << " Objects Events" << endl;
    out << setw(45) << setfill('-') << left << " " << setfill(' ') << endl;
    for(uint32_t cut = 0, max = _cuts.size() - 1; max > cut; ++cut)
    {
        out << *_cuts[cut] << endl;
    }
    out << **(_cuts.end() - 1);
}



// Muon Selector
//
MuonSelector::MuonSelector()
{
    _pt.reset(new Comparator<>(30, "pT"));
    _eta.reset(new Comparator<std::less<float> >(2.1, "|eta|"));
    _is_global.reset(new Comparator<std::logical_and<bool> >(true, "is Global"));
    _is_tracker.reset(new Comparator<std::logical_and<bool> >(true, "is Tracker"));
    _muon_segments.reset(new Comparator<>(1, "Muon Segments"));
    _muon_hits.reset(new Comparator<>(0, "Muon Hits"));
    _muon_normalized_chi2.reset(new Comparator<std::less<float> >(10, "Muon Chi2 / ndof"));
    _tracker_hits.reset(new Comparator<>(10, "Tracker hits"));
    _pixel_hits.reset(new Comparator<>(0, "Pixel hits"));
    _d0_bsp.reset(new Comparator<std::less<float> >(0.02, "|d0_bsp|"));
    _primary_vertex.reset(new Comparator<std::less<float> >(1, "|mu.z() - pv.z()|"));

    monitor(_pt);
    monitor(_eta);
    monitor(_is_global);
    monitor(_is_tracker);
    monitor(_muon_segments);
    monitor(_muon_hits);
    monitor(_muon_normalized_chi2);
    monitor(_tracker_hits);
    monitor(_pixel_hits);
    monitor(_d0_bsp);
    monitor(_primary_vertex);
}

MuonSelector::MuonSelector(const MuonSelector &object)
{
    _pt = dynamic_pointer_cast<Cut>(object._pt->clone());
    _eta = dynamic_pointer_cast<Cut>(object._eta->clone());
    _is_global = dynamic_pointer_cast<Cut>(object._is_global->clone());
    _is_tracker = dynamic_pointer_cast<Cut>(object._is_tracker->clone());
    _muon_segments = dynamic_pointer_cast<Cut>(object._muon_segments->clone());
    _muon_hits = dynamic_pointer_cast<Cut>(object._muon_hits->clone());
    _muon_normalized_chi2 = dynamic_pointer_cast<Cut>(object._muon_normalized_chi2->clone());
    _tracker_hits = dynamic_pointer_cast<Cut>(object._tracker_hits->clone());
    _pixel_hits = dynamic_pointer_cast<Cut>(object._pixel_hits->clone());
    _d0_bsp = dynamic_pointer_cast<Cut>(object._d0_bsp->clone());
    _primary_vertex = dynamic_pointer_cast<Cut>(object._primary_vertex->clone());

    monitor(_pt);
    monitor(_eta);
    monitor(_is_global);
    monitor(_is_tracker);
    monitor(_muon_segments);
    monitor(_muon_hits);
    monitor(_muon_normalized_chi2);
    monitor(_tracker_hits);
    monitor(_pixel_hits);
    monitor(_d0_bsp);
    monitor(_primary_vertex);
}

bool MuonSelector::apply(const Muon &muon, const PrimaryVertex &pv)
{
    return muon.has_extra()
        && _pt->apply(bsm::pt(muon.physics_object().p4()))
        && _eta->apply(fabs(bsm::eta(muon.physics_object().p4())))
        && _is_global->apply(muon.extra().is_global())
        && _is_tracker->apply(muon.extra().is_tracker())
        && _muon_segments->apply(muon.extra().number_of_matches())
        && _muon_hits->apply(muon.global_track().hits())
        && _muon_normalized_chi2->apply(muon.global_track().normalized_chi2())
        && _tracker_hits->apply(muon.inner_track().hits())
        && _pixel_hits->apply(muon.extra().pixel_hits())
        && _d0_bsp->apply(fabs(muon.extra().d0_bsp()))
        && _primary_vertex->apply(fabs(muon.physics_object().vertex().z() - pv.vertex().z()));
}

CutPtr MuonSelector::pt() const
{
    return _pt;
}

CutPtr MuonSelector::eta() const
{
    return _eta;
}

CutPtr MuonSelector::is_global() const
{
    return _is_global;
}

CutPtr MuonSelector::is_tracker() const
{
    return _is_tracker;
}

CutPtr MuonSelector::muon_segments() const
{
    return _muon_segments;
}

CutPtr MuonSelector::muon_hits() const
{
    return _muon_hits;
}

CutPtr MuonSelector::muon_normalized_chi2() const
{
    return _muon_normalized_chi2;
}

CutPtr MuonSelector::tracker_hits() const
{
    return _tracker_hits;
}

CutPtr MuonSelector::pixel_hits() const
{
    return _pixel_hits;
}

CutPtr MuonSelector::d0_bsp() const
{
    return _d0_bsp;
}

CutPtr MuonSelector::primary_vertex() const
{
    return _primary_vertex;
}

void MuonSelector::enable()
{
    pt()->enable();
    eta()->enable();
    is_global()->enable();
    is_tracker()->enable();
    muon_segments()->enable();
    muon_hits()->enable();
    muon_normalized_chi2()->enable();
    tracker_hits()->enable();
    pixel_hits()->enable();
    d0_bsp()->enable();
    primary_vertex()->enable();
}

void MuonSelector::disable()
{
    pt()->disable();
    eta()->disable();
    is_global()->disable();
    is_tracker()->disable();
    muon_segments()->disable();
    muon_hits()->disable();
    muon_normalized_chi2()->disable();
    tracker_hits()->disable();
    pixel_hits()->disable();
    d0_bsp()->disable();
    primary_vertex()->disable();
}

uint32_t MuonSelector::id() const
{
    return core::ID<MuonSelector>::get();
}

MuonSelector::ObjectPtr MuonSelector::clone() const
{
    return ObjectPtr(new MuonSelector(*this));
}

void MuonSelector::print(std::ostream &out) const
{
    out << "     CUT                 " << setw(5) << " "
        << " Objects Events" << endl;
    out << setw(45) << setfill('-') << left << " " << setfill(' ') << endl;
    out << *_pt << endl;
    out << *_eta << endl;
    out << *_is_global << endl;
    out << *_is_tracker << endl;
    out << *_muon_segments << endl;
    out << *_muon_hits << endl;
    out << *_muon_normalized_chi2 << endl;
    out << *_tracker_hits << endl;
    out << *_pixel_hits << endl;
    out << *_d0_bsp << endl;
    out << *_primary_vertex;
}



// PrimaryVertex Selector
//
PrimaryVertexSelector::PrimaryVertexSelector()
{
    _ndof.reset(new Comparator<std::greater_equal<float> >(4, "ndof"));
    _vertex_z.reset(new Comparator<std::less_equal<float> >(24, "|pv.z()|"));
    _rho.reset(new Comparator<std::less_equal<float> >(4.0, "pv.rho()"));

    monitor(_ndof);
    monitor(_vertex_z);
    monitor(_rho);
}

PrimaryVertexSelector::PrimaryVertexSelector(const PrimaryVertexSelector &object)
{
    _ndof = dynamic_pointer_cast<Cut>(object._ndof->clone());
    _vertex_z = dynamic_pointer_cast<Cut>(object._vertex_z->clone());
    _rho = dynamic_pointer_cast<Cut>(object._rho->clone());

    monitor(_ndof);
    monitor(_vertex_z);
    monitor(_rho);
}

bool PrimaryVertexSelector::apply(const PrimaryVertex &pv)
{
    return pv.has_extra()
        && _ndof->apply(pv.extra().ndof())
        && _vertex_z->apply(pv.vertex().z())
        && _rho->apply(pv.extra().rho());
}

CutPtr PrimaryVertexSelector::ndof() const
{
    return _ndof;
}

CutPtr PrimaryVertexSelector::vertex_z() const
{
    return _vertex_z;
}

CutPtr PrimaryVertexSelector::rho() const
{
    return _rho;
}

void PrimaryVertexSelector::enable()
{
    ndof()->enable();
    vertex_z()->enable();
    rho()->enable();
}

void PrimaryVertexSelector::disable()
{
    ndof()->disable();
    vertex_z()->disable();
    rho()->disable();
}

uint32_t PrimaryVertexSelector::id() const
{
    return core::ID<PrimaryVertexSelector>::get();
}

PrimaryVertexSelector::ObjectPtr PrimaryVertexSelector::clone() const
{
    return ObjectPtr(new PrimaryVertexSelector(*this));
}

void PrimaryVertexSelector::print(std::ostream &out) const
{
    out << "     CUT                 " << setw(5) << " "
        << " Objects Events" << endl;
    out << setw(45) << setfill('-') << left << " " << setfill(' ') << endl;
    out << *_ndof << endl;
    out << *_vertex_z << endl;
    out << *_rho;
}



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

JetEnergyCorrectionDelegate *SynchSelector::getJetEnergyCorrectionDelegate() const
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



// WJetSelector
//
WJetSelector::WJetSelector()
{
    _children.reset(new Comparator<std::equal_to<uint32_t> >(2, "Children"));
    _pt.reset(new Comparator<>(200, "pT"));
    _mass_drop.reset(new Comparator<std::less<float> >(0.4, "Mass drop"));
    _mass_lower_bound.reset(new Comparator<>(60, "Mass lower bound"));
    _mass_upper_bound.reset(new Comparator<std::less<float> >(130, "Mass upper bound"));

    monitor(_children);
    monitor(_pt);
    monitor(_mass_drop);
    monitor(_mass_lower_bound);
    monitor(_mass_upper_bound);
}

WJetSelector::WJetSelector(const WJetSelector &object)
{
    _children = dynamic_pointer_cast<Cut>(object._children->clone());;
    _pt = dynamic_pointer_cast<Cut>(object._pt->clone());;
    _mass_drop = dynamic_pointer_cast<Cut>(object._mass_drop->clone());;
    _mass_lower_bound = dynamic_pointer_cast<Cut>(object._mass_lower_bound->clone());;
    _mass_upper_bound = dynamic_pointer_cast<Cut>(object._mass_upper_bound->clone());;

    monitor(_children);
    monitor(_pt);
    monitor(_mass_drop);
    monitor(_mass_lower_bound);
    monitor(_mass_upper_bound);
}

bool WJetSelector::apply(const Jet &jet)
{
    if (!_children->apply(jet.children().size()))
        return false;

    if (!_pt->apply(bsm::pt(jet.physics_object().p4())))
        return false;

    float m0 = bsm::mass(jet.physics_object().p4());
    float m1 = bsm::mass(jet.children().Get(0).physics_object().p4());
    float m2 = bsm::mass(jet.children().Get(1).physics_object().p4());
    float m12 = bsm::mass(jet.children().Get(0).physics_object().p4()
            + jet.children().Get(1).physics_object().p4());

    return _mass_drop->apply(std::max(m1, m2) / m0)
        && _mass_lower_bound->apply(m12)
        && _mass_upper_bound->apply(m12);
}

CutPtr WJetSelector::children() const
{
    return _children;
}

CutPtr WJetSelector::pt() const
{
    return _pt;
}

CutPtr WJetSelector::mass_drop() const
{
    return _mass_drop;
}

CutPtr WJetSelector::mass_lower_bound() const
{
    return _mass_lower_bound;
}

CutPtr WJetSelector::mass_upper_bound() const
{
    return _mass_upper_bound;
}

void WJetSelector::enable()
{
    children()->enable();
    pt()->enable();
    mass_drop()->enable();
    mass_lower_bound()->enable();
    mass_upper_bound()->enable();
}

void WJetSelector::disable()
{
    children()->disable();
    pt()->disable();
    mass_drop()->disable();
    mass_lower_bound()->disable();
    mass_upper_bound()->disable();
}

uint32_t WJetSelector::id() const
{
    return core::ID<WJetSelector>::get();
}

WJetSelector::ObjectPtr WJetSelector::clone() const
{
    return ObjectPtr(new WJetSelector(*this));
}

void WJetSelector::print(std::ostream &out) const
{
    out << "     CUT                 " << setw(5) << " "
        << " Objects Events" << endl;
    out << setw(45) << setfill('-') << left << " " << setfill(' ') << endl;
    out << *_children << endl;
    out << *_pt << endl;
    out << *_mass_drop << endl;
    out << *_mass_lower_bound << endl;
    out << *_mass_upper_bound;
}



// Lock Selector Event Counter on Update
//
LockSelectorEventCounterOnUpdate::LockSelectorEventCounterOnUpdate(
        ElectronSelector &selector)
{
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.pt()->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.eta()->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.primary_vertex()->events())));
}

LockSelectorEventCounterOnUpdate::LockSelectorEventCounterOnUpdate(
        JetSelector &selector)
{
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.pt()->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.eta()->events())));
}

LockSelectorEventCounterOnUpdate::LockSelectorEventCounterOnUpdate(
        MuonSelector &selector)
{
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.pt()->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.eta()->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.is_global()->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.is_tracker()->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.muon_segments()->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.muon_hits()->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.muon_normalized_chi2()->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.tracker_hits()->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.pixel_hits()->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.d0_bsp()->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.primary_vertex()->events())));
}

LockSelectorEventCounterOnUpdate::LockSelectorEventCounterOnUpdate(
        WJetSelector &selector)
{
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.children()->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.pt()->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.mass_drop()->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.mass_lower_bound()->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.mass_upper_bound()->events())));
}



// Helpers
//
std::ostream &bsm::operator <<(std::ostream &out,
        const SynchSelector::LeptonMode &lepton_mode)
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
        const SynchSelector::CutMode &cut_mode)
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
