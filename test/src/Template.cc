// Input Template
//
// Created by Samvel Khalatyan, Dec 2, 2011
// Copyright 2011, All rights reserved

#include <ostream>

#include "interface/Template.h"

using namespace std;

Template::operator string() const
{
    switch(type())
    {
        case MET: return "met";
        case HTLEP: return "htlep";
        case NPV: return "npv_with_pileup";
        case NJET: return "njets";
        case TTBAR_MASS: return "mttbar_after_htlep";
        case TTBAR_PT: return "ttbar_pt";
        case WLEP_MT: return "wlep_mt";
        case WLEP_MASS: return "wlep_mass";
        case JET1_PT: return "First_jet/pt";
        case JET1_ETA: return "First_jet/eta";
        case JET2_PT: return "Second_jet/pt";
        case JET2_ETA: return "Second_jet/eta";
        case JET3_PT: return "Third_jet/pt";
        case JET3_ETA: return "Third_jet/eta";
        case ELECTRON_PT: return "Electron/pt";
        case ELECTRON_ETA: return "Electron/eta";
        case LTOP_PT: return "ltop/pt";
        case LTOP_ETA: return "ltop/eta";
        case LTOP_MASS: return "ltop/mass";
        case LTOP_MT: return "ltop/mt";
        case HTOP_PT: return "htop/pt";
        case HTOP_ETA: return "htop/eta";
        case HTOP_MASS: return "htop/mass";
        case HTOP_MT: return "htop/mt";

        default: return "unknown";
    }
}

string Template::repr() const
{
    switch(type())
    {
        case MET: return "met";
        case HTLEP: return "htlep";
        case NPV: return "npv_with_pileup";
        case NJET: return "njets";
        case TTBAR_MASS: return "mttbar_after_htlep";
        case TTBAR_PT: return "ttbar_pt";
        case WLEP_MT: return "wlep_mt";
        case WLEP_MASS: return "wlep_mass";
        case JET1_PT: return "first_jet_pt";
        case JET1_ETA: return "first_jet_eta";
        case JET2_PT: return "second_jet_pt";
        case JET2_ETA: return "second_jet_eta";
        case JET3_PT: return "third_jet_pt";
        case JET3_ETA: return "third_jet_eta";
        case ELECTRON_PT: return "electron_pt";
        case ELECTRON_ETA: return "electron_eta";
        case LTOP_PT: return "ltop_pt";
        case LTOP_ETA: return "ltop_eta";
        case LTOP_MASS: return "ltop_mass";
        case LTOP_MT: return "ltop_mt";
        case HTOP_PT: return "htop_pt";
        case HTOP_ETA: return "htop_eta";
        case HTOP_MASS: return "htop_mass";
        case HTOP_MT: return "htop_mt";
        
        default: return "unknown";
    }
}

Template &Template::operator ++()
{
    if (HTOP_MT != type());
        _type = Type(static_cast<int>(type()) + 1);

    return *this;
}

Template &Template::operator --()
{
    if (MET != type());
        _type = Type(static_cast<int>(type()) - 1);

    return *this;
}

string Template::unit() const
{
    switch(type())
    {
        case MET: return "GeV/c^{2}";
        case HTLEP: return "GeV";
        case NPV: return "";
        case NJET: return "";
        case TTBAR_MASS: return "GeV/c^{2}";
        case TTBAR_PT: return "GeV/c";
        case WLEP_MT: return "GeV/c^{2}";
        case WLEP_MASS: return "GeV/c^{2}";
        case JET1_PT: return "GeV/c";
        case JET1_ETA: return "";
        case JET2_PT: return "GeV/c";
        case JET2_ETA: return "";
        case JET3_PT: return "GeV/c";
        case JET3_ETA: return "";
        case ELECTRON_PT: return "GeV/c";
        case ELECTRON_ETA: return "";
        case LTOP_PT: return "GeV/c";
        case LTOP_ETA: return "";
        case LTOP_MASS: return "GeV/c^{2}";
        case LTOP_MT: return "GeV/c^{2}";
        case HTOP_PT: return "GeV/c";
        case HTOP_ETA: return "";
        case HTOP_MASS: return "GeV/c^{2}";
        case HTOP_MT: return "GeV/c^{2}";

        default: return "";
    }
}



// Helpers
//
bool operator <(const Template &c1, const Template &c2)
{
    return c1.type() < c2.type();
}

bool operator >(const Template &c1, const Template &c2)
{
    return c1.type() > c2.type();
}

bool operator <=(const Template &c1, const Template &c2)
{
    return c1.type() <= c2.type();
}

bool operator >=(const Template &c1, const Template &c2)
{
    return c1.type() >= c2.type();
}

bool operator ==(const Template &c1, const Template &c2)
{
    return c1.type() == c2.type();
}

bool operator !=(const Template &c1, const Template &c2)
{
    return c1.type() != c2.type();
}

ostream &operator <<(ostream &out, const Template &c)
{
    return out << static_cast<string>(c);
}
