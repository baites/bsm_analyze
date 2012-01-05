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

    /*
    namespace dev 
    {
        // Search for closest jet to electron, muon
        //
        class ClosestJet : public core::Object
        {
            public:
                typedef ::google::protobuf::RepeatedPtrField<Jet>
                    Jets;

                ClosestJet();
                ClosestJet(const ClosestJet &);

                const Jet *find(const Jets &, const Electron &);
                const Jet *find(const Jets &, const Muon &);

                // Object interface
                //
                virtual uint32_t id() const;

                virtual ObjectPtr clone() const;
                using Object::merge;

                virtual void print(std::ostream &) const;

            private:
                // Prevent copying
                //
                ClosestJet &operator =(const ClosestJet &);

                typedef boost::shared_ptr<TLorentzVector> P4;

                const Jet *find(const Jets &);

                P4 _p4;
        };

        // Given the Decay:
        //
        //      A -> B + Neutrino
        //
        // reconstruct the Neutrino pZ component of the Lorentz Vector if
        // it's Energy, px, and py are known.
        //
        // Calculation is carried with formula:
        //
        //      P4A^2 = (P4B + P4Neutrino)^2
        //
        // and
        //
        //      P4Neutrino^2 = 0
        //
        //  (within the SM: m_neutrino = 0)
        //
        class NeutrinoReconstruct : public core::Object
        {
            public:
                typedef boost::shared_ptr<LorentzVector> P4Ptr;

                NeutrinoReconstruct(const float &mass_a,
                        const float &mass_b);
                NeutrinoReconstruct(const NeutrinoReconstruct &);

                // return number of solutions found
                //  0   imaginary solution
                //  1   only one solution (discriminator = 0)
                //  2   two solutions are found
                //
                uint32_t apply(const LorentzVector &p4_b,
                        const LorentzVector &missing_energy);

                // Solutions are counted from 0
                //
                P4Ptr solution(const uint32_t &) const;

                void reset();

                // Object interface
                //
                virtual uint32_t id() const;

                virtual ObjectPtr clone() const;
                virtual void merge(const ObjectPtr &);

                virtual void print(std::ostream &) const;

            private:
                // Prevent copying
                //
                NeutrinoReconstruct &operator =(const NeutrinoReconstruct &);

                void addSolution(P4Ptr &,
                        const LorentzVector &,
                        const float &pz);

                void setSolution(P4Ptr &,
                        const float &,
                        const float &,
                        const float &,
                        const float &);

                float _mass_a;
                float _mass_b;

                uint32_t _solutions;

                P4Ptr _solution_one;
                P4Ptr _solution_two;
        };

        // Hadronic decay of the t-quark:
        //
        //      t -> W + b
        //
        // Given w-tagged jet and b-jet:
        //
        //      1. Reconstruct top P4
        //      2. Calculate DeltaR(Wjet, top)
        //      4.      "    DeltaR(bjet, top)
        //      5.      "    Combo of 2 and 4
        //
        class HadronicDecay : public core::Object
        {
            public:
                typedef boost::shared_ptr<LorentzVector> P4Ptr;

                HadronicDecay();
                HadronicDecay(const HadronicDecay &);

                const P4Ptr top() const;

                float dr() const;
                float dr_w_top() const;
                float dr_b_top() const;

                float apply(const LorentzVector &w, const LorentzVector &b);

                void reset();

                // Object interface
                //
                virtual uint32_t id() const;

                virtual ObjectPtr clone() const;
                virtual void merge(const ObjectPtr &);

                virtual void print(std::ostream &) const;

            private:
                // Prevent copying
                //
                HadronicDecay &operator =(const HadronicDecay &);

                P4Ptr _top;

                float _dr;
                float _dr_w_top;
                float _dr_b_top;
        };

        // Leptonic decay of the t-quark:
        //
        //      t -> l + neutrino + b
        //
        // Given lepton, neutrino and b-jet:
        //
        //      1. Reconstruct the t-quark P4
        //      2. Calculate DeltaR(l, top)
        //      3.      "    DeltaR(nu, top)
        //      4.      "    DeltaR(b, top)
        //      5. Combine 2) - 4)
        //
        class LeptonicDecay : public core::Object
        {
            public:
                typedef boost::shared_ptr<LorentzVector> P4Ptr;

                LeptonicDecay();
                LeptonicDecay(const LeptonicDecay &);

                const P4Ptr top() const;

                float dr() const;
                float dr_l_top() const;
                float dr_nu_top() const;
                float dr_b_top() const;

                float apply(const LorentzVector &l,
                        const LorentzVector &nu,
                        const LorentzVector &b);

                void reset();

                // Object interface
                //
                virtual uint32_t id() const;

                virtual ObjectPtr clone() const;
                virtual void merge(const ObjectPtr &);

                virtual void print(std::ostream &) const;

            private:
                // Prevent copying
                //
                LeptonicDecay &operator =(const LeptonicDecay &);

                P4Ptr _top;

                float _dr;
                float _dr_l_top;
                float _dr_nu_top;
                float _dr_b_top;
        };

        class TTbarDeltaRReconstruct : public core::Object
        {
            public:
                typedef std::vector<const Jet *> Jets;
                typedef boost::shared_ptr<HadronicDecay> HadronicPtr;
                typedef boost::shared_ptr<LeptonicDecay> LeptonicPtr;

                TTbarDeltaRReconstruct();
                TTbarDeltaRReconstruct(const TTbarDeltaRReconstruct &);

                float dr() const;

                HadronicPtr hadronicDecay() const;
                LeptonicPtr leptonicDecay() const;

                // Function will return combined DeltaR:
                //
                //  DR = DR_leptonic + DR_hadronic
                //
                float apply(const Jets &,
                        const LorentzVector &lepton,
                        const LorentzVector &missing_energy,
                        const LorentzVector &wjet);

                void reset();

                // Object interface
                //
                virtual uint32_t id() const;

                virtual ObjectPtr clone() const;
                virtual void merge(const ObjectPtr &);

                virtual void print(std::ostream &) const;

            private:
                // Prevent copying
                //
                TTbarDeltaRReconstruct &operator =(const TTbarDeltaRReconstruct &);

                float minimize(const Jets &,
                        const Jets::const_iterator &jet1,
                        const Jets::const_iterator &jet2,
                        const LorentzVector &lepton,
                        const LorentzVector &missing_energy,
                        const LorentzVector &wjet);

                float _dr;
                
                HadronicPtr _hadronic;
                LeptonicPtr _leptonic;
        };
    }
    */
}

#endif
