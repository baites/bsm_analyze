//
//
// Generate decay table of processed events
//
// Created by Samvel Khalatyan, Jun 29, 2011
// Copyright 2011, All rights reserved

#include <cmath>

#include "interface/ElectronIDAnalyzer.h"
#include "bsm_input/interface/Algebra.h"

namespace bsm
{


ElectronIDAnalyzer::ElectronIDAnalyzer() : HistogramProducer()
{
    book1d("EIDLoosePt", 50, 0, 100);
    book1d("EIDLooseEta", 50, -2.5, 2.5);
    book1d("EIDLoosePhi", 50, 0, 3.15);    

    book1d("EIDTightPt", 50, 0, 100);
    book1d("EIDTightEta", 50, -2.5, 2.5);
    book1d("EIDTightPhi", 50, 0, 3.15);

    book1d("EIDSuperTightPt", 50, 0, 100);
    book1d("EIDSuperTightEta", 50, -2.5, 2.5);
    book1d("EIDSuperTightPhi", 50, 0, 3.15);

    book1d("EIDHyperTight1Pt", 50, 0, 100);
    book1d("EIDHyperTight1Eta", 50, -2.5, 2.5);
    book1d("EIDHyperTight1Phi", 50, 0, 3.15);

    book1d("EIDHyperTight2Pt", 50, 0, 100);
    book1d("EIDHyperTight2Eta", 50, -2.5, 2.5);
    book1d("EIDHyperTight2Phi", 50, 0, 3.15);

    book1d("EIDHyperTight3Pt", 50, 0, 100);
    book1d("EIDHyperTight3Eta", 50, -2.5, 2.5);
    book1d("EIDHyperTight3Phi", 50, 0, 3.15);

    book1d("EIDHyperTight4Pt", 50, 0, 100);
    book1d("EIDHyperTight4Eta", 50, -2.5, 2.5);
    book1d("EIDHyperTight4Phi", 50, 0, 3.15);


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
            {
                get1d("EIDLoosePt")->fill(pt(electron.physics_object().p4()));
                get1d("EIDLooseEta")->fill(eta(electron.physics_object().p4()));
                get1d("EIDLoosePhi")->fill(phi(electron.physics_object().p4()));
            }
            if (electronid.name() == bsm::Electron::Tight && electronid.identification())
            {
                get1d("EIDTightPt")->fill(pt(electron.physics_object().p4()));
                get1d("EIDTightEta")->fill(eta(electron.physics_object().p4()));
                get1d("EIDTightPhi")->fill(phi(electron.physics_object().p4()));
            }
            if (electronid.name() == bsm::Electron::SuperTight && electronid.identification())
            {
                get1d("EIDSuperTightPt")->fill(pt(electron.physics_object().p4()));
                get1d("EIDSuperTightEta")->fill(eta(electron.physics_object().p4()));
                get1d("EIDSuperTightPhi")->fill(phi(electron.physics_object().p4()));
            }
            if (electronid.name() == bsm::Electron::HyperTight1 && electronid.identification())
            {
                get1d("EIDHyperTight1Pt")->fill(pt(electron.physics_object().p4()));
                get1d("EIDHyperTight1Eta")->fill(eta(electron.physics_object().p4()));
                get1d("EIDHyperTight1Phi")->fill(phi(electron.physics_object().p4()));
            }
            if (electronid.name() == bsm::Electron::HyperTight2 && electronid.identification())
            {
                get1d("EIDHyperTight2Pt")->fill(pt(electron.physics_object().p4()));
                get1d("EIDHyperTight2Eta")->fill(eta(electron.physics_object().p4()));
                get1d("EIDHyperTight2Phi")->fill(phi(electron.physics_object().p4()));
            }
            if (electronid.name() == bsm::Electron::HyperTight3 && electronid.identification())
            {
                get1d("EIDHyperTight3Pt")->fill(pt(electron.physics_object().p4()));
                get1d("EIDHyperTight3Eta")->fill(eta(electron.physics_object().p4()));
                get1d("EIDHyperTight3Phi")->fill(phi(electron.physics_object().p4()));
            }
            if (electronid.name() == bsm::Electron::HyperTight4 && electronid.identification())
            {
                get1d("EIDHyperTight4Pt")->fill(pt(electron.physics_object().p4()));
                get1d("EIDHyperTight4Eta")->fill(eta(electron.physics_object().p4()));
                get1d("EIDHyperTight4Phi")->fill(phi(electron.physics_object().p4()));
            }
        }
    }
}


}
