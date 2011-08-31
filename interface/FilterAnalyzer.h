// Apply selectors and filter events
//
// Created by Samvel Khalatyan, May 20, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_FILTER_ANALYZER
#define BSM_FILTER_ANALYZER

#include <boost/shared_ptr.hpp>

#include "bsm_input/interface/bsm_input_fwd.h"
#include "bsm_input/interface/Reader.h"
#include "interface/Analyzer.h"
#include "interface/bsm_fwd.h"

namespace bsm
{
    class FilterAnalyzer : public Analyzer,
        public ReaderDelegate
    {
        public:
            FilterAnalyzer();
            FilterAnalyzer(const FilterAnalyzer &);

            JetEnergyCorrectionDelegate *getJetEnergyCorrectionDelegate() const;
            SynchSelectorDelegate *getSynchSelectorDelegate() const;

            // Analyzer interface
            //
            virtual void onFileOpen(const std::string &filename, const Input *);
            virtual void process(const Event *);

            // Reader Delegate interface
            //
            virtual void fileWillClose(const Reader *);

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;

            virtual void print(std::ostream &) const;

        private:
            boost::shared_ptr<Writer> _writer;
            boost::shared_ptr<SynchSelector> _synch_selector;
    };
}

#endif
