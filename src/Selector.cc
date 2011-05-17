// Common Selectors Classes/functions
//
// Physics objects selectors: muons, electrons, jets
//
// Created by Samvel Khalatyan, May 16, 2011
// Copyright 2011, All rights reserved

#include <cmath>
#include <ostream>

#include <boost/pointer_cast.hpp>

#include <TLorentzVector.h>

#include "bsm_input/interface/Electron.pb.h"
#include "bsm_input/interface/Physics.pb.h"
#include "bsm_input/interface/PrimaryVertex.pb.h"
#include "interface/Selector.h"
#include "interface/Utility.h"

using std::ostream;
using std::endl;

using bsm::selector::Cut;
using bsm::selector::Comparator;
using bsm::selector::ElectronSelector;
using bsm::selector::Selector;

// ElectronSelector
//
ElectronSelector::ElectronSelector()
{
    _et.reset(new Comparator<>(30));
    _eta.reset(new Comparator<std::less<double> >(2.5));
    _primary_vertex.reset(new Comparator<std::less<double> >(1));

    _p4.reset(new TLorentzVector());
}

ElectronSelector::~ElectronSelector()
{
}

bool ElectronSelector::operator()(const Electron &electron,
        const PrimaryVertex &pv)
{
    utility::set(_p4.get(), &electron.physics_object().p4());

    return _et->operator()(_p4->Et())
        && _eta->operator()(fabs(_p4->Eta()))
        && _primary_vertex->operator()(fabs(electron.physics_object().vertex().z() - pv.vertex().z()));
}

ElectronSelector::CutPtr ElectronSelector::et() const
{
    return _et;
}

ElectronSelector::CutPtr ElectronSelector::eta() const
{
    return _eta;
}

ElectronSelector::CutPtr ElectronSelector::primary_vertex() const
{
    return _primary_vertex;
}

void ElectronSelector::print(std::ostream &out) const
{
    out << " [+]                Et > " << _et->value() << " " << *_et << endl;
    out << " [+]             |eta| < " << _eta->value() << " " << *_eta << endl;
    out << " [+] |el.z() - pv.z()| < " << _primary_vertex->value()
        << " " << *_primary_vertex << endl;
}

Selector::SelectorPtr ElectronSelector::clone() const
{
    boost::shared_ptr<ElectronSelector>
        selector(new ElectronSelector());

    *selector->et() = *et();
    *selector->eta() = *eta();
    *selector->primary_vertex() = *primary_vertex();

    return selector;
}

void ElectronSelector::merge(const SelectorPtr &selector)
{
    if (!selector)
        return;

    boost::shared_ptr<ElectronSelector> el_selector =
        boost::dynamic_pointer_cast<ElectronSelector>(selector);
    if (!el_selector)
        return;

    *et() += *el_selector->et();
    *eta() += *el_selector->eta();
    *primary_vertex() += *el_selector->primary_vertex();
}



// Cut
//
Cut::Cut(const double &value):
    _value(value),
    _count(0)
{}

Cut &Cut::operator +=(const Cut &cut)
{
    _count += cut;

    return *this;
}

Cut::operator uint32_t() const
{
    return _count;
}

double Cut::value() const
{
    return _value;
}

bool Cut::operator()(const double &value)
{
    return isPass(value)
        ? (++_count, true)
        : false;
}



// Helpers
//
std::ostream &bsm::selector::operator <<(std::ostream &out, const Selector &s)
{
    s.print(out);

    return out;
}
