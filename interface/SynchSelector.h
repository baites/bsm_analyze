// Synchronization exercise selector that is used to select "good"
// events with muons, electrons, jetc, etc.
//
// Created by Samvel Khalatyan, Aug 01, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_SYNCHRONIZATION_SELECTOR
#define BSM_SYNCHRONIZATION_SELECTOR

#include <string>
#include <vector>

#include "bsm_core/interface/Object.h"
#include "bsm_input/interface/bsm_input_fwd.h"
#include "bsm_input/interface/Physics.pb.h"
#include "interface/bsm_fwd.h"
#include "interface/JetEnergyCorrections.h"
#include "interface/AppController.h"
#include "interface/DelegateManager.h"
#include "interface/Selector.h"
#include "interface/CorrectedJet.h"
#include "interface/TriggerAnalyzer.h"
#include "interface/Cache.h"

#include "interface/RandomGenerator.h"

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
            virtual void setLeadingJetPt(const float &) {}
            virtual void setJetPt(const float &) {}
            virtual void setMaxBtag(const float &) {}
            virtual void setMinBtag(const float &) {}
            virtual void setToptag(const float &) {}
            virtual void setElectronPt(const float &) {}
            virtual void setQCDTemplate(const bool &) {}
            virtual void setWjetsTemplate(const bool &) {}

            virtual void setLtopPt(const float &) {}
            virtual void setChi2Discriminator(const float &) {}
    };

    class SynchSelectorOptions:
        public Options,
        public DelegateManager<SynchSelectorDelegate>
    {
        public:
            SynchSelectorOptions();

            // Options interface
            //
            virtual DescriptionPtr description() const;

        private:
            void setLeptonMode(std::string);
            void setCutMode(std::string);
            void setLeadingJetPt(const float &);
            void setJetPt(const float &);
            void setMaxBtag(const float &);
            void setMinBtag(const float &);
            void setToptag(const float &);
            void setElectronPt(const float &);
            void setQCDTemplate(const bool &);
            void setWjetsTemplate(const bool &);

            void setLtopPt(const float &);

            void setChi2Discriminator(const float &);

            DescriptionPtr _description;
    };

    class SynchSelector : public Selector,
        public SynchSelectorDelegate,
        public JetEnergyCorrectionDelegate,
        public TriggerDelegate
    {
        public:
            typedef boost::shared_ptr<Cut> CutPtr;
            typedef boost::shared_ptr<LorentzVector> LorentzVectorPtr;
            typedef boost::shared_ptr<MultiplicityCutflow> CutflowPtr;
            typedef boost::shared_ptr<RandomGenerator<> > RandomGeneratorPtr;

            typedef std::vector<const PrimaryVertex *> GoodPrimaryVertices;
            typedef std::vector<const Electron *> GoodElectrons;
            typedef std::vector<const Muon *> GoodMuons;
            typedef std::vector<CorrectedJet> GoodJets;
            typedef LorentzVectorPtr GoodMET;

            enum Selection
            {
                PRESELECTION = 0,
                TRIGGER,
                SCRAPING,
                HBHENOISE,
                PRIMARY_VERTEX,
                JET,
                LEPTON,
                VETO_SECOND_ELECTRON,
                VETO_SECOND_MUON,
                CUT_LEPTON,
                LEADING_JET,
                MAX_BTAG,
                MIN_BTAG,
                TOPTAG,
                HTLEP,
                TRICUT,
                MET,
                RECONSTRUCTION,
                LTOP,
                CHI2,

                SELECTIONS // this item should always be the last one
            };

            SynchSelector();
            SynchSelector(const SynchSelector &);

            virtual ~SynchSelector();

            // Access cuts
            //
            CutPtr cut() const;
            CutPtr leadingJet() const;
            CutPtr maxBtag() const;
            CutPtr minBtag() const;
            CutPtr toptag() const;
            CutPtr htlep() const;
            CutPtr tricut() const;
            CutPtr met() const;
            CutPtr reconstruction() const;
            CutPtr ltop() const;
            CutPtr chi2() const;

            // Test if muon passes the selector
            //
            virtual bool apply(const Event *);

            CutflowPtr cutflow() const;

            const GoodPrimaryVertices &goodPrimaryVertices() const;
            const GoodElectrons &goodElectrons() const;
            const GoodMuons &goodMuons() const;
            const GoodJets &niceJets() const;
            const GoodJets &goodJets() const;
            const GoodJets &caJets() const;
            const GoodJets &topJets() const;
            const GoodMET &goodMET() const;
            GoodJets::const_iterator closestJet() const;

            LeptonMode leptonMode() const;
            CutMode cutMode() const;
            bool qcdTemplate() const;
            bool wjetsTemplate() const;

            Cut2DSelectorDelegate *getCut2DSelectorDelegate() const;
            BtagDelegate *getBtagDelegate() const;

            bool reconstruction(const bool &value); // apply reconstruction cut
            bool ltop(const float &value); // apply ltop cut
            bool chi2(const float &value);

            uint32_t countBtaggedJets();

            // SynchSelectorDelegate interface
            //
            virtual void setLeptonMode(const LeptonMode &);
            virtual void setCutMode(const CutMode &);
            virtual void setLeadingJetPt(const float &);
            virtual void setJetPt(const float &);
            virtual void setMaxBtag(const float &);
            virtual void setMinBtag(const float &);
            virtual void setToptag(const float &);
            virtual void setElectronPt(const float &);
            virtual void setQCDTemplate(const bool &);
            virtual void setWjetsTemplate(const bool &);

            virtual void setLtopPt(const float &);

            virtual void setChi2Discriminator(const float &);

            // Jet Energy Correction Delegate interface
            //
            virtual void setCorrection(const Level &,
                    const std::string &file_name); // proxy to JEC setCorrection

            virtual void setSystematic(const Systematic &,
                    const std::string &filename);

            virtual void setChildCorrection();

            // Trigger Delegater interface
            //
            virtual void setTrigger(const Trigger &trigger);

            // Set the use of toptag by weight
            void useToptagWeight();

            // Return true if toptag by weight is used
            bool isToptagWeight() const;

            // Compute toptag weight and update jet mass
            float toptagWeight();

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

        public:

            bool triangularCut(const Event *);
            bool missingEnergy(const Event *);
            
        private:
            bool triggers(const Event *);
            bool primaryVertices(const Event *);
            bool jets(const Event *);
            bool lepton();
            bool secondElectronVeto();
            bool secondMuonVeto();
            bool isolationAnd2DCut();
            bool leadingJetCut();
            bool maxBtags();
            bool minBtags();
            bool toptagCut();
            bool htlepCut(const Event *);

            void selectGoodPrimaryVertices(const Event *);
            void selectGoodElectrons(const Event *);
            void selectGoodMuons(const Event *);

            bool cut2D(const LorentzVector *);
            bool isolation(const LorentzVector *, const PFIsolation *);

            void invalidate_cache();

            LeptonMode _lepton_mode;
            CutMode _cut_mode;

            CutflowPtr _cutflow;

            boost::shared_ptr<PrimaryVertexSelector> _primary_vertex_selector;
            boost::shared_ptr<ElectronSelector> _electron_selector;
            boost::shared_ptr<MuonSelector> _muon_selector;
            boost::shared_ptr<JetSelector> _nice_jet_selector;
            boost::shared_ptr<JetSelector> _good_jet_selector;
            boost::shared_ptr<Cut2DSelector> _cut2d_selector;

            boost::shared_ptr<JetEnergyCorrections> _jec;

            GoodPrimaryVertices _good_primary_vertices;
            GoodElectrons _good_electrons;
            GoodMuons _good_muons;
            GoodJets _nice_jets; // pT > 25
            GoodJets _good_jets; // pT > 50
            GoodJets _ca_jets;
            GoodJets _top_jets; // to tagged jets
            GoodJets::const_iterator _closest_jet;
            GoodMET _good_met;

            // cuts
            //
            CutPtr _cut;
            CutPtr _leading_jet;
            CutPtr _max_btag;
            CutPtr _min_btag;
            CutPtr _toptag;
            CutPtr _htlep;
            CutPtr _tricut;
            CutPtr _met;
            CutPtr _reconstruction;
            CutPtr _ltop;
            CutPtr _chi2;

            bool _qcd_template;
            bool _wjets_template;

            typedef std::vector<uint64_t> Triggers;
            Triggers _triggers; // hashes of triggers to be passed
            
            bool _weighted_toptag;

            RandomGeneratorPtr _random_generator; 

            boost::shared_ptr<Btag> _btag;

            // cache
            //
            Cache<uint32_t> _btagged_jets;
    };

    // Helpers
    //
    std::ostream &operator <<(std::ostream &,
            const SynchSelectorDelegate::LeptonMode &);
    std::ostream &operator <<(std::ostream &,
            const SynchSelectorDelegate::CutMode &);
}

#endif
