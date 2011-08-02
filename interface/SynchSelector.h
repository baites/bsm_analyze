// Synchronization exercise selector that is used to select "good"
// events with muons, electrons, jetc, etc.
//
// Created by Samvel Khalatyan, Aug 01, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_SYNCHRONIZATION_SELECTOR
#define BSM_SYNCHRONIZATION_SELECTOR

#include <string>

#include "bsm_core/interface/Object.h"
#include "bsm_input/interface/bsm_input_fwd.h"
#include "bsm_input/interface/Physics.pb.h"
#include "JetMETObjects/interface/JetCorrectorParameters.h"
#include "interface/AppController.h"
#include "interface/Selector.h"

namespace bsm
{
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

            void setDelegate(SynchSelectorDelegate *);
            SynchSelectorDelegate *delegate() const;

            // Options interface
            //
            virtual DescriptionPtr description() const;

        private:
            void setLeptonMode(std::string);
            void setCutMode(std::string);

            SynchSelectorDelegate *_delegate;

            DescriptionPtr _description;
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

            SynchSelector()
            SynchSelector(const SynchSelector &);

            // Test if muon passes the selector
            //
            virtual bool apply(const Event *);

            JetEnergyCorrectionDelegate *getJetEnergyCorrectionDelegate() const;

            // SynchSelectorDelegate interface
            //
            virtual void setLeptonMode(const LeptonMode &);
            virtual void setCutMode(const CutMode &);

            // Selector interface
            //
            // Note: empty at the moment
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
            typedef boost::shared_ptr<LorentzVector> LorentzVectorPtr;

            struct CorrectedJet
            {
                CorrectedJet()
                {
                    jet = 0;
                }

                const Jet *jet;
                LorentzVectorPtr corrected_p4;
            };

            typedef std::vector<const Electron *> GoodElectrons;
            typedef std::vector<const Muon *> GoodMuons;
            typedef std::vector<CorrectedJet> GoodJets;

            bool primaryVertices(const Event *);
            bool jets(const Event *);
            bool lepton();
            bool secondaryLeptonVeto();
            bool cut();
            bool leadingJet();
            bool htlep(const Event *);

            void selectGoodElectrons(const Event *);
            void selectGoodMuons(const Event *);

            bool cut2D(const LorentzVector *);
            bool isolation(const LorentzVector *, const PFIsolation *);

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

    // Helpers
    //
    std::ostream &operator <<(std::ostream &,
            const SynchSelectorDelegate::LeptonMode &);
    std::ostream &operator <<(std::ostream &,
            const SynchSelectorDelegate::CutMode &);
}

#endif
