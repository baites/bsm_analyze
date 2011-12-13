// Template histogram
//
// Created by Samvel Khalatyan, Dec 2, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_TEMPLATE
#define BSM_TEMPLATE

#include <string>
#include <iosfwd>

class Template
{
    public:
        enum Type
        {
            MET = 0,
            MET_QCD,
            MET_QCD_NOWEIGHT,
            HTLEP,
            NPV,
            NPV_NO_PU,
            NJET,
            TTBAR_MASS,
            TTBAR_PT,
            WLEP_MT,
            WLEP_MASS,
            JET1_PT,
            JET1_ETA,
            JET2_PT,
            JET2_ETA,
            JET3_PT,
            JET3_ETA,
            ELECTRON_PT,
            ELECTRON_ETA,
            ELECTRON_PT_BEFORE_TRICUT,
            ELECTRON_ETA_BEFORE_TRICUT,
            LTOP_PT,
            LTOP_ETA,
            LTOP_MASS,
            LTOP_MT,
            HTOP_PT,
            HTOP_ETA,
            HTOP_MASS,
            HTOP_MT,
            DPHI_ELECTRON_VS_MET,
            DPHI_JET_VS_MET,
            DPHI_ELECTRON_VS_MET_BEFORE_TRICUT,
            DPHI_JET_VS_MET_BEFORE_TRICUT
        };

        Template(const Type &type): _type(type)
        {
        }

        inline Type type() const
        {
            return _type;
        }

        operator std::string() const;

        std::string repr() const;

        Template &operator ++();
        Template &operator --();

        std::string unit() const;

    private:
        Type _type;
};

bool operator <(const Template &, const Template &);
bool operator >(const Template &, const Template &);

bool operator <=(const Template &, const Template &);
bool operator >=(const Template &, const Template &);

bool operator ==(const Template &, const Template &);
bool operator !=(const Template &, const Template &);

std::ostream &operator <<(std::ostream &out, const Template &c);

#endif
