// Common Algorithm Classes/functions
//
// Useful tools that are heavily used in the analysis
//
// Created by Samvel Khalatyan, Apr 25, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_ALGORITHM
#define BSM_ALGORITHM

#include <boost/shared_ptr.hpp>

#include "bsm_core/interface/Object.h"
#include "bsm_input/interface/bsm_input_fwd.h"
#include "bsm_input/interface/Jet.pb.h"

class TLorentzVector;

namespace bsm
{
    namespace algorithm
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

                NeutrinoReconstruct(const double &mass_a,
                        const double &mass_b);
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
                        const double &pz);

                void setSolution(P4Ptr &,
                        const double &,
                        const double &,
                        const double &,
                        const double &);

                double _mass_a;
                double _mass_b;

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

                double apply(const LorentzVector &w, const LorentzVector &b);

                double dr() const;
                double dr_w_top() const;
                double dr_b_top() const;

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

                double _dr;
                double _dr_w_top;
                double _dr_b_top;
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

                double apply(const LorentzVector &l,
                        const LorentzVector &nu,
                        const LorentzVector &b);

                double dr() const;
                double dr_l_top() const;
                double dr_nu_top() const;
                double dr_b_top() const;

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

                double _dr;
                double _dr_l_top;
                double _dr_nu_top;
                double _dr_b_top;
        };
    }

    using algorithm::ClosestJet;
    using algorithm::NeutrinoReconstruct;
    using algorithm::HadronicDecay;
    using algorithm::LeptonicDecay;
}

#endif
