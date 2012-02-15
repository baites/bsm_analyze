// A collection of Monitors
//
// Monitors are used to easily check different quantities, physics objects,
// jets, etc.
//
// Created by Samvel Khalatyan, Apr 22, 2011
// Copyright 2011, All rights reserved

#include <iomanip>
#include <ostream>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/GenParticle.pb.h"
#include "bsm_input/interface/MissingEnergy.pb.h"
#include "bsm_input/interface/Physics.pb.h"
#include "bsm_stat/interface/H1.h"
#include "bsm_stat/interface/H2.h"
#include "bsm_stat/interface/Utility.h"

#include "interface/Monitor.h"
#include "interface/StatProxy.h"
#include "interface/Utility.h"

using std::endl;
using std::left;
using std::setw;

using bsm::DeltaMonitor;
using bsm::ElectronsMonitor;
using bsm::GenParticleMonitor;
using bsm::JetsMonitor;
using bsm::P4Monitor;
using bsm::MissingEnergyMonitor;
using bsm::MuonsMonitor;
using bsm::PrimaryVerticesMonitor;

using bsm::stat::H1;
using bsm::stat::H2;

using bsm::H1Ptr;
using bsm::H2Ptr;

// Delta Monitor
//
DeltaMonitor::DeltaMonitor()
{
    _r.reset(new H1Proxy(50, 0, 5));
    _eta.reset(new H1Proxy(100, -5, 5));
    _phi.reset(new H1Proxy(80, -4, 4));
    _ptrel.reset(new H1Proxy(100, 0, 10));
    _angle.reset(new H1Proxy(80, -4, 4));
    _ptrel_vs_r.reset(new H2Proxy(100, 0, 10, 50, 0, 5));

    monitor(_r);
    monitor(_eta);
    monitor(_phi);
    monitor(_ptrel);
    monitor(_angle);
    monitor(_ptrel_vs_r);
}

DeltaMonitor::DeltaMonitor(const DeltaMonitor &object)
{
    _r.reset(new H1Proxy(*object._r));
    _eta.reset(new H1Proxy(*object._eta));
    _phi.reset(new H1Proxy(*object._phi));
    _ptrel.reset(new H1Proxy(*object._ptrel));
    _angle.reset(new H1Proxy(*object._angle));
    _ptrel_vs_r.reset(new H2Proxy(*object._ptrel_vs_r));

    monitor(_r);
    monitor(_eta);
    monitor(_phi);
    monitor(_ptrel);
    monitor(_angle);
    monitor(_ptrel_vs_r);
}

void DeltaMonitor::fill(const LorentzVector &p1,
                        const LorentzVector &p2,
                        const float &weight)
{
    r()->fill(dr(p1, p2), weight);
    eta()->fill(bsm::eta(p1) - bsm::eta(p2), weight);
    phi()->fill(dphi(p1, p2), weight);
    ptrel()->fill(bsm::ptrel(p1, p2), weight);
    angle()->fill(bsm::angle(p1, p2), weight);

    ptrel_vs_r()->fill(bsm::ptrel(p1, p2), dr(p1, p2), weight);
}

const H1Ptr DeltaMonitor::r() const
{
    return _r->histogram();
}

const H1Ptr DeltaMonitor::eta() const
{
    return _eta->histogram();
}

const H1Ptr DeltaMonitor::phi() const
{
    return _phi->histogram();
}

const H1Ptr DeltaMonitor::ptrel() const
{
    return _ptrel->histogram();
}

const H1Ptr DeltaMonitor::angle() const
{
    return _angle->histogram();
}

const H2Ptr DeltaMonitor::ptrel_vs_r() const
{
    return _ptrel_vs_r->histogram();
}

uint32_t DeltaMonitor::id() const
{
    return core::ID<DeltaMonitor>::get();
}

DeltaMonitor::ObjectPtr DeltaMonitor::clone() const
{
    return ObjectPtr(new DeltaMonitor(*this));
}

void DeltaMonitor::print(std::ostream &out) const
{
    out << setw(15) << left << " [R]" << *r() << endl;
    out << setw(15) << left << " [eta]" << *eta() << endl;
    out << setw(15) << left << " [phi] " << *phi() << endl;
    out << setw(15) << left << " [pTrel]" << *ptrel() << endl;
    out << setw(15) << left << " [angle]" << *angle() << endl;
    out << setw(14) << left << " [pTrel vs R]" << *ptrel_vs_r();
}



// Electrons Monitor
//
ElectronsMonitor::ElectronsMonitor()
{
    _multiplicity.reset(new H1Proxy(10, 0, 10));
    _pt.reset(new H1Proxy(100, 0, 100));
    _leading_pt.reset(new H1Proxy(100, 0, 100));

    monitor(_multiplicity);
    monitor(_pt);
    monitor(_leading_pt);
}

ElectronsMonitor::ElectronsMonitor(const ElectronsMonitor &object)
{
    _multiplicity.reset(new H1Proxy(*object._multiplicity));
    _pt.reset(new H1Proxy(*object._pt));
    _leading_pt.reset(new H1Proxy(*object._leading_pt));

    monitor(_multiplicity);
    monitor(_pt);
    monitor(_leading_pt);
}

void ElectronsMonitor::fill(const Electrons &electrons, const float &weight)
{
    multiplicity()->fill(electrons.size(), weight);

    float max_el_pt = 0;
    float el_pt = 0;
    for(Electrons::const_iterator electron = electrons.begin();
            electrons.end() != electron;
            ++electron)
    {
        el_pt = bsm::pt(electron->physics_object().p4());

        pt()->fill(el_pt, weight);

        if (el_pt <= max_el_pt)
            continue;

        max_el_pt = el_pt;
    }

    if (max_el_pt)
        leading_pt()->fill(max_el_pt, weight);
}

const H1Ptr ElectronsMonitor::multiplicity() const
{
    return _multiplicity->histogram();
}

const H1Ptr ElectronsMonitor::pt() const
{
    return _pt->histogram();
}

const H1Ptr ElectronsMonitor::leading_pt() const
{
    return _leading_pt->histogram();
}

uint32_t ElectronsMonitor::id() const
{
    return core::ID<ElectronsMonitor>::get();
}

ElectronsMonitor::ObjectPtr ElectronsMonitor::clone() const
{
    return ObjectPtr(new ElectronsMonitor(*this));
}

void ElectronsMonitor::print(std::ostream &out) const
{
    out << setw(16) << left << " [multiplicity]" << *multiplicity() << endl;
    out << setw(16) << left << " [pt]" << *pt() << endl;
    out << setw(16) << left << " [leading pt] " << *leading_pt();
}



// Jets Monitor
//
JetsMonitor::JetsMonitor()
{
    _multiplicity.reset(new H1Proxy(10, 0, 10));
    _pt.reset(new H1Proxy(100, 0, 100));
    _uncorrected_pt.reset(new H1Proxy(100, 0, 100));
    _leading_pt.reset(new H1Proxy(100, 0, 100));
    _leading_uncorrected_pt.reset(new H1Proxy(100, 0, 100));
    _children.reset(new H1Proxy(10, 0, 10));

    monitor(_multiplicity);
    monitor(_pt);
    monitor(_uncorrected_pt);
    monitor(_leading_pt);
    monitor(_leading_uncorrected_pt);
    monitor(_children);
}

JetsMonitor::JetsMonitor(const JetsMonitor &object)
{
    _multiplicity.reset(new H1Proxy(*object._multiplicity));
    _pt.reset(new H1Proxy(*object._pt));
    _uncorrected_pt.reset(new H1Proxy(*object._uncorrected_pt));
    _leading_pt.reset(new H1Proxy(*object._leading_pt));
    _leading_uncorrected_pt.reset(new H1Proxy(*object._leading_uncorrected_pt));
    _children.reset(new H1Proxy(*object._children));

    monitor(_multiplicity);
    monitor(_pt);
    monitor(_uncorrected_pt);
    monitor(_leading_pt);
    monitor(_leading_uncorrected_pt);
    monitor(_children);
}

void JetsMonitor::fill(const Jets &jets, const float &weight)
{
    multiplicity()->fill(jets.size(), weight);

    float max_jet_pt = 0;
    float max_jet_uncorrected_pt = 0;
    float jet_pt = 0;
    float jet_uncorrected_pt = 0;
    for(Jets::const_iterator jet = jets.begin();
            jets.end() != jet;
            ++jet)
    {
        children()->fill(jet->child().size(), weight);

        jet_pt = bsm::pt(jet->physics_object().p4());
        jet_uncorrected_pt = 0;

        pt()->fill(jet_pt, weight);

        if (jet->has_uncorrected_p4())
        {
            jet_uncorrected_pt = bsm::pt(jet->uncorrected_p4());

            uncorrected_pt()->fill(jet_uncorrected_pt, weight);
        }

        if (jet_pt <= max_jet_pt)
            continue;

        max_jet_pt = jet_pt;
        max_jet_uncorrected_pt = jet_uncorrected_pt;
    }

    if (max_jet_pt)
    {
        leading_pt()->fill(max_jet_pt);
        leading_uncorrected_pt()->fill(max_jet_uncorrected_pt, weight);
    }
}

const H1Ptr JetsMonitor::multiplicity() const
{
    return _multiplicity->histogram();
}

const H1Ptr JetsMonitor::pt() const
{
    return _pt->histogram();
}

const H1Ptr JetsMonitor::uncorrected_pt() const
{
    return _uncorrected_pt->histogram();
}

const H1Ptr JetsMonitor::leading_pt() const
{
    return _leading_pt->histogram();
}

const H1Ptr JetsMonitor::leading_uncorrected_pt() const
{
    return _leading_uncorrected_pt->histogram();
}

const H1Ptr JetsMonitor::children() const
{
    return _children->histogram();
}

uint32_t JetsMonitor::id() const
{
    return core::ID<JetsMonitor>::get();
}

JetsMonitor::ObjectPtr JetsMonitor::clone() const
{
    return ObjectPtr(new JetsMonitor(*this));
}

void JetsMonitor::print(std::ostream &out) const
{
    out << setw(16) << left << " [multiplicity]"
        << *multiplicity() << endl;
    out << setw(16) << left << " [pt]" << *pt() << endl;
    out << setw(16) << left << " [uncorrected pt]" << *uncorrected_pt() << endl;
    out << setw(16) << left << " [leading uncorrected pt] " << *leading_uncorrected_pt()
        << endl;
    out << setw(16) << left << " [leading pt] " << *leading_pt()
        << endl;
    out << setw(16) << left << " [children]" << *children();
}



// Lorentz Vector Monitor
//
P4Monitor::P4Monitor()
{
    _energy.reset(new H1Proxy(100, 0, 100));
    _px.reset(new H1Proxy(100, 0, 100));
    _py.reset(new H1Proxy(100, 0, 100));
    _pz.reset(new H1Proxy(100, 0, 100));
    _pt.reset(new H1Proxy(500, 0, 500));
    _eta.reset(new H1Proxy(1000, -5, 5));
    _phi.reset(new H1Proxy(800, -4, 4));
    _mass.reset(new H1Proxy(500, 0, 500));
    _mt.reset(new H1Proxy(500, 0, 500));
    _et.reset(new H1Proxy(100, 0, 100));

    monitor(_energy);
    monitor(_px);
    monitor(_py);
    monitor(_pz);
    monitor(_pt);
    monitor(_eta);
    monitor(_phi);
    monitor(_mass);
    monitor(_mt);
    monitor(_et);
}

P4Monitor::P4Monitor(const P4Monitor &object)
{
    _energy.reset(new H1Proxy(*object._energy));
    _px.reset(new H1Proxy(*object._px));
    _py.reset(new H1Proxy(*object._py));
    _pz.reset(new H1Proxy(*object._pz));
    _pt.reset(new H1Proxy(*object._pt));
    _eta.reset(new H1Proxy(*object._eta));
    _phi.reset(new H1Proxy(*object._phi));
    _mass.reset(new H1Proxy(*object._mass));
    _mt.reset(new H1Proxy(*object._mt));
    _et.reset(new H1Proxy(*object._et));

    monitor(_energy);
    monitor(_px);
    monitor(_py);
    monitor(_pz);
    monitor(_pt);
    monitor(_eta);
    monitor(_phi);
    monitor(_mass);
    monitor(_mt);
    monitor(_et);
}

void P4Monitor::fill(const LorentzVector &p4, const float &weight)
{
    energy()->fill(p4.e(), weight);
    px()->fill(p4.px(), weight);
    py()->fill(p4.py(), weight);
    pz()->fill(p4.pz(), weight);

    pt()->fill(bsm::pt(p4), weight);
    eta()->fill(bsm::eta(p4), weight);
    phi()->fill(bsm::phi(p4), weight);
    mass()->fill(bsm::mass(p4), weight);

    mt()->fill(bsm::mt(p4), weight);
    et()->fill(bsm::et(p4), weight);
}

const H1Ptr P4Monitor::energy() const
{
    return _energy->histogram();
}

const H1Ptr P4Monitor::px() const
{
    return _px->histogram();
}

const H1Ptr P4Monitor::py() const
{
    return _py->histogram();
}
 
const H1Ptr P4Monitor::pz() const
{
    return _pz->histogram();
}

const H1Ptr P4Monitor::pt() const
{
    return _pt->histogram();
}

const H1Ptr P4Monitor::eta() const
{
    return _eta->histogram();
}

const H1Ptr P4Monitor::phi() const
{
    return _phi->histogram();
}

const H1Ptr P4Monitor::mass() const
{
    return _mass->histogram();
}

const H1Ptr P4Monitor::mt() const
{
    return _mt->histogram();
}

const H1Ptr P4Monitor::et() const
{
    return _et->histogram();
}

uint32_t P4Monitor::id() const
{
    return core::ID<P4Monitor>::get();
}

P4Monitor::ObjectPtr P4Monitor::clone() const
{
    return ObjectPtr(new P4Monitor(*this));
}

void P4Monitor::print(std::ostream &out) const
{
    out << setw(16) << left << " [e]" << *energy() << endl;
    out << setw(16) << left << " [px]" << *px() << endl;
    out << setw(16) << left << " [py]" << *py() << endl;
    out << setw(16) << left << " [pz]" << *pz() << endl;
    out << setw(16) << left << " [pt]" << *pt() << endl;
    out << setw(16) << left << " [eta]" << *eta() << endl;
    out << setw(16) << left << " [phi]" << *phi() << endl;
    out << setw(16) << left << " [mass]" << *mass() << endl;
    out << setw(16) << left << " [mt]" << *mt() << endl;
    out << setw(16) << left << " [et]" << *et();
}



// Gen Particle Monitor
//
GenParticleMonitor::GenParticleMonitor()
{
    _pdg_id.reset(new H1Proxy(100, -50, 50));
    _status.reset(new H1Proxy(10, 0, 10));

    monitor(_pdg_id);
    monitor(_status);
}

GenParticleMonitor::GenParticleMonitor(const GenParticleMonitor &object):
    P4Monitor(object)
{
    _pdg_id.reset(new H1Proxy(*object._pdg_id));
    _status.reset(new H1Proxy(*object._status));

    monitor(_pdg_id);
    monitor(_status);
}

void GenParticleMonitor::fill(const GenParticle &particle, const float &weight)
{
    pdg_id()->fill(particle.id(), weight);
    status()->fill(particle.status(), weight);

    P4Monitor::fill(particle.physics_object().p4(), weight);
}

const H1Ptr GenParticleMonitor::pdg_id() const
{
    return _pdg_id->histogram();
}

const H1Ptr GenParticleMonitor::status() const
{
    return _status->histogram();
}

uint32_t GenParticleMonitor::id() const
{
    return core::ID<GenParticleMonitor>::get();
}

GenParticleMonitor::ObjectPtr GenParticleMonitor::clone() const
{
    return ObjectPtr(new GenParticleMonitor(*this));
}

void GenParticleMonitor::print(std::ostream &out) const
{
    out << setw(10) << left << " [PDG id]" << *pdg_id() << endl;
    out << setw(10) << left << " [status]" << *status() << endl;

    P4Monitor::print(out);
}



// Missing Energy Monitor
//
MissingEnergyMonitor::MissingEnergyMonitor()
{
    _pt.reset(new H1Proxy(100, 0, 100));
    _x.reset(new H1Proxy(100, -50, 50));
    _y.reset(new H1Proxy(100, -50, 50));
    _z.reset(new H1Proxy(100, -50, 50));

    monitor(_pt);
    monitor(_x);
    monitor(_y);
    monitor(_z);
}

MissingEnergyMonitor::MissingEnergyMonitor(const MissingEnergyMonitor &object)
{
    _pt.reset(new H1Proxy(*object._pt));
    _x.reset(new H1Proxy(*object._x));
    _y.reset(new H1Proxy(*object._y));
    _z.reset(new H1Proxy(*object._z));

    monitor(_pt);
    monitor(_x);
    monitor(_y);
    monitor(_z);
}

void MissingEnergyMonitor::fill(const MissingEnergy &missing_energy, const float &weight)
{
    pt()->fill(bsm::pt(missing_energy.p4()), weight);
}

const H1Ptr MissingEnergyMonitor::pt() const
{
    return _pt->histogram();
}

const H1Ptr MissingEnergyMonitor::x() const
{
    return _x->histogram();
}

const H1Ptr MissingEnergyMonitor::y() const
{
    return _y->histogram();
}

const H1Ptr MissingEnergyMonitor::z() const
{
    return _z->histogram();
}

uint32_t MissingEnergyMonitor::id() const
{
    return core::ID<MissingEnergyMonitor>::get();
}

MissingEnergyMonitor::ObjectPtr MissingEnergyMonitor::clone() const
{
    return ObjectPtr(new MissingEnergyMonitor(*this));
}

void MissingEnergyMonitor::print(std::ostream &out) const
{
    out << setw(16) << left << " [pt]" << *pt() << endl;
    out << setw(16) << left << " [x]" << *x() << endl;
    out << setw(16) << left << " [y]" << *y() << endl;
    out << setw(16) << left << " [z]" << *z();
}



// Muons Monitor
//
MuonsMonitor::MuonsMonitor()
{
    _multiplicity.reset(new H1Proxy(10, 0, 10));
    _pt.reset(new H1Proxy(100, 0, 100));
    _leading_pt.reset(new H1Proxy(100, 0, 100));

    monitor(_multiplicity);
    monitor(_pt);
    monitor(_leading_pt);
}

MuonsMonitor::MuonsMonitor(const MuonsMonitor &object)
{
    _multiplicity.reset(new H1Proxy(*object._multiplicity));
    _pt.reset(new H1Proxy(*object._pt));
    _leading_pt.reset(new H1Proxy(*object._leading_pt));

    monitor(_multiplicity);
    monitor(_pt);
    monitor(_leading_pt);
}

void MuonsMonitor::fill(const Muons &muons, const float &weight)
{
    multiplicity()->fill(muons.size(), weight);

    float max_muon_pt = 0;
    float muon_pt = 0;
    for(Muons::const_iterator muon = muons.begin();
            muons.end() != muon;
            ++muon)
    {
        muon_pt = bsm::pt(muon->physics_object().p4());

        pt()->fill(muon_pt, weight);

        if (muon_pt <= max_muon_pt)
            continue;

        max_muon_pt = muon_pt;
    }

    if (max_muon_pt)
        leading_pt()->fill(max_muon_pt, weight);
}

const H1Ptr MuonsMonitor::multiplicity() const
{
    return _multiplicity->histogram();
}

const H1Ptr MuonsMonitor::pt() const
{
    return _pt->histogram();
}

const H1Ptr MuonsMonitor::leading_pt() const
{
    return _leading_pt->histogram();
}

uint32_t MuonsMonitor::id() const
{
    return core::ID<MuonsMonitor>::get();
}

MuonsMonitor::ObjectPtr MuonsMonitor::clone() const
{
    return ObjectPtr(new MuonsMonitor(*this));
}

void MuonsMonitor::print(std::ostream &out) const
{
    out << setw(16) << left << " [multiplicity]"
        << *multiplicity() << endl;
    out << setw(16) << left << " [pt]" << *pt() << endl;
    out << setw(16) << left << " [leading pt] " << *leading_pt();
}



// Primary Vertices Monitor
//
PrimaryVerticesMonitor::PrimaryVerticesMonitor()
{
    _multiplicity.reset(new H1Proxy(20, 0, 20));
    _x.reset(new H1Proxy(200, -.1, .1));
    _y.reset(new H1Proxy(200, -.1, .1));
    _z.reset(new H1Proxy(100, -50, 50));

    monitor(_multiplicity);
    monitor(_x);
    monitor(_y);
    monitor(_z);
}

PrimaryVerticesMonitor::PrimaryVerticesMonitor(const PrimaryVerticesMonitor &object)
{
    _multiplicity.reset(new H1Proxy(*object._multiplicity));
    _x.reset(new H1Proxy(*object._x));
    _y.reset(new H1Proxy(*object._y));
    _z.reset(new H1Proxy(*object._z));

    monitor(_multiplicity);
    monitor(_x);
    monitor(_y);
    monitor(_z);
}

void PrimaryVerticesMonitor::fill(const PrimaryVertices &primary_vertices,
        const float &weight)
{
    multiplicity()->fill(primary_vertices.size(), weight);

    for(PrimaryVertices::const_iterator primary_vertex = primary_vertices.begin();
            primary_vertices.end() != primary_vertex;
            ++primary_vertex)
    {
        x()->fill(primary_vertex->vertex().x(), weight);
        y()->fill(primary_vertex->vertex().y(), weight);
        z()->fill(primary_vertex->vertex().z(), weight);
    }
}

const H1Ptr PrimaryVerticesMonitor::multiplicity() const
{
    return _multiplicity->histogram();
}

const H1Ptr PrimaryVerticesMonitor::x() const
{
    return _x->histogram();
}

const H1Ptr PrimaryVerticesMonitor::y() const
{
    return _y->histogram();
}

const H1Ptr PrimaryVerticesMonitor::z() const
{
    return _z->histogram();
}

uint32_t PrimaryVerticesMonitor::id() const
{
    return core::ID<PrimaryVerticesMonitor>::get();
}

PrimaryVerticesMonitor::ObjectPtr PrimaryVerticesMonitor::clone() const
{
    return ObjectPtr(new PrimaryVerticesMonitor(*this));
}

void PrimaryVerticesMonitor::print(std::ostream &out) const
{
    out << setw(16) << left << " [multiplicity]"
        << *multiplicity() << endl;
    out << setw(16) << left << " [x]" << *x() << endl;
    out << setw(16) << left << " [y]" << *y() << endl;
    out << setw(16) << left << " [z]" << *z();
}
