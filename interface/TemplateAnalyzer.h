// Generate Template plots and comparison:
//
//  [MC]        hTlep, Mttbar in regions S1, S2, Signal
//  [Data]      hTlep, Mttbar in regions S1, S2
//  [MC/Data]   compare hTlep, Mttbar in S1 + S2
//
// Created by Samvel Khalatyan, Aug 29, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_TEMPLATE_ANALYZER
#define BSM_TEMPLATE_ANALYZER

#include <iosfwd>

#include "bsm_stat/interface/bsm_stat_fwd.h"
#include "bsm_input/interface/bsm_input_fwd.h"
#include "interface/Analyzer.h"
#include "interface/bsm_fwd.h"

namespace bsm
{
    class TemplateAnalyzer : public Analyzer, public CounterDelegate
    {
        public:
            typedef boost::shared_ptr<stat::H1> H1Ptr;
            typedef boost::shared_ptr<stat::H2> H2Ptr;

            TemplateAnalyzer();
            TemplateAnalyzer(const TemplateAnalyzer &);

            const H1Ptr htlep() const;
            const H1Ptr mttbar() const;
            const H2Ptr drVsPtrel() const;

            JetEnergyCorrectionDelegate *getJetEnergyCorrectionDelegate() const;
            SynchSelectorDelegate *getSynchSelectorDelegate() const;
            Cut2DSelectorDelegate *getCut2DSelectorDelegate() const;

            // Counter Delegate interface
            //
            virtual void didCounterAdd();

            // Anlayzer interface
            //
            virtual void onFileOpen(const std::string &filename, const Input *);
            virtual void process(const Event *);

            // Object interface
            //
            virtual uint32_t id() const;
            virtual ObjectPtr clone() const;
            virtual void print(std::ostream &) const;

        private:
            typedef boost::shared_ptr<H1Proxy> H1ProxyPtr;
            typedef boost::shared_ptr<H2Proxy> H2ProxyPtr;

            boost::shared_ptr<SynchSelector> _synch_selector;

            H1ProxyPtr _htlep;
            H1ProxyPtr _mttbar;
            H2ProxyPtr _dr_vs_ptrel;
    };
}

#endif
