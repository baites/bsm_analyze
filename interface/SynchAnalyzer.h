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
#include "bsm_input/interface/bsm_input_fwd.h"
#include "JetMETObjects/interface/JetCorrectorParameters.h"
#include "interface/Analyzer.h"
#include "interface/AppController.h"
#include "interface/Cut.h"
#include "interface/EventDump.h"
#include "interface/bsm_fwd.h"
#include "interface/SynchSelector.h"

class FactorizedJetCorrector;

namespace bsm
{
    enum SynchMode
    {
        ELECTRON = 0,
        MUON
    };

    enum SynchCut
    {
        ISOLATION = 0,
        CUT_2D
    };

    class SynchAnalyzerDelegate
    {
        public:
            virtual ~SynchAnalyzerDelegate()
            {
            }

            virtual void setSelection(const SynchSelector::Selection &)
            {
            }
    };

    class SynchAnalyzerOptions : public Options
    {
        public:
            SynchAnalyzerOptions();
            virtual ~SynchAnalyzerOptions();

            void setDelegate(SynchAnalyzerDelegate *);
            SynchAnalyzerDelegate *delegate() const;

            // Options interface
            //
            virtual DescriptionPtr description() const;

        private:
            void setSelection(std::string);

            SynchAnalyzerDelegate *_delegate;
            DescriptionPtr _description;
    };

    class SynchAnalyzer : public Analyzer,
        public SynchAnalyzerDelegate,
        public CounterDelegate,
        public EventDumpDelegate
    {
        public:
            SynchAnalyzer();
            SynchAnalyzer(const SynchAnalyzer &);

            virtual ~SynchAnalyzer();

            JetEnergyCorrectionDelegate *getJetEnergyCorrectionDelegate() const;
            SynchSelectorDelegate *getSynchSelectorDelegate() const;
            Cut2DSelectorDelegate *getCut2DSelectorDelegate() const;

            // Anlayzer interface
            //
            virtual void onFileOpen(const std::string &filename, const Input *);
            virtual void process(const Event *);

            // Synch Analyzer Delegate interface
            //
            virtual void setSelection(const SynchSelector::Selection &);

            // Counter Delegate interface
            //
            virtual void didCounterAdd();

            // Event Dump Delegate interface
            //
            virtual void setEventNumber(const Event::Extra &);

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
            virtual void merge(const ObjectPtr &);

            virtual void print(std::ostream &) const;

        private:
            void dump(const Event *);

            boost::shared_ptr<SynchSelector> _synch_selector;

            SynchSelector::Selection _selection;

            boost::shared_ptr<Format> _format;
            boost::shared_ptr<Format> _dump_format;
            std::ostringstream _out;

            const Event *_event;

            std::vector<Event::Extra> _events_to_dump;
    };

    std::ostream &operator <<(std::ostream &, const SynchMode &);
}

#endif
