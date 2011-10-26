// Compare Offline and HLT PF Jets 
//
// Created by Samvel Khalatyan, Oct 12, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_JET_ANALYZER
#define BSM_JET_ANALYZER

#include <boost/shared_ptr.hpp>

#include "bsm_input/interface/bsm_input_fwd.h"
#include "interface/Analyzer.h"
#include "interface/bsm_fwd.h"

namespace bsm
{
    class JetAnalyzer : public Analyzer
    {
        public:
            typedef boost::shared_ptr<P4Monitor> P4MonitorPtr;

            JetAnalyzer();
            JetAnalyzer(const JetAnalyzer &);

            JetEnergyCorrectionDelegate *getJetEnergyCorrectionDelegate() const;
            SynchSelectorDelegate *getSynchSelectorDelegate() const;

            // Getters
            //
            const P4MonitorPtr hltLeadingJetMonitor() const;
            const P4MonitorPtr selectedHltLeadingJetMonitor() const;
            const P4MonitorPtr recoLeadingJetMonitor() const;

            // Analyzer interface
            //
            virtual void onFileOpen(const std::string &filename, const Input *);
            virtual void process(const Event *);

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;

            virtual void print(std::ostream &) const;

        private:
            boost::shared_ptr<SynchSelector> _synch_selector;
            boost::shared_ptr<P4Selector> _hlt_jet_selector;

            typedef uint64_t Hash;

            Hash _hlt_jet_producer;

            P4MonitorPtr _hlt_leading_jet;
            P4MonitorPtr _selected_hlt_leading_jet;
            P4MonitorPtr _reco_leading_jet;

    };
}

#endif
