// Efficiency Plots
//
// Apply lepton removal to jets, Jet Energy Corrections, select events
// based on the corrected P4, leptons and then aaply 2D cut:
//      DeltaR > CUT
//      pT,rel > CUT
//
// between the muon/electron and closest jet
//
// Created by Samvel Khalatyan, Jul 19, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_EFFICIENCY_ANALYZER
#define BSM_EFFICIENCY_ANALYZER

#include <vector>

#include <boost/shared_ptr.hpp>

#include "JetMETObjects/interface/JetCorrectorParameters.h"
#include "interface/Analyzer.h"
#include "interface/bsm_fwd.h"

class FactorizedJetCorrector;

namespace bsm
{
    class EfficiencyAnalyzer : public Analyzer
    {
        public:
            typedef std::vector<JetCorrectorParameters> Corrections;

            enum LeptonMode
            {
                ELECTRON = 1,
                MUON = 2
            };

            EfficiencyAnalyzer(const LeptonMode & = ELECTRON);
            EfficiencyAnalyzer(const EfficiencyAnalyzer &);

            void setJetEnergyCorrections(const Corrections &corrections);

            // Anlayzer interface
            //
            virtual void onFileOpen(const std::string &filename, const Input *);
            virtual void process(const Event *);

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
            virtual void merge(const ObjectPtr &);

            virtual void print(std::ostream &) const;

        private:
            typedef boost::shared_ptr<MultiplicityCutflow> CutflowPtr;
            typedef std::vector<const Electron *> GoodElectrons;

            struct CorrectedJet
            {
                CorrectedJet()
                {
                    jet = 0;
                }

                const Jet *jet;
                LorentzVector corrected_p4;
            };

            enum
            {
                PRESELECTION = 0,
                SCRAPING,
                HBHENOISE,
                PRIMARY_VERTEX,
                JET,
                LEPTON,
                VETO_SECOND_LEPTON,
                HTLEP,
                ISOLATED_LEPTON,
                CUT_2D_LEPTON,
                
                SELECTIONS
            };

            typedef std::vector<CorrectedJet> GoodJets;
            typedef std::vector<const Muon *> GoodMuons;

            void copyCorrections(const Corrections &);

            GoodJets jets(const Event *,
                    const GoodElectrons &,
                    const GoodMuons &);

            LeptonMode _lepton_mode;
            boost::shared_ptr<MultiplicityCutflow> _cutflow;

            boost::shared_ptr<PrimaryVertexSelector> _primary_vertex_selector;
            boost::shared_ptr<JetSelector> _jet_selector;
            boost::shared_ptr<ElectronSelector> _electron_selector;
            boost::shared_ptr<MuonSelector> _muon_selector;

            Corrections _corrections;
            boost::shared_ptr<FactorizedJetCorrector> _jec;
    };

    std::ostream &operator <<(std::ostream &, const EfficiencyAnalyzer::LeptonMode &);
}

#endif
