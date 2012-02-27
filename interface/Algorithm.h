// Common Algorithm Classes/functions
//
// Useful tools that are heavily used in the analysis
//
// Created by Samvel Khalatyan, Apr 25, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_ALGORITHM
#define BSM_ALGORITHM

#include <vector>

#include <boost/shared_ptr.hpp>

#include "bsm_core/interface/Object.h"
#include "bsm_input/interface/bsm_input_fwd.h"
#include "interface/bsm_fwd.h"
#include "interface/DecayGenerator.h"
#include "interface/SynchSelector.h"

namespace bsm
{
    // Given the Decay:
    //
    //      A -> B + Neutrino
    //
    // reconstruct the Neutrino pZ component of the Lorentz Vector given
    // it's Energy, px, and py are known.
    //
    // Calculation is carried with formula:
    //
    //      P4A^2 = (P4B + P4Neutrino)^2
    //
    // assuming:
    //
    //      P4Neutrino^2 = 0
    //      P4B^2 = 0
    //
    // within the SM: m_neutrino = 0, mass of the second decay product
    // is neglected due to expected small mass, e.g. in case of the
    // electron: m_B = 0.5 MeV, for the muon: m_mu = 105 MeV and mass
    // of the W-boson: m_W = 80 GeV. m_B is used in formula in form:
    //
    //      m_A^2 - m_B^2
    //
    //  and therefore m_B can be neglected.
    //
    class NeutrinoReconstruct : public core::Object
    {
        public:
            typedef boost::shared_ptr<LorentzVector> LorentzVectorPtr;
            typedef std::vector<LorentzVectorPtr> Solutions;

            NeutrinoReconstruct();
            NeutrinoReconstruct(const NeutrinoReconstruct &);

            // return found solutions. Real part of the solution is taken
            // in case of the imaginary solution
            //
            Solutions operator()(const LorentzVector &lepton,
                    const LorentzVector &neutrino);

            int solutions() const;

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;

            virtual void print(std::ostream &) const;

        private:
            int _solutions; // number of solutions found:
                            //  0 - complex, 1, 2 - real
    };

    class TTbarDeltaRReconstruct : public core::Object
    {
        public:
            typedef std::vector<LorentzVector *> Jets;
            typedef boost::shared_ptr<LorentzVector> LorentzVectorPtr;

            struct TTbar
            {
                LorentzVectorPtr top;
                LorentzVectorPtr tbar;
            };

            TTbarDeltaRReconstruct();
            TTbarDeltaRReconstruct(const TTbarDeltaRReconstruct &);

            // Function will return combined DeltaR:
            //
            //  DR = DR_leptonic + DR_hadronic
            //
            TTbar operator()(const Jets &,
                    const LorentzVector &lepton,
                    const LorentzVector &neutrino);

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;

            virtual void print(std::ostream &) const;

        private:
            void minimize(const Jets &,
                    const LorentzVector &lepton,
                    const LorentzVector &neutrino,
                    const Jets::const_iterator &jet_leptonic,
                    const Jets::const_iterator &jet_hadronic_1,
                    const Jets::const_iterator &jet_hadronic_2,
                    const Jets::const_iterator &jet_hadronic_3);

            float _dr_min;
            float _dr_max;

            TTbar _ttbar;
    };

    class JetsSelector
    {
        public:
            typedef TTbarDeltaRReconstruct::Jets Jets;
            typedef std::vector<Jets::const_iterator> SelectedJets;

            JetsSelector(const Jets &jets, const uint32_t &size);

            bool next();

        private:
            bool isValid(Jets::const_iterator &);
            bool next(SelectedJets::reverse_iterator &iterator);
            bool next(Jets::const_iterator &jet);

            const Jets &_jets;

            SelectedJets _selected_jets;
    };

    class JetIterator
    {
        public:
            typedef TTbarDeltaRReconstruct::Jets Jets;

            JetIterator(const Jets &jets, const bool &is_valid = true);

            bool isValid() const;

            const Jets::const_iterator iterator() const;

            // Prefix increment
            //
            void operator++();

        private:
            bool _is_valid;

            const Jets &_jets;
            Jets::const_iterator _jet;
    };

    class ResonanceReconstructor: public core::Object
    {
        public:
            typedef DecayGenerator<CorrectedJet> Generator;
            typedef Generator::Iterators Iterators;
            typedef std::vector<CorrectedJet> CorrectedJets;
            typedef std::vector<LorentzVector> LorentzVectors;

            struct Mttbar
            {
                Mttbar():
                    htop_njets(0),
                    solutions(0),
                    valid(false)
                {
                }

                LorentzVector mttbar;
                LorentzVector wlep;
                LorentzVector whad;
                LorentzVector neutrino;     // Selected MET solution
                LorentzVectors neutrinos;   // All MET solutions
                LorentzVector ltop;
                LorentzVector htop;

                CorrectedJets htop_jets;
                CorrectedJets ltop_jets;

                LorentzVector ltop_jet;

                int htop_njets;
                int solutions;

                bool valid;
            };

            Mttbar run(const LorentzVector &lepton,
                       const LorentzVector &met,
                       const SynchSelector::GoodJets &) const;

            // Object interface
            //
            virtual void print(std::ostream &) const;

        protected:
            virtual bool isValidHadronicSide(const LorentzVector &,
                                             const Iterators &) const = 0;

            virtual bool isValidLeptonicSide(const LorentzVector &,
                                             const Iterators &) const = 0;

            virtual bool isValidNeutralSide(const LorentzVector &,
                                            const Iterators &) const = 0;

            virtual LorentzVector getLeptonicJet(const Iterators &) const = 0;

            virtual float getHadronicDiscriminator(
                    const LorentzVector &ltop,
                    const LorentzVector &htop) const;
    };

    class SimpleResonanceReconstructor: public ResonanceReconstructor
    {
        public:
            // Object interface
            //
            virtual uint32_t id() const;
            virtual ObjectPtr clone() const;

        protected:
            virtual bool isValidHadronicSide(const LorentzVector &,
                                             const Iterators &) const;

            virtual bool isValidLeptonicSide(const LorentzVector &,
                                             const Iterators &) const;

            virtual bool isValidNeutralSide(const LorentzVector &,
                                            const Iterators &) const;

            virtual LorentzVector getLeptonicJet(const Iterators &) const;
    };

    class BtagResonanceReconstructor: public SimpleResonanceReconstructor
    {
        public:
            // Object interface
            //
            virtual uint32_t id() const;
            virtual ObjectPtr clone() const;

        protected:
            virtual bool isValidHadronicSide(const LorentzVector &,
                                             const Iterators &) const;

            virtual bool isValidLeptonicSide(const LorentzVector &,
                                             const Iterators &) const;

            virtual bool isValidNeutralSide(const LorentzVector &,
                                            const Iterators &) const;

            virtual LorentzVector getLeptonicJet(const Iterators &) const;

        private:
            uint32_t countBtags(const Iterators &) const;
            bool isBtagJet(const Jet *jet) const;
    };

    class SimpleDrResonanceReconstructor: public SimpleResonanceReconstructor
    {
        public:
            SimpleDrResonanceReconstructor():
                _leptonic_dr(1),
                _hadronic_dr(3.14)
            {
            }

            // Object interface
            //
            virtual uint32_t id() const;
            virtual ObjectPtr clone() const;

        protected:
            virtual bool isValidHadronicSide(const LorentzVector &,
                                             const Iterators &) const;

            virtual bool isValidLeptonicSide(const LorentzVector &,
                                             const Iterators &) const;

            virtual bool isValidNeutralSide(const LorentzVector &,
                                            const Iterators &) const;

        private:
            const float _leptonic_dr;
            const float _hadronic_dr;
    };

    class HemisphereResonanceReconstructor: public SimpleResonanceReconstructor
    {
        public:
            HemisphereResonanceReconstructor():
                _half_pi(3.14159265 / 2)
            {
            }

            // Object interface
            //
            virtual uint32_t id() const;
            virtual ObjectPtr clone() const;

        protected:
            virtual bool isValidHadronicSide(const LorentzVector &,
                                             const Iterators &) const;

            virtual bool isValidLeptonicSide(const LorentzVector &,
                                             const Iterators &) const;

            virtual bool isValidNeutralSide(const LorentzVector &,
                                            const Iterators &) const;

        private:
            const float _half_pi;
    };

    class SimpleResonanceReconstructorWithMass: public SimpleResonanceReconstructor
    {
        public:
            // Object interface
            //
            virtual uint32_t id() const;
            virtual ObjectPtr clone() const;

        protected:
            virtual float getHadronicDiscriminator(
                    const LorentzVector &ltop,
                    const LorentzVector &htop) const;
    };
}

#endif
