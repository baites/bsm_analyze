// Common Selectors Classes/functions
//
// Physics objects selectors: muons, electrons, jets
//
// Created by Samvel Khalatyan, May 16, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_SELECTOR
#define BSM_SELECTOR

#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "bsm_core/interface/Object.h"
#include "bsm_input/interface/bsm_input_fwd.h"
#include "interface/bsm_fwd.h"

namespace bsm
{
    // Selector Interface. Each selector can be enabled or disabled
    //
    class Selector : public core::Object
    {
        public:
            Selector() {}
            Selector(const Selector &);

            // Enable disable all cuts
            //
            virtual void enable();
            virtual void disable();

            // Object interface
            //
            virtual void print(std::ostream &) const;

        protected:
            // Access cut. Throw out_of_range exception if id is not valid. ID
            // starts counting from 0
            //
            CutPtr getCut(const uint32_t &id) const;

            // Add cut to monitorables
            //
            void addCut(const uint32_t &id, const CutPtr &);

            void removeCut(const uint32_t &id);

            // Get number of registered cuts
            //
            uint32_t cuts() const;

        private:
            typedef std::map<uint32_t, CutPtr> Cuts;

            Cuts _cuts;
    };

    class ElectronSelector : public Selector
    {
        public:
            enum Cut
            {
                PT = 0,
                ETA,
                PRIMARY_VERTEX
            };

            ElectronSelector();

            CutPtr cut(const Cut &) const;

            // Test if electron passes the selector
            //
            bool apply(const Electron &, const PrimaryVertex &);

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
    };

    class JetSelector : public Selector
    {
        public:
            enum Cut
            {
                PT = 0,
                ETA
            };

            JetSelector();

            CutPtr cut(const Cut &) const;

            // Test if object passes the selector
            //
            virtual bool apply(const Jet &);

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
    };

    class P4Selector : public Selector
    {
        public:
            enum Cut
            {
                PT = 0,
                ETA
            };

            P4Selector();

            CutPtr cut(const Cut &) const;

            // Test if object passes the selector
            //
            virtual bool apply(const LorentzVector &);

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
    };

    class MultiplicityCutflow : public Selector
    {
        public:
            MultiplicityCutflow(const uint32_t &max);

            // Cuts accessors
            //
            CutPtr cut(const uint32_t &) const;

            // Apply cutflow to a number
            //
            virtual bool apply(const uint32_t &);

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
    };

    class MuonSelector : public Selector
    {
        public:
            enum Cut
            {
                PT = 0,
                ETA,
                IS_GLOBAL,
                IS_TRACKER,
                MUON_SEGMENTS,
                MUON_HITS,
                MUON_NORMALIZED_CHI2,
                TRACKER_HITS,
                PIXEL_HITS,
                D0,
                PRIMARY_VERTEX
            };

            MuonSelector();

            CutPtr cut(const Cut &) const;

            // Test if muon passes the selector
            //
            virtual bool apply(const Muon &, const PrimaryVertex &);

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
    };

    class PrimaryVertexSelector : public Selector
    {
        public:
            enum Cut
            {
                NDOF = 0,
                VERTEX_Z,
                RHO
            };

            PrimaryVertexSelector();

            CutPtr cut(const Cut &) const;

            // Test if muon passes the selector
            //
            virtual bool apply(const PrimaryVertex &);

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
    };

    class WJetSelector : public Selector
    {
        public:
            enum Cut
            {
                CHILDREN,
                PT,
                MASS_DROP,
                MASS
            };

            WJetSelector();

            CutPtr cut(const Cut &) const;

            // Test if object passes the selector
            //
            virtual bool apply(const Jet &);

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
    };

    class LockSelectorEventCounterOnUpdate
    {
        public:
            LockSelectorEventCounterOnUpdate(ElectronSelector &);
            LockSelectorEventCounterOnUpdate(JetSelector &);
            LockSelectorEventCounterOnUpdate(P4Selector &);
            LockSelectorEventCounterOnUpdate(MuonSelector &);
            LockSelectorEventCounterOnUpdate(PrimaryVertexSelector &);
            LockSelectorEventCounterOnUpdate(WJetSelector &);

        private:
            typedef boost::shared_ptr<LockCounterOnUpdate> Locker;

            std::vector<Locker> _lockers;
    };
}

#endif
