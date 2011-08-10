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

#include "interface/Analyzer.h"
#include "interface/bsm_fwd.h"

class FactorizedJetCorrector;

namespace bsm
{
    class EfficiencyAnalyzer : public Analyzer
    {
        public:
            EfficiencyAnalyzer();
            EfficiencyAnalyzer(const EfficiencyAnalyzer &);

            JetEnergyCorrectionDelegate *getJetEnergyCorrectionDelegate() const;
            SynchSelectorDelegate *getSynchSelectorDelegate() const;

            // Anlayzer interface
            //
            virtual void onFileOpen(const std::string &filename, const Input *);
            virtual void process(const Event *);

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
            using Object::merge;

            virtual void print(std::ostream &) const;

        private:
            boost::shared_ptr<SynchSelector> _synch_selector;
    };
}

#endif
