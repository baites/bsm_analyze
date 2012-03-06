// Common Algorithm Classes/functions
//
// Useful tools that are heavily used in the analysis
//
// Created by Samvel Khalatyan, Apr 25, 2011
// Copyright 2011, All rights reserved

#include <cfloat>
#include <iostream>

#include <boost/pointer_cast.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Jet.pb.h"
#include "bsm_input/interface/Physics.pb.h"
#include "interface/Algorithm.h"
#include "interface/Utility.h"

using namespace std;
using namespace bsm;

// Neutrino Recontstruct: neglect products masses
//
NeutrinoReconstruct::NeutrinoReconstruct()
{
}

NeutrinoReconstruct::NeutrinoReconstruct(const NeutrinoReconstruct &)
{
}

NeutrinoReconstruct::Solutions
    NeutrinoReconstruct::operator()(const LorentzVector &lepton,
        const LorentzVector &neutrino)
{
    // The final equation is:
    //
    //  (-pTlep^2) * x^2 + 2 * (mu * pZlep) * x + (mu^2 - Elep^2 * pTnu^2) = 0
    //
    // where
    //      x is pz_nu
    //      mu = mW^2 / 2 + pTlep * pTnu * cos(phi)
    //      phi is angle between p_lepton and p_neutrino in transverse plane
    //
    Vector lepton_pT = toVector(lepton);
    lepton_pT.set_z(0);

    Vector neutrino_pT = toVector(neutrino);
    neutrino_pT.set_z(0);

    const float mass_w = 80.399;
    float mu = mass_w * mass_w / 2 + lepton_pT * neutrino_pT;

    float A = - (lepton_pT * lepton_pT);
    float B = mu * lepton.pz();
    float C = mu * mu - lepton.e() * lepton.e() * (neutrino_pT * neutrino_pT);

    float discriminant = B * B - A * C;

    Solutions solutions;

    if (0 >= discriminant)
    {
        // Take only real part of the solution
        //
        LorentzVectorPtr solution(new LorentzVector());
        *solution = neutrino;
        solution->set_pz(-B / A);
        solution->set_e(magnitude(toVector(*solution)));

        solutions.push_back(solution);

        _solutions = 0 > discriminant ? 0 : 1;
    }
    else
    {
        discriminant = sqrt(discriminant);

        LorentzVectorPtr solution(new LorentzVector());
        *solution = neutrino;
        solution->set_pz((-B - discriminant) / A);
        solution->set_e(magnitude(toVector(*solution)));

        solutions.push_back(solution);

        solution.reset(new LorentzVector());
        *solution = neutrino;
        solution->set_pz((-B + discriminant) / A);
        solution->set_e(magnitude(toVector(*solution)));

        solutions.push_back(solution);

        _solutions = 2;
    }

    return solutions;
}

int NeutrinoReconstruct::solutions() const
{
    return _solutions;
}

uint32_t NeutrinoReconstruct::id() const
{
    return core::ID<NeutrinoReconstruct>::get();
}

NeutrinoReconstruct::ObjectPtr NeutrinoReconstruct::clone() const
{
    return ObjectPtr(new NeutrinoReconstruct(*this));
}

void NeutrinoReconstruct::print(std::ostream &out) const
{
}



// TTbar DeltaR-based Reconstruction
//
TTbarDeltaRReconstruct::TTbarDeltaRReconstruct():
    _dr_min(FLT_MAX),
    _dr_max(0)
{
}

TTbarDeltaRReconstruct::TTbarDeltaRReconstruct(const TTbarDeltaRReconstruct &object):
    _dr_min(object._dr_min),
    _dr_max(object._dr_max)
{
    if (object._ttbar.top)
    {
        _ttbar.top.reset(new LorentzVector());
        *_ttbar.top = *object._ttbar.top;
    }

    if (object._ttbar.tbar)
    {
        _ttbar.tbar.reset(new LorentzVector());
        *_ttbar.tbar = *object._ttbar.tbar;
    }
}

TTbarDeltaRReconstruct::TTbar TTbarDeltaRReconstruct::operator()(const Jets &jets,
        const LorentzVector &lepton,
        const LorentzVector &neutrino)
{
    _dr_min = FLT_MAX;
    _dr_max = 0;

    _ttbar = TTbar();

    if (1 < jets.size())
        minimize(jets, lepton, neutrino, jets.end(),
                jets.end(), jets.end(), jets.end());

    return _ttbar;
}

uint32_t TTbarDeltaRReconstruct::id() const
{
    return core::ID<TTbarDeltaRReconstruct>::get();
}

TTbarDeltaRReconstruct::ObjectPtr TTbarDeltaRReconstruct::clone() const
{
    return ObjectPtr(new TTbarDeltaRReconstruct(*this));
}

void TTbarDeltaRReconstruct::print(std::ostream &out) const
{
}

// Privates
//
void TTbarDeltaRReconstruct::minimize(const Jets &jets,
        const LorentzVector &lepton,
        const LorentzVector &neutrino,
        const Jets::const_iterator &jet_leptonic,
        const Jets::const_iterator &jet_hadronic_1,
        const Jets::const_iterator &jet_hadronic_2,
        const Jets::const_iterator &jet_hadronic_3)
{
    if (jets.end() == jet_leptonic)
    {
        for(Jets::const_iterator jet = jets.begin(); jets.end() != jet; ++jet)
        {
            minimize(jets, lepton, neutrino, jet,
                    jets.end(), jets.end(), jets.end());
        }
    }
    else if (jets.end() == jet_hadronic_1)
    {
        for(Jets::const_iterator jet = jets.begin(); jets.end() != jet; ++jet)
        {
            if (jet_leptonic == jet)
                continue;

            minimize(jets, lepton, neutrino, jet_leptonic,
                    jet,  jets.end(), jets.end());
        }
    }
    else if (2 < jets.size()
            && jets.end() == jet_hadronic_2)
    {
        for(Jets::const_iterator jet = jets.begin();
                jets.end() != jet;
                ++jet)
        {
            if (jet_leptonic == jet
                    || jet_hadronic_1 == jet)
                continue;

            minimize(jets, lepton, neutrino, jet_leptonic,
                    jet_hadronic_1,  jet, jets.end());
        }
    }
    else if (3 < jets.size()
            && jets.end() == jet_hadronic_3)
    {
        for(Jets::const_iterator jet = jets.begin();
                jets.end() != jet;
                ++jet)
        {
            if (jet_leptonic == jet
                    || jet_hadronic_1 == jet
                    || jet_hadronic_2 == jet)
                continue;

            minimize(jets, lepton, neutrino, jet_leptonic,
                    jet_hadronic_1,  jet_hadronic_2, jet);
        }
    }
    else
    {
        LorentzVectorPtr top(new LorentzVector());
        *top = lepton + neutrino + *(*jet_leptonic);

        LorentzVectorPtr tbar(new LorentzVector());
        *tbar = *(*jet_hadronic_1);

        if (jets.end() != jet_hadronic_2)
            *tbar += *(*jet_hadronic_2);

        if (jets.end() != jet_hadronic_3)
            *tbar += *(*jet_hadronic_3);

        const float dr_min = dr(*top, lepton)
            + dr(*top, neutrino)
            + dr(*top, *(*jet_leptonic));

        const float dr_max = dr(*top, *tbar);

        if (dr_min < _dr_min
                && dr_max > _dr_max)
        {
            _dr_min = dr_min;
            _dr_max = dr_max;

            _ttbar.top = top;
            _ttbar.tbar = tbar;
        }
    }
}



// Jets Selector
//
JetsSelector::JetsSelector(const Jets &jets, const uint32_t &size):
    _jets(jets)
{

    for(Jets::const_iterator jet = jets.begin();
            jets.end() != jet
                && size > (jet - jets.begin());
            ++jet)
    {
        _selected_jets.push_back(jet);
    }
}

bool JetsSelector::next()
{
    /*
    for(SelectedJets::reverse_iterator iterator = _selected_jets.rbegin();
            _selected_jets.rend() != iterator;
            )
    {
        for(Jets::const_iterator jet = *iterator;
                _jets.end() != ++jet;
                )
        {
            if (isValid(jet))
            {
                *iterator = jet;

                jet_is_found
                break;
            }
        }
    }
    */

    return false;
}

// Private
//
bool JetsSelector::next(SelectedJets::reverse_iterator &iterator)
{
    bool jet_is_found = false;

    if (_selected_jets.rend() != iterator)
    {
        Jets::const_iterator &jet = ++(*iterator);

        while(!jet_is_found)
        {
            jet_is_found = next(jet);

            if (jet_is_found)
                break;

            SelectedJets::reverse_iterator next_item = iterator + 1;
            if (!next(next_item))
                break;

            jet = _jets.begin();
        }
    }

    return jet_is_found;
}

bool JetsSelector::next(Jets::const_iterator &jet)
{
    bool jet_is_found = false;
    while(_jets.end() != jet)
    {
        if (isValid(jet))
        {
            jet_is_found = true;

            break;
        }

        ++jet;
    }

    return jet_is_found;
}

bool JetsSelector::isValid(Jets::const_iterator &jet)
{
    return _selected_jets.end() == find(_selected_jets.begin(),
            _selected_jets.end(),
            jet);
}



// Jet Iterator
//
JetIterator::JetIterator(const Jets &jets, const bool &is_valid):
    _is_valid(is_valid),
    _jets(jets)
{
    _jet = isValid()
        ? _jets.begin()
        : _jets.end();
}

bool JetIterator::isValid() const
{
    return _is_valid;
}

const JetIterator::Jets::const_iterator JetIterator::iterator() const
{
    return _jet;
}

void JetIterator::operator++()
{
    if (isValid())
    {
        ++_jet;

        if (_jets.end() == _jet)
            _is_valid = false;
    }
}



// -- Resonance Reconstructor -------------------------------------------------
//
ResonanceReconstructor::Mttbar ResonanceReconstructor::run(
        const LorentzVector &lepton,
        const LorentzVector &met,
        const SynchSelector::GoodJets &jets) const
{
    Mttbar result;

    // Reconstruct the neutrino pZ and keep solutions in vector for later
    // use
    //
    NeutrinoReconstruct neutrinoReconstruct;
    NeutrinoReconstruct::Solutions neutrinos =
        neutrinoReconstruct(lepton, met);

    result.solutions = neutrinoReconstruct.solutions();

    for(NeutrinoReconstruct::Solutions::const_iterator neutrino =
                neutrinos.begin();
            neutrinos.end() != neutrino;
            ++neutrino)
    {
        result.neutrinos.push_back(**neutrino);
    }

    // Prepare generator and loop over all hypotheses of the decay
    // (different jets assignment to leptonic/hadronic legs)
    //
    Generator generator;
    generator.init(jets);

    // Best Solution should have minimun value of the DeltaRmin:
    //
    //  DeltaRmin = DeltaR(ltop, b) + DeltaR(ltop, l) + DeltaR(ltop, nu)
    //
    // and maximum value of the DeltaR between leptonic and hadronic
    // tops in case the same DeltaRmin is found:
    //
    //  DeltaRlh = DeltaR(ltop, htop)
    //
    struct Solution
    {
        Solution():
            htop_discriminator(0),
            ltop_discriminator(FLT_MAX),
            htop_njets(0),
            valid(false)
        {
        }

        LorentzVector ltop; // Reconstructed leptonic leg
        LorentzVector htop; // Reconstructed hadronic leg
        LorentzVector missing_energy;

        CorrectedJets htop_jets;
        CorrectedJets ltop_jets;

        LorentzVector ltop_jet; // Used jet in the ltop reconstruction

        float htop_discriminator;
        float ltop_discriminator;
        int htop_njets;

        bool valid;
    } best_solution;

    // Loop over all possible hypotheses and pick the best one
    // Note: take into account all reconstructed neutrino solutions
    //
    do
    {
        Generator::Hypothesis hypothesis = generator.hypothesis();

        if (!isValidHadronicSide(lepton, hypothesis.hadronic)
                || !isValidLeptonicSide(lepton, hypothesis.leptonic)
                || !isValidNeutralSide(lepton, hypothesis.neutral))

                continue;

        // Leptonic Top p4 = leptonP4 + nuP4 + bP4
        // where bP4 is:
        //  - b-tagged jet
        //  - otherwise, the hardest jet (highest pT)
        //
        LorentzVector ltop = lepton;
        LorentzVector ltop_jet = getLeptonicJet(hypothesis.leptonic);
        ltop += ltop_jet;

        // the neutrino will be taken into account later
        //

        // htop is a sum of all jet p4s assigned to the hadronic leg
        //
        LorentzVector htop;
        for(Generator::Iterators::const_iterator jet =
                    hypothesis.hadronic.begin();
                hypothesis.hadronic.end() != jet;
                ++jet)
        {
            htop += *(*jet)->corrected_p4;
        }

        // Take into account all neutrino solutions. Solutions are kept in
        // a vector of pointer
        //
        for(NeutrinoReconstruct::Solutions::const_iterator neutrino =
                    neutrinos.begin();
                neutrinos.end() != neutrino;
                ++neutrino)
        {
            const LorentzVector &neutrino_p4 = *(*neutrino);

            LorentzVector ltop_tmp = ltop;
            ltop_tmp += neutrino_p4;

            const float ltop_discriminator =
                getLeptonicDiscriminator(ltop_tmp,
                                         lepton,
                                         neutrino_p4,
                                         ltop_jet);

            const float htop_discriminator =
                getHadronicDiscriminator(ltop_tmp, htop, hypothesis.hadronic);

            if (ltop_discriminator < best_solution.ltop_discriminator
                    || (ltop_discriminator == best_solution.ltop_discriminator
                        && htop_discriminator > best_solution.htop_discriminator))
            {
                best_solution.htop_discriminator = htop_discriminator;
                best_solution.ltop_discriminator = ltop_discriminator;
                best_solution.ltop = ltop_tmp;
                best_solution.ltop_jet = ltop_jet;
                best_solution.htop = htop;
                best_solution.missing_energy = neutrino_p4;
                best_solution.htop_njets = hypothesis.hadronic.size();

                best_solution.htop_jets.clear();
                for(Generator::Iterators::const_iterator jet =
                            hypothesis.hadronic.begin();
                        hypothesis.hadronic.end() != jet;
                        ++jet)
                {
                    best_solution.htop_jets.push_back(*(*jet));
                }

                best_solution.ltop_jets.clear();
                for(Generator::Iterators::const_iterator jet =
                            hypothesis.leptonic.begin();
                        hypothesis.leptonic.end() != jet;
                        ++jet)
                {
                    best_solution.ltop_jets.push_back(*(*jet));
                }

                best_solution.valid = true;
            }
        }
    }
    while(generator.next());

    // Best Solution is found
    //
    if (best_solution.valid)
    {
        result.mttbar = best_solution.ltop + best_solution.htop;
        result.wlep = best_solution.missing_energy + lepton;
        result.neutrino = best_solution.missing_energy;
        result.ltop = best_solution.ltop;
        result.ltop_jet = best_solution.ltop_jet;
        result.htop = best_solution.htop;
        result.htop_njets = best_solution.htop_njets;

        result.htop_jets.clear();
        for(CorrectedJets::const_iterator jet = best_solution.htop_jets.begin();
                best_solution.htop_jets.end() != jet;
                ++jet)
        {
            result.htop_jets.push_back(*jet);
        }

        result.ltop_jets.clear();
        for(CorrectedJets::const_iterator jet = best_solution.ltop_jets.begin();
                best_solution.ltop_jets.end() != jet;
                ++jet)
        {
            result.ltop_jets.push_back(*jet);
        }

        sort(result.htop_jets.begin(), result.htop_jets.end(), CorrectedPtGreater()); 
        sort(result.ltop_jets.begin(), result.ltop_jets.end(), CorrectedPtGreater()); 

        result.valid = true;
    }

    return result;
}

void ResonanceReconstructor::print(std::ostream &out) const
{
    out << "ResonanceReconstructor" << endl;
}



// -- Simple Resonance Reconstructor ------------------------------------------ 
//
uint32_t SimpleResonanceReconstructor::id() const
{
    return core::ID<SimpleResonanceReconstructor>::get();
}

SimpleResonanceReconstructor::ObjectPtr SimpleResonanceReconstructor::clone() const
{
    return ObjectPtr(new SimpleResonanceReconstructor(*this));
}

void SimpleResonanceReconstructor::print(std::ostream &out) const
{
    out << "SimpleResonanceReconstructor" << endl;
}

// Private
//
bool SimpleResonanceReconstructor::isValidHadronicSide(const LorentzVector &,
        const Iterators &jets) const
{
    return !jets.empty();
}

bool SimpleResonanceReconstructor::isValidLeptonicSide(const LorentzVector &,
        const Iterators &jets) const
{
    return !jets.empty();
}

bool SimpleResonanceReconstructor::isValidNeutralSide(const LorentzVector &,
        const Iterators &jets) const
{
    return true;
}

bsm::LorentzVector SimpleResonanceReconstructor::getLeptonicJet(
        const Iterators &jets) const
{
    const CorrectedJet *hardest_jet = 0;
    float highest_pt = 0;

    // Select the hardest jet (highest pT)
    // Note: hypothesis keeps vector of iterators to Correcte Jets.
    //       Corrected jet has a pointer to the original jet and
    //       corrected P4
    //
    for(Iterators::const_iterator jet = jets.begin(); jets.end() != jet; ++jet)
    {
        const float jet_pt = pt(*(*jet)->corrected_p4);
        if (jet_pt > highest_pt)
            hardest_jet = &*(*jet);
    }

    return *hardest_jet->corrected_p4;
}

float SimpleResonanceReconstructor::getLeptonicDiscriminator(
        const LorentzVector &ltop,
        const LorentzVector &lepton,
        const LorentzVector &neutrino,
        const LorentzVector &jet) const
{
    return dr(ltop, lepton) + dr(ltop, neutrino) + dr(ltop, jet);
}

float SimpleResonanceReconstructor::getHadronicDiscriminator(
        const LorentzVector &ltop, 
        const LorentzVector &htop,
        const Iterators &htop_jets) const
{
    return dr(ltop, htop);
}



// -- Btag Resonance Reconstructor -------------------------------------------- 
//
uint32_t BtagResonanceReconstructor::id() const
{
    return core::ID<BtagResonanceReconstructor>::get();
}

BtagResonanceReconstructor::ObjectPtr BtagResonanceReconstructor::clone() const
{
    return ObjectPtr(new BtagResonanceReconstructor(*this));
}

void BtagResonanceReconstructor::print(std::ostream &out) const
{
    out << "BtagResonanceReconstructor" << endl;
}

// Private
//
bool BtagResonanceReconstructor::isValidHadronicSide(const LorentzVector &lepton,
        const Iterators &jets) const
{
    return SimpleResonanceReconstructor::isValidHadronicSide(lepton, jets)
        && 1 >= countBtags(jets);
}

bool BtagResonanceReconstructor::isValidLeptonicSide(const LorentzVector &lepton,
        const Iterators &jets) const
{
    return SimpleResonanceReconstructor::isValidLeptonicSide(lepton, jets)
        && 1 >= countBtags(jets);
}

bool BtagResonanceReconstructor::isValidNeutralSide(const LorentzVector &,
        const Iterators &jets) const
{
    return 1 > countBtags(jets);
}

bsm::LorentzVector BtagResonanceReconstructor::getLeptonicJet(
        const Iterators &jets) const
{
    const CorrectedJet *hardest_jet = 0;
    float highest_pt = 0;

    // Select the hardest jet (highest pT)
    // Note: hypothesis keeps vector of iterators to Correcte Jets.
    //       Corrected jet has a pointer to the original jet and
    //       corrected P4
    //
    for(Iterators::const_iterator jet = jets.begin(); jets.end() != jet; ++jet)
    {
        if (isBtagJet((*jet)->jet))
        {
            hardest_jet = &*(*jet);
            break;
        }

        const float jet_pt = pt(*(*jet)->corrected_p4);
        if (jet_pt > highest_pt)
            hardest_jet = &*(*jet);
    }

    return *hardest_jet->corrected_p4;
}

uint32_t BtagResonanceReconstructor::countBtags(const Iterators &jets) const
{
    uint32_t btagged_jets = 0;
    for(Iterators::const_iterator jet = jets.begin(); jets.end() != jet; ++jet)
    {
        if (isBtagJet((*jet)->jet))
            ++btagged_jets;
    }

    return btagged_jets;
}

bool BtagResonanceReconstructor::isBtagJet(const Jet *jet) const
{
    typedef ::google::protobuf::RepeatedPtrField<Jet::BTag> BTags;

    for(BTags::const_iterator btag = jet->btag().begin();
            jet->btag().end() != btag;
            ++btag)
    {
        if (Jet::BTag::SSVHE == btag->type())
        {
            return 1.74 < btag->discriminator();
        }
    }

    return false;
}



// -- DeltaR Resonance Reconstructor ------------------------------------------
//
uint32_t SimpleDrResonanceReconstructor::id() const
{
    return core::ID<SimpleDrResonanceReconstructor>::get();
}

SimpleDrResonanceReconstructor::ObjectPtr SimpleDrResonanceReconstructor::clone() const
{
    return ObjectPtr(new SimpleDrResonanceReconstructor(*this));
}

void SimpleDrResonanceReconstructor::print(std::ostream &out) const
{
    out << "SimpleDrResonanceReconstructor" << endl;
}

// Private
//
bool SimpleDrResonanceReconstructor::isValidHadronicSide(const LorentzVector &lepton,
        const Iterators &jets) const
{
    bool result = SimpleResonanceReconstructor::isValidHadronicSide(lepton, jets);
    for(Iterators::const_iterator jet = jets.begin();
            result && jets.end() != jet;
            ++jet)
    {
        if (_hadronic_dr > dr(lepton, *(*jet)->corrected_p4))
            result = false;
    }

    return result;
}

bool SimpleDrResonanceReconstructor::isValidLeptonicSide(const LorentzVector &lepton,
        const Iterators &jets) const
{
    bool result = SimpleResonanceReconstructor::isValidLeptonicSide(lepton, jets);
    for(Iterators::const_iterator jet = jets.begin();
            result && jets.end() != jet;
            ++jet)
    {
        if (_leptonic_dr < dr(lepton, *(*jet)->corrected_p4))
            result = false;
    }

    return result;
}

bool SimpleDrResonanceReconstructor::isValidNeutralSide(const LorentzVector &lepton,
        const Iterators &jets) const
{
    bool result = SimpleResonanceReconstructor::isValidNeutralSide(lepton, jets);
    for(Iterators::const_iterator jet = jets.begin();
            result && jets.end() != jet;
            ++jet)
    {
        if (_leptonic_dr > dr(lepton, *(*jet)->corrected_p4)
                || _hadronic_dr < dr(lepton, *(*jet)->corrected_p4))
            result = false;
    }

    return result;
}



// -- Hemisphere Resonance Reconstructor ------------------------------------------
//
uint32_t HemisphereResonanceReconstructor::id() const
{
    return core::ID<HemisphereResonanceReconstructor>::get();
}

HemisphereResonanceReconstructor::ObjectPtr HemisphereResonanceReconstructor::clone() const
{
    return ObjectPtr(new HemisphereResonanceReconstructor(*this));
}

void HemisphereResonanceReconstructor::print(std::ostream &out) const
{
    out << "HemisphereResonanceReconstructor" << endl;
}

// Private
//
bool HemisphereResonanceReconstructor::isValidHadronicSide(const LorentzVector &lepton,
        const Iterators &jets) const
{
    bool result = SimpleResonanceReconstructor::isValidHadronicSide(lepton, jets);
    for(Iterators::const_iterator jet = jets.begin();
            result && jets.end() != jet;
            ++jet)
    {
        if (_half_pi > angle(lepton, *(*jet)->corrected_p4))
            result = false;
    }

    return result;
}

bool HemisphereResonanceReconstructor::isValidLeptonicSide(const LorentzVector &lepton,
        const Iterators &jets) const
{
    bool result = SimpleResonanceReconstructor::isValidLeptonicSide(lepton, jets);
    for(Iterators::const_iterator jet = jets.begin();
            result && jets.end() != jet;
            ++jet)
    {
        if (_half_pi < angle(lepton, *(*jet)->corrected_p4))
            result = false;
    }

    return result;
}

bool HemisphereResonanceReconstructor::isValidNeutralSide(const LorentzVector &lepton,
        const Iterators &jets) const
{
    return jets.empty();
}



// --  Resonance Reconstructor with Htop Mass ----------------------------------
//
uint32_t ResonanceReconstructorWithMass::id() const
{
    return core::ID<ResonanceReconstructorWithMass>::get();
}

ResonanceReconstructorWithMass::ObjectPtr
    ResonanceReconstructorWithMass::clone() const
{
    return ObjectPtr(new ResonanceReconstructorWithMass(*this));
}

void ResonanceReconstructorWithMass::print(std::ostream &out) const
{
    out << "ResonanceReconstructorWithMass" << endl;
}

// Private
//
float ResonanceReconstructorWithMass::getHadronicDiscriminator(
        const LorentzVector &ltop,
        const LorentzVector &htop,
        const Iterators &htop_jets) const
{
    return pow(2.0 / (173 - mass(htop)), 2);
}



// --  Resonance Reconstructor with Delta Phi (htop, ltop) --------------------
//
uint32_t ResonanceReconstructorWithPhi::id() const
{
    return core::ID<ResonanceReconstructorWithPhi>::get();
}

ResonanceReconstructorWithPhi::ObjectPtr
    ResonanceReconstructorWithPhi::clone() const
{
    return ObjectPtr(new ResonanceReconstructorWithPhi(*this));
}

void ResonanceReconstructorWithPhi::print(std::ostream &out) const
{
    out << "ResonanceReconstructorWithPhi" << endl;
}

// Private
//
float ResonanceReconstructorWithPhi::getHadronicDiscriminator(
        const LorentzVector &ltop,
        const LorentzVector &htop,
        const Iterators &htop_jets) const
{
    return pow(.22 / (fabs(dphi(htop, ltop)) - 3.14159265), 2);
}



// --  Resonance Reconstructor with Mass and Delta Phi (htop, ltop) ------------
//
uint32_t ResonanceReconstructorWithMassAndPhi::id() const
{
    return core::ID<ResonanceReconstructorWithMassAndPhi>::get();
}

ResonanceReconstructorWithMassAndPhi::ObjectPtr
    ResonanceReconstructorWithMassAndPhi::clone() const
{
    return ObjectPtr(new ResonanceReconstructorWithMassAndPhi(*this));
}

void ResonanceReconstructorWithMassAndPhi::print(std::ostream &out) const
{
    out << "ResonanceReconstructorWithMassAndPhi" << endl;
}

// Private
//
float ResonanceReconstructorWithMassAndPhi::getHadronicDiscriminator(
        const LorentzVector &ltop,
        const LorentzVector &htop,
        const Iterators &htop_jets) const
{
    return ResonanceReconstructorWithMass::getHadronicDiscriminator(ltop, htop, htop_jets)
        * ResonanceReconstructorWithPhi::getHadronicDiscriminator(ltop, htop, htop_jets);
}



// -- Simple Resonance Reconstructor with Mass and Delta Phi (htop, ltop) -----
//
uint32_t SimpleResonanceReconstructorWithMassAndPhi::id() const
{
    return core::ID<SimpleResonanceReconstructorWithMassAndPhi>::get();
}

SimpleResonanceReconstructorWithMassAndPhi::ObjectPtr
    SimpleResonanceReconstructorWithMassAndPhi::clone() const
{
    return ObjectPtr(new SimpleResonanceReconstructorWithMassAndPhi(*this));
}

void SimpleResonanceReconstructorWithMassAndPhi::print(std::ostream &out) const
{
    out << "SimpleResonanceReconstructorWithMassAndPhi" << endl;
}

// Private
//
float SimpleResonanceReconstructorWithMassAndPhi::getHadronicDiscriminator(
        const LorentzVector &ltop,
        const LorentzVector &htop,
        const Iterators &htop_jets) const
{
    return SimpleResonanceReconstructor::getHadronicDiscriminator(ltop, htop, htop_jets)
        * ResonanceReconstructorWithMassAndPhi::getHadronicDiscriminator(ltop, htop, htop_jets);
}



// -- Simple Resonance Reconstructor with Mass ---------------------------------
//
uint32_t SimpleResonanceReconstructorWithMass::id() const
{
    return core::ID<SimpleResonanceReconstructorWithMass>::get();
}

SimpleResonanceReconstructorWithMass::ObjectPtr
    SimpleResonanceReconstructorWithMass::clone() const
{
    return ObjectPtr(new SimpleResonanceReconstructorWithMass(*this));
}

void SimpleResonanceReconstructorWithMass::print(std::ostream &out) const
{
    out << "SimpleResonanceReconstructorWithMass" << endl;
}

// Private
//
float SimpleResonanceReconstructorWithMass::getHadronicDiscriminator(
        const LorentzVector &ltop,
        const LorentzVector &htop,
        const Iterators &htop_jets) const
{
    return SimpleResonanceReconstructor::getHadronicDiscriminator(ltop, htop, htop_jets)
        * ResonanceReconstructorWithMass::getHadronicDiscriminator(ltop, htop, htop_jets);
}



// -- Collimated Simple Resonance Reconstructor with Mass ----------------------
//
uint32_t CollimatedSimpleResonanceReconstructorWithMass::id() const
{
    return core::ID<CollimatedSimpleResonanceReconstructorWithMass>::get();
}

CollimatedSimpleResonanceReconstructorWithMass::ObjectPtr
    CollimatedSimpleResonanceReconstructorWithMass::clone() const
{
    return ObjectPtr(new CollimatedSimpleResonanceReconstructorWithMass(*this));
}

void CollimatedSimpleResonanceReconstructorWithMass::print(std::ostream &out) const
{
    out << "CollimatedSimpleResonanceReconstructorWithMass" << endl;
}

// Private
//
float CollimatedSimpleResonanceReconstructorWithMass::getHadronicDiscriminator(
        const LorentzVector &ltop,
        const LorentzVector &htop,
        const Iterators &htop_jets) const
{
    float discriminator =
        SimpleResonanceReconstructorWithMass::getHadronicDiscriminator(
                ltop,
                htop,
                htop_jets);

    if (1 < htop_jets.size())
    {
        float hadronic_dr = 0;
        for(Generator::Iterators::const_iterator jet = htop_jets.begin();
                htop_jets.end() != jet;
                ++jet)
        {
            hadronic_dr += dr(htop, *(*jet)->corrected_p4);
        }

        discriminator *= 1. / hadronic_dr;
    }

    return discriminator;
}



// -- Collimated Simple Resonance Reconstructor with Top Mass ----------------------
//
uint32_t CollimatedSimpleResonanceReconstructorWithTopMass::id() const
{
    return core::ID<CollimatedSimpleResonanceReconstructorWithTopMass>::get();
}

CollimatedSimpleResonanceReconstructorWithTopMass::ObjectPtr
    CollimatedSimpleResonanceReconstructorWithTopMass::clone() const
{
    return ObjectPtr(new CollimatedSimpleResonanceReconstructorWithTopMass(*this));
}

void CollimatedSimpleResonanceReconstructorWithTopMass::print(std::ostream &out) const
{
    out << "CollimatedSimpleResonanceReconstructorWithTopMass" << endl;
}

// Private
//
bool CollimatedSimpleResonanceReconstructorWithTopMass::isValidLeptonicSide(
        const LorentzVector &,
        const Iterators &jets)
{
    return 1 == jets.size();
}

float CollimatedSimpleResonanceReconstructorWithTopMass::getLeptonicDiscriminator(
        const LorentzVector &ltop,
        const LorentzVector &lepton,
        const LorentzVector &neutrino,
        const LorentzVector &jet) const
{
    return pow((173.0 - mass(ltop)) / 2.0, 2);
}



// -- Collimated Tops with Mass Constrain --------------------------------------
//
uint32_t ResonanceReconstructorWithCollimatedTops::id() const
{
    return core::ID<ResonanceReconstructorWithCollimatedTops>::get();
}

ResonanceReconstructorWithCollimatedTops::ObjectPtr
    ResonanceReconstructorWithCollimatedTops::clone() const
{
    return ObjectPtr(new ResonanceReconstructorWithCollimatedTops(*this));
}

void ResonanceReconstructorWithCollimatedTops::print(std::ostream &out) const
{
    out << "ResonanceReconstructorWithCollimatedTops" << endl;
}

// Private
//
float ResonanceReconstructorWithCollimatedTops::getLeptonicDiscriminator(
        const LorentzVector &ltop,
        const LorentzVector &lepton,
        const LorentzVector &neutrino,
        const LorentzVector &jet) const
{
    return SimpleResonanceReconstructor::getLeptonicDiscriminator(ltop, lepton, neutrino, jet) *
        CollimatedSimpleResonanceReconstructorWithTopMass::getLeptonicDiscriminator(
            ltop, lepton, neutrino, jet);
}




// -- Reconstruction with ltop/htop chi2 ---------------------------------------
//
uint32_t Chi2ResonanceReconstructor::id() const
{
    return core::ID<Chi2ResonanceReconstructor>::get();
}

Chi2ResonanceReconstructor::ObjectPtr
    Chi2ResonanceReconstructor::clone() const
{
    return ObjectPtr(new Chi2ResonanceReconstructor(*this));
}

void Chi2ResonanceReconstructor::print(std::ostream &out) const
{
    out << "Chi2ResonanceReconstructor" << endl;
}

Chi2ResonanceReconstructor::Mttbar Chi2ResonanceReconstructor::run(
        const LorentzVector &lepton,
        const LorentzVector &met,
        const SynchSelector::GoodJets &jets) const
{
    Mttbar result;

    // Reconstruct the neutrino pZ and keep solutions in vector for later
    // use
    //
    NeutrinoReconstruct neutrinoReconstruct;
    NeutrinoReconstruct::Solutions neutrinos =
        neutrinoReconstruct(lepton, met);

    result.solutions = neutrinoReconstruct.solutions();

    for(NeutrinoReconstruct::Solutions::const_iterator neutrino =
                neutrinos.begin();
            neutrinos.end() != neutrino;
            ++neutrino)
    {
        result.neutrinos.push_back(**neutrino);
    }

    // Prepare generator and loop over all hypotheses of the decay
    // (different jets assignment to leptonic/hadronic legs)
    //
    Generator generator;
    generator.init(jets);

    // Best Solution should have minimun value of the DeltaRmin:
    //
    //  DeltaRmin = DeltaR(ltop, b) + DeltaR(ltop, l) + DeltaR(ltop, nu)
    //
    // and maximum value of the DeltaR between leptonic and hadronic
    // tops in case the same DeltaRmin is found:
    //
    //  DeltaRlh = DeltaR(ltop, htop)
    //
    struct Solution
    {
        Solution():
            htop_discriminator(FLT_MAX),
            ltop_discriminator(FLT_MAX),
            htop_njets(0),
            valid(false)
        {
        }

        LorentzVector ltop; // Reconstructed leptonic leg
        LorentzVector htop; // Reconstructed hadronic leg
        LorentzVector missing_energy;

        CorrectedJets htop_jets;
        CorrectedJets ltop_jets;

        LorentzVector ltop_jet; // Used jet in the ltop reconstruction

        float htop_discriminator;
        float ltop_discriminator;
        int htop_njets;

        bool valid;
    } best_solution;

    // Loop over all possible hypotheses and pick the best one
    // Note: take into account all reconstructed neutrino solutions
    //
    do
    {
        Generator::Hypothesis hypothesis = generator.hypothesis();

        if (!isValidHadronicSide(lepton, hypothesis.hadronic)
                || !isValidLeptonicSide(lepton, hypothesis.leptonic)
                || !isValidNeutralSide(lepton, hypothesis.neutral))

                continue;

        // Leptonic Top p4 = leptonP4 + nuP4 + bP4
        // where bP4 is:
        //  - b-tagged jet
        //  - otherwise, the hardest jet (highest pT)
        //
        LorentzVector ltop = lepton;
        LorentzVector ltop_jet = getLeptonicJet(hypothesis.leptonic);
        ltop += ltop_jet;

        // the neutrino will be taken into account later
        //

        // htop is a sum of all jet p4s assigned to the hadronic leg
        //
        LorentzVector htop;
        for(Generator::Iterators::const_iterator jet =
                    hypothesis.hadronic.begin();
                hypothesis.hadronic.end() != jet;
                ++jet)
        {
            htop += *(*jet)->corrected_p4;
        }

        // Take into account all neutrino solutions. Solutions are kept in
        // a vector of pointer
        //
        for(NeutrinoReconstruct::Solutions::const_iterator neutrino =
                    neutrinos.begin();
                neutrinos.end() != neutrino;
                ++neutrino)
        {
            const LorentzVector &neutrino_p4 = *(*neutrino);

            LorentzVector ltop_tmp = ltop;
            ltop_tmp += neutrino_p4;

            const float ltop_discriminator =
                getLeptonicDiscriminator(ltop_tmp,
                                         lepton,
                                         neutrino_p4,
                                         ltop_jet);

            const float htop_discriminator =
                getHadronicDiscriminator(ltop_tmp, htop, hypothesis.hadronic);

            if (ltop_discriminator < best_solution.ltop_discriminator
                    || (ltop_discriminator == best_solution.ltop_discriminator
                        && htop_discriminator < best_solution.htop_discriminator))
            {
                best_solution.htop_discriminator = htop_discriminator;
                best_solution.ltop_discriminator = ltop_discriminator;
                best_solution.ltop = ltop_tmp;
                best_solution.ltop_jet = ltop_jet;
                best_solution.htop = htop;
                best_solution.missing_energy = neutrino_p4;
                best_solution.htop_njets = hypothesis.hadronic.size();

                best_solution.htop_jets.clear();
                for(Generator::Iterators::const_iterator jet =
                            hypothesis.hadronic.begin();
                        hypothesis.hadronic.end() != jet;
                        ++jet)
                {
                    best_solution.htop_jets.push_back(*(*jet));
                }

                best_solution.ltop_jets.clear();
                for(Generator::Iterators::const_iterator jet =
                            hypothesis.leptonic.begin();
                        hypothesis.leptonic.end() != jet;
                        ++jet)
                {
                    best_solution.ltop_jets.push_back(*(*jet));
                }

                best_solution.valid = true;
            }
        }
    }
    while(generator.next());

    // Best Solution is found
    //
    if (best_solution.valid)
    {
        result.mttbar = best_solution.ltop + best_solution.htop;
        result.wlep = best_solution.missing_energy + lepton;
        result.neutrino = best_solution.missing_energy;
        result.ltop = best_solution.ltop;
        result.ltop_jet = best_solution.ltop_jet;
        result.htop = best_solution.htop;
        result.htop_njets = best_solution.htop_njets;

        result.htop_jets.clear();
        for(CorrectedJets::const_iterator jet = best_solution.htop_jets.begin();
                best_solution.htop_jets.end() != jet;
                ++jet)
        {
            result.htop_jets.push_back(*jet);
        }

        result.ltop_jets.clear();
        for(CorrectedJets::const_iterator jet = best_solution.ltop_jets.begin();
                best_solution.ltop_jets.end() != jet;
                ++jet)
        {
            result.ltop_jets.push_back(*jet);
        }

        sort(result.htop_jets.begin(), result.htop_jets.end(), CorrectedPtGreater()); 
        sort(result.ltop_jets.begin(), result.ltop_jets.end(), CorrectedPtGreater()); 

        result.valid = true;
    }

    return result;
}
