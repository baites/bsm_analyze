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


ElectronIDAnalyzer::ElectronIDAnalyzer() 
{
    // Initializing selector

    _synch_selector.reset(new SynchSelector());
    monitor(_synch_selector);        

    // Histogram booking 

    _bookkeeper.reset(new HistogramBookkeeper());

    _bookkeeper->book1d("EIDLoosePt", 50, 0, 100);
    _bookkeeper->book1d("EIDLooseEta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EIDLoosePhi", 50, 0, 3.15);    

    _bookkeeper->book1d("EIDMediumPt", 50, 0, 100);
    _bookkeeper->book1d("EIDMediumEta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EIDMediumPhi", 50, 0, 3.15);

    _bookkeeper->book1d("EIDTightPt", 50, 0, 100);
    _bookkeeper->book1d("EIDTightEta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EIDTightPhi", 50, 0, 3.15);

    _bookkeeper->book1d("EIDSuperTightPt", 50, 0, 100);
    _bookkeeper->book1d("EIDSuperTightEta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EIDSuperTightPhi", 50, 0, 3.15);

    _bookkeeper->book1d("EIDHyperTight1Pt", 50, 0, 100);
    _bookkeeper->book1d("EIDHyperTight1Eta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EIDHyperTight1Phi", 50, 0, 3.15);

    _bookkeeper->book1d("EIDHyperTight2Pt", 50, 0, 100);
    _bookkeeper->book1d("EIDHyperTight2Eta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EIDHyperTight2Phi", 50, 0, 3.15);

    _bookkeeper->book1d("EIDHyperTight3Pt", 50, 0, 100);
    _bookkeeper->book1d("EIDHyperTight3Eta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EIDHyperTight3Phi", 50, 0, 3.15);

    _bookkeeper->book1d("EIDHyperTight4Pt", 50, 0, 100);
    _bookkeeper->book1d("EIDHyperTight4Eta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EIDHyperTight4Phi", 50, 0, 3.15);

    // ID + CONV

    _bookkeeper->book1d("ECONVLoosePt", 50, 0, 100);
    _bookkeeper->book1d("ECONVLooseEta", 50, -2.5, 2.5);
    _bookkeeper->book1d("ECONVLoosePhi", 50, 0, 3.15);  

    _bookkeeper->book1d("ECONVMediumPt", 50, 0, 100);
    _bookkeeper->book1d("ECONVMediumEta", 50, -2.5, 2.5);
    _bookkeeper->book1d("ECONVMediumPhi", 50, 0, 3.15);

    _bookkeeper->book1d("ECONVTightPt", 50, 0, 100);
    _bookkeeper->book1d("ECONVTightEta", 50, -2.5, 2.5);
    _bookkeeper->book1d("ECONVTightPhi", 50, 0, 3.15);

    _bookkeeper->book1d("ECONVSuperTightPt", 50, 0, 100);
    _bookkeeper->book1d("ECONVSuperTightEta", 50, -2.5, 2.5);
    _bookkeeper->book1d("ECONVSuperTightPhi", 50, 0, 3.15);

    _bookkeeper->book1d("ECONVHyperTight1Pt", 50, 0, 100);
    _bookkeeper->book1d("ECONVHyperTight1Eta", 50, -2.5, 2.5);
    _bookkeeper->book1d("ECONVHyperTight1Phi", 50, 0, 3.15);

    _bookkeeper->book1d("ECONVHyperTight2Pt", 50, 0, 100);
    _bookkeeper->book1d("ECONVHyperTight2Eta", 50, -2.5, 2.5);
    _bookkeeper->book1d("ECONVHyperTight2Phi", 50, 0, 3.15);

    _bookkeeper->book1d("ECONVHyperTight3Pt", 50, 0, 100);
    _bookkeeper->book1d("ECONVHyperTight3Eta", 50, -2.5, 2.5);
    _bookkeeper->book1d("ECONVHyperTight3Phi", 50, 0, 3.15);

    _bookkeeper->book1d("ECONVHyperTight4Pt", 50, 0, 100);
    _bookkeeper->book1d("ECONVHyperTight4Eta", 50, -2.5, 2.5);
    _bookkeeper->book1d("ECONVHyperTight4Phi", 50, 0, 3.15);

    monitor(_bookkeeper);
}


ElectronIDAnalyzer::ElectronIDAnalyzer(const ElectronIDAnalyzer & object)
{
    _synch_selector.reset(new SynchSelector(*object._synch_selector));   
    monitor(_synch_selector);
    _bookkeeper.reset(new HistogramBookkeeper(*object._bookkeeper));
    monitor(_bookkeeper);
}


void ElectronIDAnalyzer::process(const Event *event)
{
    if (!_synch_selector->apply(event)) return;  

    for (int i = 0; i < event->pf_electrons_size(); ++i)
    {
        const bsm::Electron & electron = event->pf_electrons(i);
        for (int j = 0; j < electron.electronid_size(); ++j)
        {
            const bsm::Electron::ElectronID & electronid = electron.electronid(j);
            if (electronid.name() == bsm::Electron::Loose && electronid.identification())
            {
                _bookkeeper->get1d("EIDLoosePt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDLooseEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDLoosePhi")->fill(phi(electron.physics_object().p4()));
                if (electronid.conversion_rejection())
                {
                    _bookkeeper->get1d("ECONVLoosePt")->fill(pt(electron.physics_object().p4()));
                    _bookkeeper->get1d("ECONVLooseEta")->fill(eta(electron.physics_object().p4()));
                    _bookkeeper->get1d("ECONVLoosePhi")->fill(phi(electron.physics_object().p4()));
                }
            }
            if (electronid.name() == bsm::Electron::Medium && electronid.identification())
            {
                _bookkeeper->get1d("EIDMediumPt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDMediumEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDMediumPhi")->fill(phi(electron.physics_object().p4()));
                if (electronid.conversion_rejection())
                {
                    _bookkeeper->get1d("ECONVMediumPt")->fill(pt(electron.physics_object().p4()));
                    _bookkeeper->get1d("ECONVMediumEta")->fill(eta(electron.physics_object().p4()));
                    _bookkeeper->get1d("ECONVMediumPhi")->fill(phi(electron.physics_object().p4()));
                }
            } 
            if (electronid.name() == bsm::Electron::Tight && electronid.identification())
            {
                _bookkeeper->get1d("EIDTightPt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDTightEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDTightPhi")->fill(phi(electron.physics_object().p4()));
                if (electronid.conversion_rejection())
                {
                    _bookkeeper->get1d("ECONVTightPt")->fill(pt(electron.physics_object().p4()));
                    _bookkeeper->get1d("ECONVTightEta")->fill(eta(electron.physics_object().p4()));
                    _bookkeeper->get1d("ECONVTightPhi")->fill(phi(electron.physics_object().p4()));
                }
            }
            if (electronid.name() == bsm::Electron::SuperTight && electronid.identification())
            {
                _bookkeeper->get1d("EIDSuperTightPt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDSuperTightEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDSuperTightPhi")->fill(phi(electron.physics_object().p4()));
                if (electronid.conversion_rejection())
                {
                    _bookkeeper->get1d("ECONVSuperTightPt")->fill(pt(electron.physics_object().p4()));
                    _bookkeeper->get1d("ECONVSuperTightEta")->fill(eta(electron.physics_object().p4()));
                    _bookkeeper->get1d("ECONVSuperTightPhi")->fill(phi(electron.physics_object().p4()));
                }
            }
            if (electronid.name() == bsm::Electron::HyperTight1 && electronid.identification())
            {
                _bookkeeper->get1d("EIDHyperTight1Pt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDHyperTight1Eta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDHyperTight1Phi")->fill(phi(electron.physics_object().p4()));
                if (electronid.conversion_rejection())
                {
                    _bookkeeper->get1d("ECONVHyperTight1Pt")->fill(pt(electron.physics_object().p4()));
                    _bookkeeper->get1d("ECONVHyperTight1Eta")->fill(eta(electron.physics_object().p4()));
                    _bookkeeper->get1d("ECONVHyperTight1Phi")->fill(phi(electron.physics_object().p4()));
                }
            }
            if (electronid.name() == bsm::Electron::HyperTight2 && electronid.identification())
            {
                _bookkeeper->get1d("EIDHyperTight2Pt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDHyperTight2Eta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDHyperTight2Phi")->fill(phi(electron.physics_object().p4()));
                if (electronid.conversion_rejection())
                {
                    _bookkeeper->get1d("ECONVHyperTight2Pt")->fill(pt(electron.physics_object().p4()));
                    _bookkeeper->get1d("ECONVHyperTight2Eta")->fill(eta(electron.physics_object().p4()));
                    _bookkeeper->get1d("ECONVHyperTight2Phi")->fill(phi(electron.physics_object().p4()));
                }
            }
            if (electronid.name() == bsm::Electron::HyperTight3 && electronid.identification())
            {
                _bookkeeper->get1d("EIDHyperTight3Pt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDHyperTight3Eta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDHyperTight3Phi")->fill(phi(electron.physics_object().p4()));
                if (electronid.conversion_rejection())
                {
                    _bookkeeper->get1d("ECONVHyperTight3Pt")->fill(pt(electron.physics_object().p4()));
                    _bookkeeper->get1d("ECONVHyperTight3Eta")->fill(eta(electron.physics_object().p4()));
                    _bookkeeper->get1d("ECONVHyperTight3Phi")->fill(phi(electron.physics_object().p4()));
                }
            }
            if (electronid.name() == bsm::Electron::HyperTight4 && electronid.identification())
            {
                _bookkeeper->get1d("EIDHyperTight4Pt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDHyperTight4Eta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDHyperTight4Phi")->fill(phi(electron.physics_object().p4()));
                if (electronid.conversion_rejection())
                {
                    _bookkeeper->get1d("ECONVHyperTight4Pt")->fill(pt(electron.physics_object().p4()));
                    _bookkeeper->get1d("ECONVHyperTight4Eta")->fill(eta(electron.physics_object().p4()));
                    _bookkeeper->get1d("ECONVHyperTight4Phi")->fill(phi(electron.physics_object().p4()));
                }
            }
        }
    }
}


}
