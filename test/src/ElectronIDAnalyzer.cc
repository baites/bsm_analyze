//
//
// Generate decay table of processed events
//
// Created by Samvel Khalatyan, Jun 29, 2011
// Copyright 2011, All rights reserved

#include <cmath>

#include "interface/Cut.h"
#include "interface/ElectronIDAnalyzer.h"
#include "interface/Cut.h"
#include "bsm_input/interface/Algebra.h"

namespace bsm
{


ElectronIDAnalyzer::ElectronIDAnalyzer()
{
    // Initializing selector

    _synch_selector.reset(new SynchSelector());
    _synch_selector->htlep()->disable();
    monitor(_synch_selector);

    // Histogram booking

    _bookkeeper.reset(new HistogramBookkeeper());

    // All good electrons

    _bookkeeper->book1d("EIDAllPt", 50, 0, 100);
    _bookkeeper->book1d("EIDAllEta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EIDAllPhi", 50, 0, 3.15);

    // ID

    _bookkeeper->book1d("EIDVeryLoosePt", 50, 0, 100);
    _bookkeeper->book1d("EIDVeryLooseEta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EIDVeryLoosePhi", 50, 0, 3.15);

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

    _bookkeeper->book1d("ECONVVeryLoosePt", 50, 0, 100);
    _bookkeeper->book1d("ECONVVeryLooseEta", 50, -2.5, 2.5);
    _bookkeeper->book1d("ECONVVeryLoosePhi", 50, 0, 3.15);

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

    // Full set of bits

    _bookkeeper->book1d("EFULLVeryLoosePt", 50, 0, 100);
    _bookkeeper->book1d("EFULLVeryLooseEta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EFULLVeryLoosePhi", 50, 0, 3.15);

    _bookkeeper->book1d("EFULLLoosePt", 50, 0, 100);
    _bookkeeper->book1d("EFULLLooseEta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EFULLLoosePhi", 50, 0, 3.15);

    _bookkeeper->book1d("EFULLMediumPt", 50, 0, 100);
    _bookkeeper->book1d("EFULLMediumEta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EFULLMediumPhi", 50, 0, 3.15);

    _bookkeeper->book1d("EFULLTightPt", 50, 0, 100);
    _bookkeeper->book1d("EFULLTightEta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EFULLTightPhi", 50, 0, 3.15);

    _bookkeeper->book1d("EFULLSuperTightPt", 50, 0, 100);
    _bookkeeper->book1d("EFULLSuperTightEta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EFULLSuperTightPhi", 50, 0, 3.15);

    _bookkeeper->book1d("EFULLHyperTight1Pt", 50, 0, 100);
    _bookkeeper->book1d("EFULLHyperTight1Eta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EFULLHyperTight1Phi", 50, 0, 3.15);

    _bookkeeper->book1d("EFULLHyperTight2Pt", 50, 0, 100);
    _bookkeeper->book1d("EFULLHyperTight2Eta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EFULLHyperTight2Phi", 50, 0, 3.15);

    _bookkeeper->book1d("EFULLHyperTight3Pt", 50, 0, 100);
    _bookkeeper->book1d("EFULLHyperTight3Eta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EFULLHyperTight3Phi", 50, 0, 3.15);

    _bookkeeper->book1d("EFULLHyperTight4Pt", 50, 0, 100);
    _bookkeeper->book1d("EFULLHyperTight4Eta", 50, -2.5, 2.5);
    _bookkeeper->book1d("EFULLHyperTight4Phi", 50, 0, 3.15);

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

    SynchSelector::GoodElectrons const & electrons = _synch_selector->goodElectrons();

    for (std::size_t i = 0; i < electrons.size(); ++i)
    {
        bsm::Electron const & electron = *electrons[i];

        _bookkeeper->get1d("EIDAllPt")->fill(pt(electron.physics_object().p4()));
        _bookkeeper->get1d("EIDAllEta")->fill(eta(electron.physics_object().p4()));
        _bookkeeper->get1d("EIDAllPhi")->fill(phi(electron.physics_object().p4()));

        for (int j = 0; j < electron.id_size(); ++j)
        {
            const bsm::Electron::ElectronID & electronid = electron.id(j);

            if (electronid.name() == bsm::Electron::VeryLoose && electronid.identification())
            {
                _bookkeeper->get1d("EIDVeryLoosePt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDVeryLooseEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDVeryLoosePhi")->fill(phi(electron.physics_object().p4()));
            }
            if (electronid.name() == bsm::Electron::Loose && electronid.identification())
            {
                _bookkeeper->get1d("EIDLoosePt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDLooseEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDLoosePhi")->fill(phi(electron.physics_object().p4()));
            }
            if (electronid.name() == bsm::Electron::Medium && electronid.identification())
            {
                _bookkeeper->get1d("EIDMediumPt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDMediumEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDMediumPhi")->fill(phi(electron.physics_object().p4()));
            }
            if (electronid.name() == bsm::Electron::Tight && electronid.identification())
            {
                _bookkeeper->get1d("EIDTightPt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDTightEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDTightPhi")->fill(phi(electron.physics_object().p4()));
            }
            if (electronid.name() == bsm::Electron::SuperTight && electronid.identification())
            {
                _bookkeeper->get1d("EIDSuperTightPt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDSuperTightEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDSuperTightPhi")->fill(phi(electron.physics_object().p4()));
            }
            if (electronid.name() == bsm::Electron::HyperTight1 && electronid.identification())
            {
                _bookkeeper->get1d("EIDHyperTight1Pt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDHyperTight1Eta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDHyperTight1Phi")->fill(phi(electron.physics_object().p4()));
            }
            if (electronid.name() == bsm::Electron::HyperTight2 && electronid.identification())
            {
                _bookkeeper->get1d("EIDHyperTight2Pt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDHyperTight2Eta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDHyperTight2Phi")->fill(phi(electron.physics_object().p4()));
            }
            if (electronid.name() == bsm::Electron::HyperTight3 && electronid.identification())
            {
                _bookkeeper->get1d("EIDHyperTight3Pt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDHyperTight3Eta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDHyperTight3Phi")->fill(phi(electron.physics_object().p4()));
            }
            if (electronid.name() == bsm::Electron::HyperTight4 && electronid.identification())
            {
                _bookkeeper->get1d("EIDHyperTight4Pt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDHyperTight4Eta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDHyperTight4Phi")->fill(phi(electron.physics_object().p4()));
            }

            // EID + CONV
            if (
                electronid.name() == bsm::Electron::VeryLoose &&
                electronid.identification() &&
                electronid.conversion_rejection()
            )
            {
                _bookkeeper->get1d("ECONVVeryLoosePt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("ECONVVeryLooseEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("ECONVVeryLoosePhi")->fill(phi(electron.physics_object().p4()));
            }
            if (
                electronid.name() == bsm::Electron::Loose &&
                electronid.identification() &&
                electronid.conversion_rejection()
            )
            {
                _bookkeeper->get1d("ECONVLoosePt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("ECONVLooseEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("ECONVLoosePhi")->fill(phi(electron.physics_object().p4()));
            }
            if (
                electronid.name() == bsm::Electron::Medium &&
                electronid.identification() &&
                electronid.conversion_rejection()
            )
            {
                _bookkeeper->get1d("ECONVMediumPt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("ECONVMediumEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("ECONVMediumPhi")->fill(phi(electron.physics_object().p4()));
            }
            if (
                electronid.name() == bsm::Electron::Tight &&
                electronid.identification() &&
                electronid.conversion_rejection()
            )
            {
                _bookkeeper->get1d("ECONVTightPt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("ECONVTightEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("ECONVTightPhi")->fill(phi(electron.physics_object().p4()));
            }
            if (
                electronid.name() == bsm::Electron::SuperTight &&
                electronid.identification() &&
                electronid.conversion_rejection()
            )
            {
                _bookkeeper->get1d("ECONVSuperTightPt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("ECONVSuperTightEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("ECONVSuperTightPhi")->fill(phi(electron.physics_object().p4()));
            }
            if (
                electronid.name() == bsm::Electron::HyperTight1 &&
                electronid.identification() &&
                electronid.conversion_rejection()
            )
            {
                _bookkeeper->get1d("ECONVHyperTight1Pt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("ECONVHyperTight1Eta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("ECONVHyperTight1Phi")->fill(phi(electron.physics_object().p4()));
            }
            if (
                electronid.name() == bsm::Electron::HyperTight2 &&
                electronid.identification() &&
                electronid.conversion_rejection()
            )
            {
                _bookkeeper->get1d("ECONVHyperTight2Pt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("ECONVHyperTight2Eta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("ECONVHyperTight2Phi")->fill(phi(electron.physics_object().p4()));
            }
            if (
                electronid.name() == bsm::Electron::HyperTight3 &&
                electronid.identification() &&
                electronid.conversion_rejection()
            )
            {
                _bookkeeper->get1d("ECONVHyperTight3Pt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("ECONVHyperTight3Eta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("ECONVHyperTight3Phi")->fill(phi(electron.physics_object().p4()));
            }
            if (
                electronid.name() == bsm::Electron::HyperTight4 &&
                electronid.identification() &&
                electronid.conversion_rejection()
            )
            {
                _bookkeeper->get1d("ECONVHyperTight4Pt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("ECONVHyperTight4Eta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("ECONVHyperTight4Phi")->fill(phi(electron.physics_object().p4()));
            }

            // FULL

            if (
                electronid.name() == bsm::Electron::VeryLoose &&
                electronid.identification() &&
                electronid.isolation() &&
                electronid.conversion_rejection() &&
                electronid.impact_parameter()

            )
            {
                _bookkeeper->get1d("EFULLVeryLoosePt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EFULLVeryLooseEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EFULLVeryLoosePhi")->fill(phi(electron.physics_object().p4()));
            }
            if (
                electronid.name() == bsm::Electron::Loose &&
                electronid.identification() &&
                electronid.isolation() &&
                electronid.conversion_rejection() &&
                electronid.impact_parameter()

            )
            {
                _bookkeeper->get1d("EFULLLoosePt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EFULLLooseEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EFULLLoosePhi")->fill(phi(electron.physics_object().p4()));
            }
            if (
                electronid.name() == bsm::Electron::Medium &&
                electronid.identification() &&
                electronid.isolation() &&
                electronid.conversion_rejection() &&
                electronid.impact_parameter()
            )
            {
                _bookkeeper->get1d("EFULLMediumPt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EFULLMediumEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EFULLMediumPhi")->fill(phi(electron.physics_object().p4()));
            }
            if (
                electronid.name() == bsm::Electron::Tight &&
                electronid.identification() &&
                electronid.isolation() &&
                electronid.conversion_rejection() &&
                electronid.impact_parameter()
            )
            {
                _bookkeeper->get1d("EFULLTightPt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EFULLTightEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EFULLTightPhi")->fill(phi(electron.physics_object().p4()));
            }
            if (
                electronid.name() == bsm::Electron::SuperTight &&
                electronid.identification() &&
                electronid.isolation() &&
                electronid.conversion_rejection() &&
                electronid.impact_parameter()
            )
            {
                _bookkeeper->get1d("EFULLSuperTightPt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EFULLSuperTightEta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EFULLSuperTightPhi")->fill(phi(electron.physics_object().p4()));
            }
            if (
                electronid.name() == bsm::Electron::HyperTight1 &&
                electronid.identification() &&
                electronid.isolation() &&
                electronid.conversion_rejection() &&
                electronid.impact_parameter()
            )
            {
                _bookkeeper->get1d("EFULLHyperTight1Pt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EFULLHyperTight1Eta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EFULLHyperTight1Phi")->fill(phi(electron.physics_object().p4()));
            }
            if (
                electronid.name() == bsm::Electron::HyperTight2 &&
                electronid.identification() &&
                electronid.isolation() &&
                electronid.conversion_rejection() &&
                electronid.impact_parameter()
            )
            {
                _bookkeeper->get1d("EFULLHyperTight2Pt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EFULLHyperTight2Eta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EFULLHyperTight2Phi")->fill(phi(electron.physics_object().p4()));
            }
            if (
                electronid.name() == bsm::Electron::HyperTight3 &&
                electronid.identification() &&
                electronid.isolation() &&
                electronid.conversion_rejection() &&
                electronid.impact_parameter()
            )
            {
                _bookkeeper->get1d("EFULLHyperTight3Pt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EFULLHyperTight3Eta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EFULLHyperTight3Phi")->fill(phi(electron.physics_object().p4()));
            }
            if (
                electronid.name() == bsm::Electron::HyperTight4 &&
                electronid.identification() &&
                electronid.isolation() &&
                electronid.conversion_rejection() &&
                electronid.impact_parameter()
            )
            {
                _bookkeeper->get1d("EFULLHyperTight4Pt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EFULLHyperTight4Eta")->fill(eta(electron.physics_object().p4()));
                _bookkeeper->get1d("EFULLHyperTight4Phi")->fill(phi(electron.physics_object().p4()));
            }
        }
    }
}


}
