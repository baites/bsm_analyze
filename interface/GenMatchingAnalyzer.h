// Apply SynchSelector, use events with matched reconstructed jets with
// gen partons.
//
// Created by Samvel Khalatyan, Mar 28, 2012
// Copyright 2011, All rights reserved

#ifndef BSM_GEN_MATCHING_ANALYZER
#define BSM_GEN_MATCHING_ANALYZER

#include <iosfwd>
#include <map>
#include <utility>

#include <boost/shared_ptr.hpp>

#include "bsm_stat/interface/bsm_stat_fwd.h"
#include "bsm_input/interface/bsm_input_fwd.h"
#include "interface/Algorithm.h"
#include "interface/Analyzer.h"
#include "interface/AppController.h"
#include "interface/Cut.h"
#include "interface/DecayGenerator.h"
#include "interface/SynchSelector.h"
#include "interface/bsm_fwd.h"

namespace bsm
{
    namespace gen
    {
        typedef std::vector<const CorrectedJet *> CorrectedJets;

        struct MatchedJet
        {
            MatchedJet()
            {
                parton = 0;
                jet = 0;
            }

            bool match(CorrectedJets &corrected_jets);

            const GenParticle *parton;
            const CorrectedJet *jet;
        };

        struct Wboson
        {
            Wboson()
            {
                lepton = 0;
                neutrino = 0;
            }

            enum Decay
            {
                UNKNOWN = 0,
                ELECTRON = 1,
                MUON = 2,
                TAU = 3,
                HADRONIC = 4
            };

            void fill(const GenParticle &);
            bool match(CorrectedJets &corrected_jets);

            Decay decay;

            const GenParticle *lepton;
            const GenParticle *neutrino;
            std::vector<MatchedJet> jets;
        };

        struct Top
        {
            void fill(const GenParticle &);
            bool match(CorrectedJets &corrected_jets);

            Wboson wboson;
            std::vector<MatchedJet> jets;
        };

        struct TTbar
        {
            void fill(const GenParticle &gen_top);
            bool match(CorrectedJets &corrected_jets);

            Top ltop;
            Top htop;
        };
    }

    class GenMatchingAnalyzer: public Analyzer,
                               public CounterDelegate
    {
        public:
            typedef boost::shared_ptr<stat::H1> H1Ptr;
            typedef boost::shared_ptr<P4Monitor> P4MonitorPtr;

            GenMatchingAnalyzer();
            GenMatchingAnalyzer(const GenMatchingAnalyzer &);

            const H1Ptr cutflow() const;

            const H1Ptr ltop_drsum() const;
            const H1Ptr htop_drsum() const;

            const H1Ptr htop_dphi() const;

            const P4MonitorPtr ltop() const;
            const P4MonitorPtr htop() const;

            const P4MonitorPtr ttbar() const;

            JetEnergyCorrectionDelegate *getJetEnergyCorrectionDelegate() const;
            SynchSelectorDelegate *getSynchSelectorDelegate() const;
            TriggerDelegate *getTriggerDelegate() const;
            BtagDelegate *getBtagDelegate() const;

            // Counter Delegate interface
            //
            virtual void didCounterAdd(const Counter *);

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
            typedef boost::shared_ptr<H1Proxy> H1ProxyPtr;

            gen::TTbar gen_decay(const Event *) const;

            boost::shared_ptr<SynchSelector> _synch_selector;

            // map: counter pointer to SynchSelector selection
            //
            std::map<const Counter *, uint32_t> _counters;
            H1ProxyPtr _cutflow;

            H1ProxyPtr _ltop_drsum;
            H1ProxyPtr _htop_drsum;

            H1ProxyPtr _htop_dphi;

            P4MonitorPtr _ltop;
            P4MonitorPtr _htop;
            P4MonitorPtr _ttbar;
    };
}

#endif
