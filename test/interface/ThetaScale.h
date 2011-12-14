// Scales for each sample from Theta
//
// Created by Samvel Khalatyan, Dec 14, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_THETA_SCALE
#define BSM_THETA_SCALE

#include <string>

struct ThetaScale
{
    ThetaScale():
        wjets(0),
        zjets(0),
        stop(0),
        ttjets(0),
        qcd(0)
    {
    }

    void load(const std::string &filename);

    float wjets;
    float zjets;
    float stop;
    float ttjets;
    float qcd;
};

std::ostream &operator <<(std::ostream &, const ThetaScale &);

#endif
