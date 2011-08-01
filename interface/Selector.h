// Common Selectors Classes/functions
//
// Physics objects selectors: muons, electrons, jets
//
// Created by Samvel Khalatyan, May 16, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_SELECTOR
#define BSM_SELECTOR

#include <algorithm>
#include <iosfwd>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "bsm_core/interface/Object.h"
#include "bsm_input/interface/bsm_input_fwd.h"
#include "JetMETObjects/interface/JetCorrectorParameters.h"
#include "interface/AppController.h"
#include "interface/Cut.h"

namespace bsm
{
    typedef boost::shared_ptr<Cut> CutPtr;

    // Common interface for all selectors. Each selector knows how to:
    //
    //  1. print
    //  2. clone
    //  3. merge
    //
    // the last two are required for the proper use in threads
    //
    class Selector : public core::Object
    {
        public:
            // Enable disable all cuts
            //
            virtual void enable() = 0;
            virtual void disable() = 0;
    };

    class ElectronSelector : public Selector
    {
        public:
            ElectronSelector();
            ElectronSelector(const ElectronSelector &);

            // Test if electron passes the selector
            //
            virtual bool apply(const Electron &, const PrimaryVertex &);

            // Cuts accessors
            //
            CutPtr pt() const;
            CutPtr eta() const;
            CutPtr primary_vertex() const;

            // Selector interface
            //
            virtual void enable();
            virtual void disable();

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
            using Object::merge;

            virtual void print(std::ostream &) const;

        private:
            // Prevent copying
            //
            ElectronSelector &operator =(const ElectronSelector &);

            CutPtr _pt;
            CutPtr _eta;
            CutPtr _primary_vertex;
    };

    class JetSelector : public Selector
    {
        public:
            JetSelector();
            JetSelector(const JetSelector &);

            // Test if object passes the selector
            //
            virtual bool apply(const Jet &);

            // Cuts accessors
            //
            CutPtr pt() const;
            CutPtr eta() const;

            // Selector interface
            //
            virtual void enable();
            virtual void disable();

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
            using Object::merge;

            virtual void print(std::ostream &) const;

        private:
            CutPtr _pt;
            CutPtr _eta;
    };

    class MultiplicityCutflow : public Selector
    {
        public:
            MultiplicityCutflow(const uint32_t &max);
            MultiplicityCutflow(const MultiplicityCutflow &);

            // Apply cutflow to a number
            //
            virtual void apply(const uint32_t &);

            // Cuts accessors
            //
            CutPtr cut(const uint32_t &) const;

            virtual void enable();
            virtual void disable();

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
            using Object::merge;

            virtual void print(std::ostream &) const;

        private:
            // Prevent copying
            //
            MultiplicityCutflow &operator =(const MultiplicityCutflow &);

            typedef std::vector<CutPtr> Cuts;

            Cuts _cuts;
    };

    class MuonSelector : public Selector
    {
        public:
            MuonSelector();
            MuonSelector(const MuonSelector &);

            // Test if muon passes the selector
            //
            virtual bool apply(const Muon &, const PrimaryVertex &);

            // Cuts accessors
            //
            CutPtr pt() const;
            CutPtr eta() const;
            CutPtr is_global() const;
            CutPtr is_tracker() const;
            CutPtr muon_segments() const;
            CutPtr muon_hits() const;
            CutPtr muon_normalized_chi2() const;
            CutPtr tracker_hits() const;
            CutPtr pixel_hits() const;
            CutPtr d0_bsp() const;
            CutPtr primary_vertex() const;

            // Selector interface
            //
            virtual void enable();
            virtual void disable();

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
            using Object::merge;

            virtual void print(std::ostream &) const;

        private:
            // Prevent copying
            //
            MuonSelector &operator =(const MuonSelector &);

            CutPtr _pt;
            CutPtr _eta;
            CutPtr _is_global;
            CutPtr _is_tracker;
            CutPtr _muon_segments;
            CutPtr _muon_hits;
            CutPtr _muon_normalized_chi2;
            CutPtr _tracker_hits;
            CutPtr _pixel_hits;
            CutPtr _d0_bsp;
            CutPtr _primary_vertex;
    };

    class PrimaryVertexSelector : public Selector
    {
        public:
            PrimaryVertexSelector();
            PrimaryVertexSelector(const PrimaryVertexSelector &);

            // Test if muon passes the selector
            //
            virtual bool apply(const PrimaryVertex &);

            // Cuts accessors
            //
            CutPtr ndof() const;
            CutPtr vertex_z() const;
            CutPtr rho() const;

            // Selector interface
            //
            virtual void enable();
            virtual void disable();

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
            using Object::merge;

            virtual void print(std::ostream &) const;

        private:
            // Prevent copying
            //
            PrimaryVertexSelector &operator =(const PrimaryVertexSelector &);

            CutPtr _ndof;
            CutPtr _vertex_z;
            CutPtr _rho;
    };

    // Synchronization Exercise Selector
    //
    class SynchSelectorDelegate
    {
        public:
            enum LeptonMode
            {
                ELECTRON = 0,
                MUON
            };

            enum CutMode
            {
                CUT_2D = 0,
                ISOLATION
            };

            virtual ~SynchSelectorDelegate() {}

            virtual void setLeptonMode(const LeptonMode &) {}
            virtual void setCutMode(const CutMode &) {}
    };

    class SynchSelectorOptions : public Options
    {
        public:
            SynchSelectorOptions();
            virtual ~SynchSelectorOptions();

            // Options interface
            //
            virtual OptionsPtr options() const;

            void setLeptonMode(std::string);
            void setCutMode(std::string);

            void setDelegate(SynchSelectorDelegate *);
            SynchSelectorDelegate *delegate() const;

        private:
            SynchSelectorDelegate *_delegate;

            OptionsPtr _options;
    };

    class SynchSelector : public Selector,
        public SynchSelectorDelegate
    {
        public:
            typedef boost::shared_ptr<MultiplicityCutflow> CutflowPtr;
            typedef std::vector<JetCorrectorParameters> Corrections;

            enum Selection
            {
                PRESELECTION = 0,
                SCRAPING,
                HBHENOISE,
                PRIMARY_VERTEX,
                JET,
                LEPTON,
                VETO_SECOND_LEPTON,
                CUT_LEPTON,
                LEADING_JET,
                HTLEP,

                SELECTIONS // this item should always be the last one
            };

            SynchSelector(const LeptonMode & = ELECTRON,
                    const CutMode & = CUT_2D);
            SynchSelector(const SynchSelector &);

            // Test if muon passes the selector
            //
            virtual bool apply(const Event *);

            CutflowPtr cutflow() const;

            JetEnergyCorrectionDelegate *getJetEnergyCorrectionDelegate() const;

            // SynchSelectorDelegate interface
            //
            virtual void setLeptonMode(const LeptonMode &);
            virtual void setCutMode(const CutMode &);

            // Selector interface
            //
            virtual void enable();
            virtual void disable();

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
            using Object::merge;

            virtual void print(std::ostream &) const;

        private:
            struct CorrectedJet
            {
                CorrectedJet()
                {
                    jet = 0;
                }

                const Jet *jet;
                LorentzVector corrected_p4;
            };

            typedef std::vector<const Electron *> GoodElectrons;
            typedef std::vector<const Muon *> GoodMuons;
            typedef std::vector<CorrectedJet> GoodJets;

            bool primaryVertices(const Event *);
            bool jets(const Event *);
            bool lepton(const Event *);
            bool secondaryLeptonVeto(const Event *);

            void selectGoodElectrons(const Event *);
            void selectGoodMuons(const Event *);

            LeptonMode _lepton_mode;
            CutMode _cut_mode;

            CutflowPtr _cutflow;

            boost::shared_ptr<PrimaryVertexSelector> _primary_vertex_selector;
            boost::shared_ptr<ElectronSelector> _electron_selector;
            boost::shared_ptr<MuonSelector> _muon_selector;
            boost::shared_ptr<JetSelector> _nice_jet_selector;
            boost::shared_ptr<JetSelector> _good_jet_selector;

            boost::shared_ptr<JetEnergyCorrections> _jec;

            GoodElectrons _good_electrons;
            GoodMuons _good_muons;
            GoodJets _nice_jets; // pT > 25
            GoodJets _good_jets; // pT > 50
    };

    class WJetSelector : public Selector
    {
        public:
            WJetSelector();
            WJetSelector(const WJetSelector &);

            // Test if object passes the selector
            //
            virtual bool apply(const Jet &);

            // Cuts accessors
            //
            CutPtr children() const;
            CutPtr pt() const;
            CutPtr mass_drop() const;
            CutPtr mass_lower_bound() const;
            CutPtr mass_upper_bound() const;

            // Selector interface
            //
            virtual void enable();
            virtual void disable();

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
            using Object::merge;

            virtual void print(std::ostream &) const;

        private:
            // Prevent copying
            //
            WJetSelector &operator =(const WJetSelector &);

            CutPtr _children;
            CutPtr _pt;
            CutPtr _mass_drop;
            CutPtr _mass_lower_bound;
            CutPtr _mass_upper_bound;;
    };

    class LockSelectorEventCounterOnUpdate
    {
        public:
            LockSelectorEventCounterOnUpdate(ElectronSelector &);
            LockSelectorEventCounterOnUpdate(JetSelector &);
            LockSelectorEventCounterOnUpdate(MuonSelector &);
            LockSelectorEventCounterOnUpdate(WJetSelector &);

        private:
            typedef boost::shared_ptr<LockCounterOnUpdate> Locker;

            std::vector<Locker> _lockers;
    };

    // Helpers
    //
    std::ostream &operator <<(std::ostream &,
            const SynchSelector::LeptonMode &);
    std::ostream &operator <<(std::ostream &,
            const SynchSelector::CutMode &);
}

#endif
