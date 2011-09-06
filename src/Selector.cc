// Common Selectors Classes/functions
//
// Physics objects selectors: muons, electrons, jets
//
// Created by Samvel Khalatyan, May 16, 2011
// Copyright 2011, All rights reserved

#include <cfloat>
#include <cmath>
#include <functional>
#include <iostream>
#include <iomanip>
#include <ostream>
#include <sstream>

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
#include "interface/Selector.h"
#include "interface/Utility.h"

using namespace std;

using boost::dynamic_pointer_cast;

using bsm::Selector;

using bsm::CutPtr;
using bsm::ElectronSelector;
using bsm::JetEnergyCorrectionDelegate;
using bsm::JetSelector;
using bsm::MultiplicityCutflow;
using bsm::MuonSelector;
using bsm::PrimaryVertexSelector;
using bsm::WJetSelector;
using bsm::LockSelectorEventCounterOnUpdate;

// Selector
//
Selector::Selector(const Selector &object)
{
    for(Cuts::const_iterator cut = object._cuts.begin();
            object._cuts.end() != cut;
            ++cut)
    {
        addCut(cut->first, dynamic_pointer_cast<Cut>(cut->second->clone()));
    }
}

void Selector::enable()
{
    for(Cuts::const_iterator cut = _cuts.begin();
            _cuts.end() != cut;
            ++cut)
    {
        cut->second->enable();
    }
}

void Selector::disable()
{
    for(Cuts::const_iterator cut = _cuts.begin();
            _cuts.end() != cut;
            ++cut)
    {
        cut->second->disable();
    }
}

void Selector::print(ostream &out) const
{
    if (_cuts.empty())
    {
        out << "no cuts available" << endl;

        return;
    }

    out << "     CUT                 " << setw(5) << " "
        << " Objects Events" << endl;
    out << setw(45) << setfill('-') << left << " " << setfill(' ') << endl;
    for(Cuts::const_iterator cut = _cuts.begin();
            _cuts.end() != cut;
            ++cut)
    {
        out << *cut->second << endl;
    }
}

// Protected
//
bsm::CutPtr Selector::getCut(const uint32_t &cut_id) const
{
    Cuts::const_iterator cut = _cuts.find(cut_id);
    if (_cuts.end() == cut)
        throw out_of_range("failed to get cut: it is not defined");

    return cut->second;
}

void Selector::addCut(const uint32_t &cut_id, const CutPtr &cut)
{
    if (!_cuts.insert(make_pair(cut_id, cut)).second)
        cerr << "failed to add cut " << cut_id << ": remove it first" << endl;
    else
        monitor(cut);
}

void Selector::removeCut(const uint32_t &cut_id)
{
    _cuts.erase(cut_id);
}

uint32_t Selector::cuts() const
{
    return _cuts.size();
}



// ElectronSelector
//
ElectronSelector::ElectronSelector()
{
    addCut(PT, CutPtr(new Comparator<>(30, "Pt")));
    addCut(ETA, CutPtr(new Comparator<less<float> >(2.5, "|eta|")));
    addCut(PRIMARY_VERTEX,
            CutPtr(new Comparator<less<float> >(1,
                    "|el.z() - pv.z()|")));
}

bool ElectronSelector::apply(const Electron &electron, const PrimaryVertex &pv)
{
    return cut(PT)->apply(bsm::pt(electron.physics_object().p4()))
        && cut(ETA)->apply(fabs(bsm::eta(electron.physics_object().p4())))
        && cut(PRIMARY_VERTEX)->apply(fabs(electron.physics_object().vertex().z()
                    - pv.vertex().z()));
}

CutPtr ElectronSelector::cut(const Cut &cut_id) const
{
    return getCut(cut_id);
}

uint32_t ElectronSelector::id() const
{
    return core::ID<ElectronSelector>::get();
}

ElectronSelector::ObjectPtr ElectronSelector::clone() const
{
    return ObjectPtr(new ElectronSelector(*this));
}



// JetSelector
//
JetSelector::JetSelector()
{
    addCut(PT, CutPtr(new Comparator<>(50, "Pt")));
    addCut(ETA, CutPtr(new Comparator<less<float> >(2.4, "|eta|")));
}

CutPtr JetSelector::cut(const Cut &cut_id) const
{
    return getCut(cut_id);
}

bool JetSelector::apply(const Jet &jet)
{
    return cut(PT)->apply(bsm::pt(jet.physics_object().p4()))
        && cut(ETA)->apply(fabs(bsm::eta(jet.physics_object().p4())));
}

uint32_t JetSelector::id() const
{
    return core::ID<JetSelector>::get();
}

JetSelector::ObjectPtr JetSelector::clone() const
{
    return ObjectPtr(new JetSelector(*this));
}



// Multiplicity Cutflow
//
MultiplicityCutflow::MultiplicityCutflow(const uint32_t &max)
{
    for(uint32_t i = 0; max > i; ++i)
    {
        addCut(i, CutPtr(new Comparator<equal_to<uint32_t> >(i)));
    }
    addCut(max, CutPtr(new Comparator<greater_equal<uint32_t> >(max)));
}

bsm::CutPtr MultiplicityCutflow::cut(const uint32_t &cut_id) const
{
    return getCut(cut_id);
}

bool MultiplicityCutflow::apply(const uint32_t &number)
{
    // It does not make sense to apply all cuts. Only Nth one:
    //
    if (cuts() > number)
        cut(number)->apply(number);
    else
        cut(cuts() - 1)->apply(number);

    return true;
}

uint32_t MultiplicityCutflow::id() const
{
    return core::ID<MultiplicityCutflow>::get();
}

MultiplicityCutflow::ObjectPtr MultiplicityCutflow::clone() const
{
    return ObjectPtr(new MultiplicityCutflow(*this));
}



// Muon Selector
//
MuonSelector::MuonSelector()
{
    addCut(PT, CutPtr(new Comparator<>(30, "pT")));
    addCut(ETA, CutPtr(new Comparator<less<float> >(2.1, "|eta|")));
    addCut(IS_GLOBAL,
            CutPtr(new Comparator<logical_and<bool> >(true, "is Global")));
    addCut(IS_TRACKER,
            CutPtr(new Comparator<logical_and<bool> >(true,
                    "is Tracker")));
    addCut(MUON_SEGMENTS, CutPtr(new Comparator<>(1, "Muon Segments")));
    addCut(MUON_HITS, CutPtr(new Comparator<>(0, "Muon Hits")));
    addCut(MUON_NORMALIZED_CHI2,
            CutPtr(new Comparator<less<float> >(10,
                    "Muon Chi2 / ndof")));
    addCut(TRACKER_HITS, CutPtr(new Comparator<>(10, "Tracker hits")));
    addCut(PIXEL_HITS, CutPtr(new Comparator<>(0, "Pixel hits")));
    addCut(D0, CutPtr(new Comparator<less<float> >(0.02, "|d0|")));
    addCut(PRIMARY_VERTEX,
            CutPtr(new Comparator<less<float> >(1, "|mu.z() - pv.z()|")));
}

CutPtr MuonSelector::cut(const Cut &cut_id) const
{
    return getCut(cut_id);
}

bool MuonSelector::apply(const Muon &muon, const PrimaryVertex &pv)
{
    return muon.has_extra()
        && cut(PT)->apply(bsm::pt(muon.physics_object().p4()))
        && cut(ETA)->apply(fabs(bsm::eta(muon.physics_object().p4())))
        && cut(IS_GLOBAL)->apply(muon.extra().is_global())
        && cut(IS_TRACKER)->apply(muon.extra().is_tracker())
        && cut(MUON_SEGMENTS)->apply(muon.extra().number_of_matches())
        && cut(MUON_HITS)->apply(muon.global_track().hits())
        && cut(MUON_NORMALIZED_CHI2)->apply(muon.global_track().normalized_chi2())
        && cut(TRACKER_HITS)->apply(muon.inner_track().hits())
        && cut(PIXEL_HITS)->apply(muon.extra().pixel_hits())
        && cut(D0)->apply(fabs(muon.extra().d0_bsp()))
        && cut(PRIMARY_VERTEX)->apply(fabs(muon.physics_object().vertex().z()
                    - pv.vertex().z()));
}

uint32_t MuonSelector::id() const
{
    return core::ID<MuonSelector>::get();
}

MuonSelector::ObjectPtr MuonSelector::clone() const
{
    return ObjectPtr(new MuonSelector(*this));
}



// PrimaryVertex Selector
//
PrimaryVertexSelector::PrimaryVertexSelector()
{
    addCut(NDOF, CutPtr(new Comparator<greater_equal<float> >(4, "ndof")));
    addCut(VERTEX_Z,
            CutPtr(new Comparator<less_equal<float> >(24, "|pv.z()|")));
    addCut(RHO,
            CutPtr(new Comparator<less_equal<float> >(4.0, "pv.rho()")));
}

CutPtr PrimaryVertexSelector::cut(const Cut &cut_id) const
{
    return getCut(cut_id);
}

bool PrimaryVertexSelector::apply(const PrimaryVertex &pv)
{
    return pv.has_extra()
        && cut(NDOF)->apply(pv.extra().ndof())
        && cut(VERTEX_Z)->apply(pv.vertex().z())
        && cut(RHO)->apply(pv.extra().rho());
}

uint32_t PrimaryVertexSelector::id() const
{
    return core::ID<PrimaryVertexSelector>::get();
}

PrimaryVertexSelector::ObjectPtr PrimaryVertexSelector::clone() const
{
    return ObjectPtr(new PrimaryVertexSelector(*this));
}



// WJetSelector
//
WJetSelector::WJetSelector()
{
    addCut(CHILDREN,
            CutPtr(new Comparator<equal_to<uint32_t> >(2, "Children")));
    addCut(PT, CutPtr(new Comparator<>(200, "pT")));
    addCut(MASS_DROP,
            CutPtr(new Comparator<less<float> >(0.4, "Mass drop")));
    addCut(MASS, CutPtr(new RangeComparator<>(60, 130, "Mass bound")));
}

CutPtr WJetSelector::cut(const Cut &cut_id) const
{
    return getCut(cut_id);
}

bool WJetSelector::apply(const Jet &jet)
{
    if (!cut(CHILDREN)->apply(jet.children().size()))
        return false;

    if (!cut(PT)->apply(bsm::pt(jet.physics_object().p4())))
        return false;

    float m0 = bsm::mass(jet.physics_object().p4());
    float m1 = bsm::mass(jet.children().Get(0).physics_object().p4());
    float m2 = bsm::mass(jet.children().Get(1).physics_object().p4());
    float m12 = bsm::mass(jet.children().Get(0).physics_object().p4()
            + jet.children().Get(1).physics_object().p4());

    return cut(MASS_DROP)->apply(max(m1, m2) / m0)
        && cut(MASS)->apply(m12);
}

uint32_t WJetSelector::id() const
{
    return core::ID<WJetSelector>::get();
}

WJetSelector::ObjectPtr WJetSelector::clone() const
{
    return ObjectPtr(new WJetSelector(*this));
}



// Lock Selector Event Counter on Update
//
LockSelectorEventCounterOnUpdate::LockSelectorEventCounterOnUpdate(
        ElectronSelector &selector)
{
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(ElectronSelector::PT)->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(ElectronSelector::ETA)->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(ElectronSelector::PRIMARY_VERTEX)->events())));
}

LockSelectorEventCounterOnUpdate::LockSelectorEventCounterOnUpdate(
        JetSelector &selector)
{
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(JetSelector::PT)->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(JetSelector::ETA)->events())));
}

LockSelectorEventCounterOnUpdate::LockSelectorEventCounterOnUpdate(
        MuonSelector &selector)
{
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(MuonSelector::PT)->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(MuonSelector::ETA)->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(MuonSelector::IS_GLOBAL)->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(MuonSelector::IS_TRACKER)->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(MuonSelector::MUON_SEGMENTS)->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(MuonSelector::MUON_HITS)->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(MuonSelector::MUON_NORMALIZED_CHI2)->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(MuonSelector::TRACKER_HITS)->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(MuonSelector::PIXEL_HITS)->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(MuonSelector::D0)->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(MuonSelector::PRIMARY_VERTEX)->events())));
}

LockSelectorEventCounterOnUpdate::LockSelectorEventCounterOnUpdate(
        WJetSelector &selector)
{
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(WJetSelector::CHILDREN)->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(WJetSelector::PT)->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(WJetSelector::MASS_DROP)->events())));
    _lockers.push_back(Locker(
                new LockCounterOnUpdate(selector.cut(WJetSelector::MASS)->events())));
}
