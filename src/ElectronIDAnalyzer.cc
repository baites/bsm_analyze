//
//
// Generate decay table of processed events
//
// Created by Samvel Khalatyan, Jun 29, 2011
// Copyright 2011, All rights reserved

#include <cmath>

#include "interface/ElectronIDAnalyzer.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Event.pb.h"

namespace bsm
{


ElectronIDAnalyzer::ElectronIDAnalyzer(): HistogramProducer()
{
    book1d("EIDLoosePt", 50, 0, 100);
    book1d("EIDTightPt", 50, 0, 100);
}


void ElectronIDAnalyzer::process(const Event *event)
{
    for (int i = 0; i < event->pf_electrons_size(); ++i)
    {
        const bsm::Electron & electron = event->pf_electrons(i);
        for (int j = 0; j < electron.electronid_size(); ++j)
        {
            const bsm::Electron::ElectronID & electronid = electron.electronid(j);
            if (electronid.name() == bsm::Electron::Loose && electronid.identification())
                fill1d("EIDLoosePt", pt(electron.physics_object().p4()));
            if (electronid.name() == bsm::Electron::Tight && electronid.identification())
                fill1d("EIDTightPt", pt(electron.physics_object().p4()));
        }
    }
}


}
