// Jet Energy Corrections Package
//
// Wrapper around CMSSW Jet Energy Corrections with options
//
// Created by Samvel Khalatyan, Jul 31, 2011
// Copyright 2011, All rights reserved

#include <algorithm>
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

    _description.reset(new po::options_description("Jet Energy Correction Options"));
    _description->add_options()
        ("l1",
         po::value<string>()->notifier(
             boost::bind(&JetEnergyCorrectionOptions::setCorrection, this,
                 JetEnergyCorrectionDelegate::L1, _1)),
         "Level 1 corrections")

        ("l2",
         po::value<string>()->notifier(
             boost::bind(&JetEnergyCorrectionOptions::setCorrection, this,
                 JetEnergyCorrectionDelegate::L2, _1)),
         "Level 2 corrections")

        ("l3",
         po::value<string>()->notifier(
             boost::bind(&JetEnergyCorrectionOptions::setCorrection, this,
                 JetEnergyCorrectionDelegate::L3, _1)),
         "Level 3 corrections")

        ("dr-correction",
         po::value<bool>()->implicit_value(true)->notifier(
             boost::bind(&JetEnergyCorrectionOptions::setDeltaRCorrection,
                 this)),
         "Use DeltaR-based correction")
    ;
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

// Options interface
//
JetEnergyCorrectionOptions::DescriptionPtr
    JetEnergyCorrectionOptions::description() const
{
    return _description;
}

// Private
//
void JetEnergyCorrectionOptions::setCorrection(
    const JetEnergyCorrectionDelegate::Level &jec_level,
    const std::string &file_name)
{
    if (!delegate())
        return;

    // Check validity of flag
    //
    switch(jec_level)
    {
        case JetEnergyCorrectionDelegate::L1: // Fall through
        case JetEnergyCorrectionDelegate::L2: // Fall through
        case JetEnergyCorrectionDelegate::L3: 
            {
                if (!fs::exists(file_name))
                    cerr << jec_level
                        << " jet energy correction file does not exist: "
                        << file_name << endl;
                else
                    delegate()->setCorrection(jec_level, file_name);

                break;
            }

        default: cerr << "unsupported Jet Energy Correction level" << endl;
    }
}

void JetEnergyCorrectionOptions::setDeltaRCorrection()
{
    if (!delegate())
        return;

    delegate()->setDeltaRCorrection();
}



// Jet Energy Corrections
//
JetEnergyCorrections::JetEnergyCorrections():
    _use_dr_correction(false)
{
}

JetEnergyCorrections::JetEnergyCorrections(const JetEnergyCorrections &object):
    _use_dr_correction(object._use_dr_correction)
{
    for(CorrectionFiles::const_reverse_iterator correction =
            object._correction_files.rbegin();
            object._correction_files.rend() != correction;
            ++correction)
    {
        setCorrection(correction->first, correction->second);
    }
}

JetEnergyCorrections::CorrectedJet JetEnergyCorrections::correctJet(
        const Jet *jet,
        const Event *event,
        const Electrons &electrons,
        const Muons &muons)
{
    CorrectedJet corrected_jet;
    corrected_jet.jet = jet;

    // Test if corrections are loaded
    //
    CorrectorPtr jec = corrector();
    if (!jec)
        return corrected_jet;

    // Check if jet uncorrected energy and area are available
    //
    if (!jet->has_uncorrected_p4()
            || !jet->has_extra()
            || !jet->extra().has_area())
        return corrected_jet;

    // Check if event RHO information is available
    //
    if (!event->has_extra()
            || !event->extra().has_rho())
        return corrected_jet;

    corrected_jet.corrected_p4.reset(new LorentzVector());
    corrected_jet.corrected_p4->CopyFrom(jet->uncorrected_p4());

    // Remove leptons only if any were passed
    //
    if (!electrons.empty()
            || !muons.empty())
    {
        if (_use_dr_correction)
        {
            const LorentzVector &jet_p4 = jet->physics_object().p4();

            // Electrons
            //
            for(Electrons::const_iterator electron = electrons.begin();
                    electrons.end() != electron;
                    ++electron)
            {
                const LorentzVector &electron_p4 =
                    (*electron)->physics_object().p4();
                if (0.5 > dr(electron_p4, jet_p4))
                {
                    *corrected_jet.corrected_p4 -= electron_p4;
                    corrected_jet.subtracted_electrons.push_back(*electron);
                }
            }

            // Muons
            //
            for(Muons::const_iterator muon = muons.begin();
                    muons.end() != muon;
                    ++muon)
            {
                const LorentzVector &muon_p4 = (*muon)->physics_object().p4();
                if (0.5 > dr(muon_p4, jet_p4))
                {
                    *corrected_jet.corrected_p4 -= muon_p4;
                    corrected_jet.subtracted_muons.push_back(*muon);
                }
            }
        }
        else
        {
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
                    const LorentzVector &electron_p4 =
                        (*electron)->physics_object().p4();
                    if (electron_p4 == child_p4)
                    {
                        *corrected_jet.corrected_p4 -= electron_p4;
                        corrected_jet.subtracted_electrons.push_back(*electron);
                    }
                }

                // Muons
                //
                for(Muons::const_iterator muon = muons.begin();
                        muons.end() != muon;
                        ++muon)
                {
                    const LorentzVector &muon_p4 = (*muon)->physics_object().p4();
                    if (muon_p4 == child_p4)
                    {
                        *corrected_jet.corrected_p4 -= muon_p4;
                        corrected_jet.subtracted_muons.push_back(*muon);
                    }
                }
            }
        }
    }

    corrected_jet.subtracted_p4.reset(new LorentzVector());
    corrected_jet.subtracted_p4->CopyFrom(*corrected_jet.corrected_p4);

    correct(corrected_jet, event);

    return corrected_jet;
}

// Jet Energy Correction Delegate interface
//
void JetEnergyCorrections::setCorrection(const Level &jec_level,
        const std::string &file_name)
{
    if (_corrections.end() != _corrections.find(jec_level))
        cerr << jec_level << " jet energy correction is already loaded" << endl;
    else
    {
        _corrections[jec_level] = JetCorrectorParameters(file_name);
        _correction_files[jec_level] = file_name;

        clog << jec_level << " loaded " << file_name << endl;

        _jec.reset();

        corrector();
    }
}

void JetEnergyCorrections::setDeltaRCorrection()
{
    _use_dr_correction = true;
}

// Object interface
//
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

void JetEnergyCorrections::correct(CorrectedJet &jet,
        const Event *event)
{
    CorrectorPtr jec = corrector();

    // Correct jet Lorentz Vector
    //
    jec->setJetEta(eta(*jet.corrected_p4));
    jec->setJetPt(pt(*jet.corrected_p4));
    jec->setJetE(jet.corrected_p4->e());
    jec->setNPV(event->primary_vertices().size());
    jec->setJetA(jet.jet->extra().area());
    jec->setRho(event->extra().rho());

    jet.correction = jec->getCorrection();
    *jet.corrected_p4 *= jet.correction;
}



// Helpers
//
std::ostream &bsm::operator <<(std::ostream &out,
        const JetEnergyCorrectionDelegate::Level &jec_level)
{
    switch(jec_level)
    {
        case JetEnergyCorrectionDelegate::L1: out << "L1";
                                              break;

        case JetEnergyCorrectionDelegate::L2: out << "L2";
                                              break;

        case JetEnergyCorrectionDelegate::L3: out << "L3";
                                              break;

        default: out << "unknown";
                 break;
    }

    return out;
}
