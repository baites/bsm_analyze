// Input type 
//
// Created by Samvel Khalatyan, Dec 2, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_ROOT_INPUT
#define BSM_ROOT_INPUT

#include <iosfwd>
#include <string>

class Input
{
    public:
        enum Type
        {
            QCD_BC_PT20_30 = 0,
            QCD_BC_PT30_80,
            QCD_BC_PT80_170,
            QCD_EM_PT20_30,
            QCD_EM_PT30_80,
            QCD_EM_PT80_170,
            TTJETS,
            TTJETS_SCALE_UP,
            TTJETS_SCALE_DOWN,
            TTJETS_MATCHING_UP,
            TTJETS_MATCHING_DOWN,
            ZJETS,
            WJETS,
            WJETS_SCALE_UP,
            WJETS_SCALE_DOWN,
            WJETS_MATCHING_UP,
            WJETS_MATCHING_DOWN,
            STOP_S,
            STOP_T,
            STOP_TW,
            SATOP_S,
            SATOP_T,
            SATOP_TW,
            RERECO_2011A_MAY10,
            RERECO_2011A_AUG05,
            PROMPT_2011A_V4,
            PROMPT_2011A_V6,
            PROMPT_2011B_V1,
            ZPRIME1000,
            ZPRIME1500,
            ZPRIME2000,
            ZPRIME3000,
            ZPRIME4000,
            UNKNOWN
        };

        Input(const Type &type): _type(type)
        {
        }

        inline Type type() const
        {
            return _type;
        }

        operator std::string() const;

        std::string repr() const;

        Input &operator ++();
        Input &operator --();

    private:
        Type _type;
};

bool operator <(const Input &l, const Input &r);
bool operator >(const Input &l, const Input &r);

bool operator <=(const Input &l, const Input &r);
bool operator >=(const Input &l, const Input &r);

bool operator ==(const Input &l, const Input &r);
bool operator !=(const Input &l, const Input &r);

std::ostream &operator <<(std::ostream &out, const Input &input);

#endif
