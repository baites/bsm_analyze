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
        wjets(1),
        zjets(1),
        stop(1),
        ttjets(1),
        qcd(1)
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
