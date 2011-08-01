// Jet Energy Corrections Package
//
// Wrapper around CMSSW Jet Energy Corrections with options
//
// Created by Samvel Khalatyan, Jul 31, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <ostream>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/pointer_cast.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Electron.pb.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Jet.pb.h"
#include "bsm_input/interface/Muon.pb.h"
#include "bsm_input/interface/Physics.pb.h"
#include "JetMETObjects/interface/FactorizedJetCorrector.h"
#include "interface/JetEnergyCorrections.h"

using namespace std;

namespace fs = boost::filesystem;
using boost::dynamic_pointer_cast;

using bsm::JetEnergyCorrectionDelegate;
using bsm::JetEnergyCorrectionOptions;
using bsm::JetEnergyCorrections;

JetEnergyCorrectionOptions::JetEnergyCorrectionOptions()
{
    _delegate = 0;

    _options.reset(new po::options_description("Jet Energy Correction Options"));
    _options->add_options()
        ("l1",
         po::value<string>()->notifier(
             boost::bind(&JetEnergyCorrectionOptions::setCorrections, this, L1, _1)),
         "Level 1 corrections")

        ("l2",
         po::value<string>()->notifier(
             boost::bind(&JetEnergyCorrectionOptions::setCorrections, this, L2, _1)),
         "Level 2 corrections")

        ("l3",
         po::value<string>()->notifier(
             boost::bind(&JetEnergyCorrectionOptions::setCorrections, this, L3, _1)),
         "Level 3 corrections")
    ;
}

JetEnergyCorrectionOptions::~JetEnergyCorrectionOptions()
{
}

JetEnergyCorrectionOptions::OptionsPtr JetEnergyCorrectionOptions::options() const
{
    return _options;
}

void JetEnergyCorrectionOptions::setCorrections(const JetEnergyCorrectionLevel &jec_level,
        const std::string &file_name)
{
    // Check validity of flag
    //
    switch(jec_level)
    {
        case L1: // Fall through
        case L2: // Fall through
        case L3: 
            {
                if (!fs::exists(file_name))
                    cerr << "Jet Energy Corrections " << jec_level
                        << " file does not exist: " << file_name << endl;
                else
                {
                    if (delegate())
                        delegate()->setCorrection(jec_level, file_name);
                }
            }
        default: cerr << "unsupported Jet Energy Corrections level" << endl;
    }
}

void JetEnergyCorrectionOptions::setDelegate(JetEnergyCorrectionDelegate *delegate)
{
    if (_delegate != delegate)
        _delegate = delegate;
}

JetEnergyCorrectionDelegate *JetEnergyCorrectionOptions::delegate() const
{
    return _delegate;
}



// Jet Energy Corrections
//
JetEnergyCorrections::JetEnergyCorrections()
{
}

JetEnergyCorrections::JetEnergyCorrections(const JetEnergyCorrections &object):
    _corrections(object._corrections)
{
}

void JetEnergyCorrections::setCorrection(const JetEnergyCorrectionLevel &jec_level,
        const std::string &file_name)
{
    if (_corrections.end() != _corrections.find(jec_level))
        cerr << "jet energy correction " << jec_level << " is already loaded" << endl;
    else
        _corrections[jec_level] = JetCorrectorParameters(file_name);
}

bsm::LorentzVector JetEnergyCorrections::correctJet(const Jet *jet,
        const Event *event,
        const Electrons &electrons,
        const Muons &muons)
{
    // Check if jet uncorrected energy is available
    //
    if (!jet->has_uncorrected_p4())
        return jet->physics_object().p4();

    // Test if corrections are loaded otherwier return uncorrected energy
    //
    CorrectorPtr jec = corrector();
    if (!jec)
        return jet->uncorrected_p4();

    // Remove leptons
    //
    LorentzVector corrected_p4 = jet->uncorrected_p4();

    typedef ::google::protobuf::RepeatedPtrField<Jet::Child> Children;
    for(Children::const_iterator child = jet->children().begin();
            jet->children().end() != child;
            ++child)
    {
        const LorentzVector &child_p4 = child->physics_object().p4();

        // Electrons
        //
        for(Electrons::const_iterator electron = electrons.begin();
                electrons.end() != electron;
                ++electron)
        {
            const LorentzVector &electron_p4 = (*electron)->physics_object().p4();
            if (electron_p4 == child_p4)
                corrected_p4 -= electron_p4;
        }

        // Muons
        //
        for(Muons::const_iterator muon = muons.begin();
                muons.end() != muon;
                ++muon)
        {
            const LorentzVector &muon_p4 = (*muon)->physics_object().p4();
            if (muon_p4 == child_p4)
                corrected_p4 -= muon_p4;
        }
    }

    jec->setJetEta(eta(corrected_p4));
    jec->setJetPt(pt(corrected_p4));
    jec->setJetE(corrected_p4.e());
    jec->setNPV(event->primary_vertices().size());
    jec->setJetA(jet->extra().area());
    jec->setRho(event->extra().rho());

    const float correction = jec->getCorrection();
    corrected_p4 *= correction;

    return corrected_p4;
}

uint32_t JetEnergyCorrections::id() const
{
    return core::ID<JetEnergyCorrections>::get();
}

JetEnergyCorrections::ObjectPtr JetEnergyCorrections::clone() const
{
    return ObjectPtr(new JetEnergyCorrections(*this));
}

void JetEnergyCorrections::print(std::ostream &out) const
{
}

// Privates
//
JetEnergyCorrections::CorrectorPtr JetEnergyCorrections::corrector()
{
    if (!_jec
            && !_corrections.empty())
    {
        vector<JetCorrectorParameters> corrections;
        for(Corrections::const_iterator correction = _corrections.begin();
                _corrections.end() != correction;
                ++correction)
        {
            corrections.push_back(correction->second);
        }

        _jec.reset(new FactorizedJetCorrector(corrections));
    }

    return _jec;
}



// Helpers
//
std::ostream &bsm::operator <<(std::ostream &out,
        const JetEnergyCorrectionLevel &jec_level)
{
    switch(jec_level)
    {
        case L1: out << "L1";
                 break;

        case L2: out << "L2";
                 break;

        case L3: out << "L3";
                 break;

        default: out << "unknown";
                 break;
    }

    return out;
}
