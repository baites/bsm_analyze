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
    // ID
    book1d("EIDLoosePt", 50, 0, 100);
    book1d("EIDLooseEta", 50, -2.5, 2.5);
    book1d("EIDLoosePhi", 50, 0, 3.15);    

    book1d("EIDMediumPt", 50, 0, 100);
    book1d("EIDMediumEta", 50, -2.5, 2.5);
    book1d("EIDMediumPhi", 50, 0, 3.15);

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

    // ID + CONV

    book1d("ECONVLoosePt", 50, 0, 100);
    book1d("ECONVLooseEta", 50, -2.5, 2.5);
    book1d("ECONVLoosePhi", 50, 0, 3.15);  

    book1d("ECONVMediumPt", 50, 0, 100);
    book1d("ECONVMediumEta", 50, -2.5, 2.5);
    book1d("ECONVMediumPhi", 50, 0, 3.15);

    book1d("ECONVTightPt", 50, 0, 100);
    book1d("ECONVTightEta", 50, -2.5, 2.5);
    book1d("ECONVTightPhi", 50, 0, 3.15);

    book1d("ECONVSuperTightPt", 50, 0, 100);
    book1d("ECONVSuperTightEta", 50, -2.5, 2.5);
    book1d("ECONVSuperTightPhi", 50, 0, 3.15);

    book1d("ECONVHyperTight1Pt", 50, 0, 100);
    book1d("ECONVHyperTight1Eta", 50, -2.5, 2.5);
    book1d("ECONVHyperTight1Phi", 50, 0, 3.15);

    book1d("ECONVHyperTight2Pt", 50, 0, 100);
    book1d("ECONVHyperTight2Eta", 50, -2.5, 2.5);
    book1d("ECONVHyperTight2Phi", 50, 0, 3.15);

    book1d("ECONVHyperTight3Pt", 50, 0, 100);
    book1d("ECONVHyperTight3Eta", 50, -2.5, 2.5);
    book1d("ECONVHyperTight3Phi", 50, 0, 3.15);

    book1d("ECONVHyperTight4Pt", 50, 0, 100);
    book1d("ECONVHyperTight4Eta", 50, -2.5, 2.5);
    book1d("ECONVHyperTight4Phi", 50, 0, 3.15);
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
                if (electronid.conversion_rejection())
                {
                    get1d("ECONVLoosePt")->fill(pt(electron.physics_object().p4()));
                    get1d("ECONVLooseEta")->fill(eta(electron.physics_object().p4()));
                    get1d("ECONVLoosePhi")->fill(phi(electron.physics_object().p4()));
                }
            }
            if (electronid.name() == bsm::Electron::Medium && electronid.identification())
            {
                get1d("EIDMediumPt")->fill(pt(electron.physics_object().p4()));
                get1d("EIDMediumEta")->fill(eta(electron.physics_object().p4()));
                get1d("EIDMediumPhi")->fill(phi(electron.physics_object().p4()));
                if (electronid.conversion_rejection())
                {
                    get1d("ECONVMediumPt")->fill(pt(electron.physics_object().p4()));
                    get1d("ECONVMediumEta")->fill(eta(electron.physics_object().p4()));
                    get1d("ECONVMediumPhi")->fill(phi(electron.physics_object().p4()));
                }
            } 
            if (electronid.name() == bsm::Electron::Tight && electronid.identification())
            {
                get1d("EIDTightPt")->fill(pt(electron.physics_object().p4()));
                get1d("EIDTightEta")->fill(eta(electron.physics_object().p4()));
                get1d("EIDTightPhi")->fill(phi(electron.physics_object().p4()));
                if (electronid.conversion_rejection())
                {
                    get1d("ECONVTightPt")->fill(pt(electron.physics_object().p4()));
                    get1d("ECONVTightEta")->fill(eta(electron.physics_object().p4()));
                    get1d("ECONVTightPhi")->fill(phi(electron.physics_object().p4()));
                }
            }
            if (electronid.name() == bsm::Electron::SuperTight && electronid.identification())
            {
                get1d("EIDSuperTightPt")->fill(pt(electron.physics_object().p4()));
                get1d("EIDSuperTightEta")->fill(eta(electron.physics_object().p4()));
                get1d("EIDSuperTightPhi")->fill(phi(electron.physics_object().p4()));
                if (electronid.conversion_rejection())
                {
                    get1d("ECONVSuperTightPt")->fill(pt(electron.physics_object().p4()));
                    get1d("ECONVSuperTightEta")->fill(eta(electron.physics_object().p4()));
                    get1d("ECONVSuperTightPhi")->fill(phi(electron.physics_object().p4()));
                }
            }
            if (electronid.name() == bsm::Electron::HyperTight1 && electronid.identification())
            {
                get1d("EIDHyperTight1Pt")->fill(pt(electron.physics_object().p4()));
                get1d("EIDHyperTight1Eta")->fill(eta(electron.physics_object().p4()));
                get1d("EIDHyperTight1Phi")->fill(phi(electron.physics_object().p4()));
                if (electronid.conversion_rejection())
                {
                    get1d("ECONVHyperTight1Pt")->fill(pt(electron.physics_object().p4()));
                    get1d("ECONVHyperTight1Eta")->fill(eta(electron.physics_object().p4()));
                    get1d("ECONVHyperTight1Phi")->fill(phi(electron.physics_object().p4()));
                }
            }
            if (electronid.name() == bsm::Electron::HyperTight2 && electronid.identification())
            {
                get1d("EIDHyperTight2Pt")->fill(pt(electron.physics_object().p4()));
                get1d("EIDHyperTight2Eta")->fill(eta(electron.physics_object().p4()));
                get1d("EIDHyperTight2Phi")->fill(phi(electron.physics_object().p4()));
                if (electronid.conversion_rejection())
                {
                    get1d("ECONVHyperTight2Pt")->fill(pt(electron.physics_object().p4()));
                    get1d("ECONVHyperTight2Eta")->fill(eta(electron.physics_object().p4()));
                    get1d("ECONVHyperTight2Phi")->fill(phi(electron.physics_object().p4()));
                }
            }
            if (electronid.name() == bsm::Electron::HyperTight3 && electronid.identification())
            {
                get1d("EIDHyperTight3Pt")->fill(pt(electron.physics_object().p4()));
                get1d("EIDHyperTight3Eta")->fill(eta(electron.physics_object().p4()));
                get1d("EIDHyperTight3Phi")->fill(phi(electron.physics_object().p4()));
                if (electronid.conversion_rejection())
                {
                    get1d("ECONVHyperTight3Pt")->fill(pt(electron.physics_object().p4()));
                    get1d("ECONVHyperTight3Eta")->fill(eta(electron.physics_object().p4()));
                    get1d("ECONVHyperTight3Phi")->fill(phi(electron.physics_object().p4()));
                }
            }
            if (electronid.name() == bsm::Electron::HyperTight4 && electronid.identification())
            {
                get1d("EIDHyperTight4Pt")->fill(pt(electron.physics_object().p4()));
                get1d("EIDHyperTight4Eta")->fill(eta(electron.physics_object().p4()));
                get1d("EIDHyperTight4Phi")->fill(phi(electron.physics_object().p4()));
                if (electronid.conversion_rejection())
                {
                    get1d("ECONVHyperTight4Pt")->fill(pt(electron.physics_object().p4()));
                    get1d("ECONVHyperTight4Eta")->fill(eta(electron.physics_object().p4()));
                    get1d("ECONVHyperTight4Phi")->fill(phi(electron.physics_object().p4()));
                }
            }
        }
    }
}


}
