// Synchronization Exercises
//
// Different Analyzers that performan the sinchronization exercises at
// different development stages
//
// Created by Samvel Khalatyan, Jul 05, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_SYNCHRONIZATION_ANALYZER
#define BSM_SYNCHRONIZATION_ANALYZER

#include <vector>
#include <sstream>

#include <boost/shared_ptr.hpp>

#include "bsm_input/interface/Event.pb.h"
#include "JetMETObjects/interface/JetCorrectorParameters.h"
#include "interface/Analyzer.h"
#include "interface/bsm_fwd.h"

class FactorizedJetCorrector;

namespace bsm
{
    enum SynchMode
    {
        ELECTRON = 1,
        MUON
    };

    class SynchJuly2011Analyzer : public Analyzer
    {
        public:
            typedef boost::shared_ptr<LorentzVectorMonitor> P4MonitorPtr;

            SynchJuly2011Analyzer(const SynchMode & = ELECTRON);
            SynchJuly2011Analyzer(const SynchJuly2011Analyzer &);

            const P4MonitorPtr leadingJet() const;

            const P4MonitorPtr electronBeforeVeto() const;
            const P4MonitorPtr muonToVeto() const;
            const P4MonitorPtr electronAfterVeto() const;

            const P4MonitorPtr muonBeforeVeto() const;
            const P4MonitorPtr electronToVeto() const;
            const P4MonitorPtr muonAfterVeto() const;

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

            enum
            {
                PRESELECTION = 0,
                PRIMARY_VERTEX = 1,
                JET = 2,
                LEPTON = 3,
                VETO_SECOND_LEPTON = 4
            };

            bool jets(const Event *);
            bool electron(const Event *);
            bool muon(const Event *);

            SynchMode _synch_mode;

            CutflowPtr _cutflow;

            boost::shared_ptr<PrimaryVertexSelector> _primary_vertex_selector;
            boost::shared_ptr<JetSelector> _jet_selector;
            boost::shared_ptr<ElectronSelector> _electron_selector;
            boost::shared_ptr<ElectronSelector> _electron_veto_selector;
            boost::shared_ptr<MuonSelector> _muon_selector;
            boost::shared_ptr<MuonSelector> _muon_veto_selector;

            std::vector<Event::Extra> _passed_events;

            P4MonitorPtr _leading_jet;

            P4MonitorPtr _electron_before_veto;
            P4MonitorPtr _muon_to_veto;
            P4MonitorPtr _electron_after_veto;

            P4MonitorPtr _muon_before_veto;
            P4MonitorPtr _electron_to_veto;
            P4MonitorPtr _muon_after_veto;
    };

    class SynchJECJuly2011Analyzer : public Analyzer
    {
        public:
            typedef std::vector<JetCorrectorParameters> Corrections;

            SynchJECJuly2011Analyzer(const SynchMode & = ELECTRON);
            SynchJECJuly2011Analyzer(const SynchJECJuly2011Analyzer &);

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

            typedef std::vector<CorrectedJet> GoodJets;
            typedef std::vector<const Muon *> GoodMuons;

            enum
            {
                PRESELECTION = 0,
                SCRAPING,
                HBHENOISE,
                PRIMARY_VERTEX,
                JET,
                LEPTON,
                VETO_SECOND_LEPTON
            };

            void copyCorrections(const Corrections &);

            GoodJets jets(const Event *,
                    const GoodElectrons &,
                    const GoodMuons &);

            void printP4(std::ostream &, const LorentzVector &);

            SynchMode _synch_mode;

            CutflowPtr _cutflow;

            boost::shared_ptr<PrimaryVertexSelector> _primary_vertex_selector;
            boost::shared_ptr<JetSelector> _jet_selector;
            boost::shared_ptr<ElectronSelector> _electron_selector;
            boost::shared_ptr<MuonSelector> _muon_selector;

            std::vector<Event::Extra> _passed_events;

            Corrections _corrections;
            boost::shared_ptr<FactorizedJetCorrector> _jec;

            std::ostringstream _out;
    };

    std::ostream &operator <<(std::ostream &, const SynchMode &);
}

#endif
