// Common Algorithm Classes/functions
//
// Useful tools that are heavily used in the analysis
//
// Created by Samvel Khalatyan, Apr 25, 2011
// Copyright 2011, All rights reserved

#include <cfloat>

#include <boost/pointer_cast.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Jet.pb.h"
#include "bsm_input/interface/Physics.pb.h"
#include "interface/Algorithm.h"
#include "interface/Utility.h"

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
            deltaRmin(FLT_MAX),
            deltaRlh(0),
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

        float deltaRmin;
        float deltaRlh;
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

            const float deltaRmin = dr(ltop_tmp, ltop_jet)
                + dr(ltop_tmp, lepton)
                + dr(ltop_tmp, neutrino_p4);

            const float deltaRlh = dr(ltop_tmp, htop);

            if (deltaRmin < best_solution.deltaRmin
                    || (deltaRmin == best_solution.deltaRmin
                        && deltaRlh > best_solution.deltaRlh))
            {
                best_solution.deltaRmin = deltaRmin;
                best_solution.deltaRlh = deltaRlh;
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



/*
// Closest Jet Algorithm
ClosestJet::ClosestJet()
{
    _p4.reset(new TLorentzVector());
}

ClosestJet::ClosestJet(const ClosestJet &object)
{
    _p4.reset(new TLorentzVector());
}

const Jet *ClosestJet::find(const Jets &jets,
                                       const Electron &electron)
{
    if (!jets.size())
        return 0;

    utility::set(_p4.get(), &electron.physics_object().p4());

    return find(jets);
}

const Jet *ClosestJet::find(const Jets &jets, const Muon &muon)
{
    if (!jets.size())
        return 0;

    utility::set(_p4.get(), &muon.physics_object().p4());

    return find(jets);
}

uint32_t ClosestJet::id() const
{
    return ID<ClosestJet>::get();
}

ClosestJet::ObjectPtr ClosestJet::clone() const
{
    return ObjectPtr(new ClosestJet(*this));
}

void ClosestJet::print(std::ostream &out) const
{
    out << "nothing to print for the closest jet";
}

// Privates
//
const Jet *ClosestJet::find(const Jets &jets)
{
    TLorentzVector lepton_p4(*_p4);

    float min_delta_r = -1;
    const Jet *closest_jet = 0;
    for(Jets::const_iterator jet = jets.begin();
            jets.end() != jet;
            ++jet)
    { 
        utility::set(_p4.get(), &jet->physics_object().p4());

        float delta_r = _p4->DeltaR(lepton_p4);

        if (-1 == min_delta_r
                || delta_r < min_delta_r)
        {
            min_delta_r = delta_r;

            closest_jet = &*jet;
        }
    }

    return closest_jet;
}



// Neutrino Momenturm Reconstructor
//
NeutrinoReconstruct::NeutrinoReconstruct(const float &mass_a,
        const float &mass_b):
    _mass_a(mass_a),
    _mass_b(mass_b),
    _solutions(0)
{
    _solution_one.reset(new LorentzVector());
    _solution_two.reset(new LorentzVector());
}

NeutrinoReconstruct::NeutrinoReconstruct(const NeutrinoReconstruct &obj):
    _mass_a(obj._mass_a),
    _mass_b(obj._mass_b),
    _solutions(obj._solutions)
{
    _solution_one.reset(new LorentzVector());
    _solution_two.reset(new LorentzVector());

    *_solution_one = *obj._solution_one;
    *_solution_two = *obj._solution_two;
}

uint32_t NeutrinoReconstruct::apply(const LorentzVector &p4,
        const LorentzVector &met)
{
    reset();

    float a = _mass_a * _mass_a
        - _mass_b * _mass_b
        + 2 * p4.px() * met.px()
        + 2 * p4.py() * met.py();

    // The final equation is:
    //
    //  4 * ( E_B^2 - pz_B^2) * x^2
    //  - 4 * a * pz_B * x
    //  + [4 E_B^2 * (px_nu^2 + py_nu^2) - a^2] = 0
    //
    //  with x being the pz_nu. OR:
    //
    //      A x^2 + 2B x + C = 0
    //
    float A = 4 * (p4.e() * p4.e() - p4.pz() * p4.pz());
    float B = -2 * a * p4.pz();
    float C = 4 * p4.e() * p4.e() * ( met.px() * met.px()
            + met.py() * met.py())
        - a * a;

    float discriminant = B * B - A * C;

    if (0 > discriminant)
    {
        // Take only real part of the solution
        _solutions = 0;

        addSolution(_solution_one, met, -B / A);
    }
    else if(0 == discriminant)
    {
        _solutions = 1;

        addSolution(_solution_one, met, -B / A);
    }
    else
    {
        _solutions = 2;

        discriminant = sqrt(discriminant);

        addSolution(_solution_one, met, (-B - discriminant) / A);
        addSolution(_solution_two, met, (-B + discriminant) / A);
    }

    return _solutions;
}

NeutrinoReconstruct::P4Ptr
    NeutrinoReconstruct::solution(const uint32_t &solution) const
{
    if (!_solutions
            || !solution)
        return _solution_one;

    return _solution_two;
}

void NeutrinoReconstruct::reset()
{
    _solutions = 0;

    setSolution(_solution_one, 0, 0, 0, 0);
    setSolution(_solution_two, 0, 0, 0, 0);
}

uint32_t NeutrinoReconstruct::id() const
{
    return ID<NeutrinoReconstruct>::get();
}

NeutrinoReconstruct::ObjectPtr NeutrinoReconstruct::clone() const
{
    return ObjectPtr(new NeutrinoReconstruct(*this));
}

void NeutrinoReconstruct::merge(const ObjectPtr &object_pointer)
{
    if (id() != object_pointer->id())
        return;

    const boost::shared_ptr<NeutrinoReconstruct> object =
        dynamic_pointer_cast<NeutrinoReconstruct>(object_pointer);

    if (!object)
        return;

    _mass_a = object->_mass_a;
    _mass_b = object->_mass_b;
    _solutions = object->_solutions;

    *_solution_one = *object->_solution_one;
    *_solution_two = *object->_solution_two;
}

void NeutrinoReconstruct::print(std::ostream &out) const
{
    out << _solutions << " solution(s): "
        << *_solution_one << " " << *_solution_two;
}

// Privates
//
void NeutrinoReconstruct::addSolution(P4Ptr &solution,
        const LorentzVector &p4,
        const float &pz)
{
    setSolution(solution, p4.e(), p4.px(), p4.py(), pz);
}

void NeutrinoReconstruct::setSolution(P4Ptr &solution,
        const float &e,
        const float &px,
        const float &py,
        const float &pz)
{
    solution->set_e(e);
    solution->set_px(px);
    solution->set_py(py);
    solution->set_pz(pz);
}



// Hadronic Decay
//
HadronicDecay::HadronicDecay():
    _dr(0),
    _dr_w_top(0),
    _dr_b_top(0)
{
    _top.reset(new LorentzVector());
}

HadronicDecay::HadronicDecay(const HadronicDecay &object):
    _dr(object._dr),
    _dr_w_top(object._dr_w_top),
    _dr_b_top(object._dr_b_top)
{
    _top.reset(new LorentzVector());

    *_top = *object._top;
}

const HadronicDecay::P4Ptr HadronicDecay::top() const
{
    return _top;
}

float HadronicDecay::dr() const
{
    return _dr;
}

float HadronicDecay::dr_w_top() const
{
    return _dr_w_top;
}

float HadronicDecay::dr_b_top() const
{
    return _dr_b_top;
}

float HadronicDecay::apply(const LorentzVector &w, const LorentzVector &b)
{
    _top->Clear();

    *_top += w;
    *_top += b;

    _dr_w_top = dr(w, *_top);
    _dr_b_top = dr(b, *_top);
    _dr = _dr_w_top + _dr_b_top;

    return _dr;
}

void HadronicDecay::reset()
{
    _top->Clear();

    _dr = 0;
    _dr_w_top = 0;
    _dr_b_top = 0;
}

uint32_t HadronicDecay::id() const
{
    return core::ID<HadronicDecay>::get();
}

HadronicDecay::ObjectPtr HadronicDecay::clone() const
{
    return ObjectPtr(new HadronicDecay(*this));
}

void HadronicDecay::merge(const ObjectPtr &object_pointer)
{
    if (id() != object_pointer->id())
        return;

    const boost::shared_ptr<HadronicDecay> object =
        dynamic_pointer_cast<HadronicDecay>(object_pointer);

    if (!object)
        return;

    _dr = object->dr();
    _dr_w_top = object->dr_w_top();
    _dr_b_top = object->dr_b_top();

    *_top = *object->_top;
}

void HadronicDecay::print(std::ostream &out) const
{
    out << "dr: " << dr()
        << " dr(w,t): " << dr_w_top()
        << " dr(b,t): " << dr_b_top();
}



// Leptonic Decay
//
LeptonicDecay::LeptonicDecay():
    _dr(0),
    _dr_l_top(0),
    _dr_nu_top(0),
    _dr_b_top(0)
{
    _top.reset(new LorentzVector());
}

LeptonicDecay::LeptonicDecay(const LeptonicDecay &object):
    _dr(object._dr),
    _dr_l_top(object._dr_l_top),
    _dr_nu_top(object._dr_nu_top),
    _dr_b_top(object._dr_b_top)
{
    _top.reset(new LorentzVector());

    *_top = *object._top;
}

const LeptonicDecay::P4Ptr LeptonicDecay::top() const
{
    return _top;
}

float LeptonicDecay::dr() const
{
    return _dr;
}

float LeptonicDecay::dr_l_top() const
{
    return _dr_l_top;
}

float LeptonicDecay::dr_nu_top() const
{
    return _dr_nu_top;
}

float LeptonicDecay::dr_b_top() const
{
    return _dr_b_top;
}

float LeptonicDecay::apply(const LorentzVector &l,
        const LorentzVector &nu,
        const LorentzVector &b)
{
    _top->Clear();

    *_top += l;
    *_top += nu;
    *_top += b;

    _dr_l_top = dr(l, *_top);
    _dr_nu_top = dr(nu, *_top);
    _dr_b_top = dr(b, *_top);
    _dr = _dr_l_top + _dr_nu_top + _dr_b_top;

    return _dr;
}

void LeptonicDecay::reset()
{
    _top->Clear();

    _dr = 0;
    _dr_l_top = 0;
    _dr_nu_top = 0;
    _dr_b_top = 0;
}

uint32_t LeptonicDecay::id() const
{
    return core::ID<LeptonicDecay>::get();
}

LeptonicDecay::ObjectPtr LeptonicDecay::clone() const
{
    return ObjectPtr(new LeptonicDecay(*this));
}

void LeptonicDecay::merge(const ObjectPtr &object_pointer)
{
    if (id() != object_pointer->id())
        return;

    const boost::shared_ptr<LeptonicDecay> object =
        dynamic_pointer_cast<LeptonicDecay>(object_pointer);

    if (!object)
        return;

    _dr = object->dr();
    _dr_l_top = object->dr_l_top();
    _dr_nu_top = object->dr_nu_top();
    _dr_b_top = object->dr_b_top();

    *_top = *object->_top;
}

void LeptonicDecay::print(std::ostream &out) const
{
    out << "dr: " << dr()
        << " dr(l,t): " << dr_l_top()
        << " dr(nu,t): " << dr_nu_top()
        << " dr(b,t): " << dr_b_top();
}
*/
