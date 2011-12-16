// Scale cutflow values per sample
//
// Created by Samvel Khalatyan, Dec 14, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_CUTFLOW
#define BSM_CUTFLOW

#include <string>

class Cutflow
{
    public:
        Cutflow()
        {
        }

        void load(const std::string &filename, const float &qcd_scale);
};

#endif
