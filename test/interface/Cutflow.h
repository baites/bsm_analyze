// Scale cutflow values per sample
//
// Created by Samvel Khalatyan, Dec 14, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_CUTFLOW
#define BSM_CUTFLOW

#include <string>

#include "interface/ThetaScale.h"

class Cutflow
{
    public:
        Cutflow()
        {
        }

        void loadScales(const std::string &filename);

        void load(const std::string &filename);

    private:
        ThetaScale _scales;
};

#endif
