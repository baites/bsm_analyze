// Dump resonance information
//
// Created by Samvel Khalatyan, Feb 17, 2012
// Copyright 2012, All rights reserved

#ifndef BSM_RESONANCE_DUMP_ANALYZER
#define BSM_RESONANCE_DUMP_ANALYZER

#include <sstream>
#include <string>

#include <boost/shared_ptr.hpp>

#include "bsm_stat/interface/bsm_stat_fwd.h"
#include "interface/bsm_fwd.h"

#include "interface/Analyzer.h"
#include "interface/EventDump.h"
#include "interface/HadronicTopAnalyzer.h"
#include "interface/Monitor.h"

namespace bsm
{
    class ResonanceDumpDelegate: public HadronicTopDelegate
    {
        public:
            virtual void setEvents(const uint32_t &max_events)
            {
            }
    };

    class ResonanceDumpOptions: public HadronicTopOptions
    {
        public:
            ResonanceDumpOptions();

        private:
            void setEvents(const uint32_t &);
    };

    class ResonanceDumpAnalyzer:
        public Analyzer,
        public ResonanceDumpDelegate,
        public EventDumpDelegate
    {
        public:
            ResonanceDumpAnalyzer();
            ResonanceDumpAnalyzer(const ResonanceDumpAnalyzer &);

            JetEnergyCorrectionDelegate *getJetEnergyCorrectionDelegate() const;
            SynchSelectorDelegate *getSynchSelectorDelegate() const;
            TriggerDelegate *getTriggerDelegate() const;

            // Hadronic Top delegate interface
            //
            virtual void setHtopNjets(const int &min, const int &max);
            virtual void setEvents(const uint32_t &max);

            // Event dump delegate interface
            //
            virtual void setFormatLevel(const Level &);

            // Anlayzer interface
            //
            virtual void onFileOpen(const std::string&, const bsm::Input*) {}
            virtual void process(const Event *);

            // Object interface
            //
            virtual uint32_t id() const;
            virtual ObjectPtr clone() const;
            void merge(const ObjectPtr &);
            virtual void print(std::ostream &) const;

        private:
            typedef ResonanceReconstructor::Mttbar Mttbar;

            Mttbar mttbar() const;

            boost::shared_ptr<SynchSelector> _synch_selector;
            boost::shared_ptr<ResonanceReconstructor> _reconstructor;

            struct {
                uint32_t min;
                uint32_t max;
            } _htop_njets;

            uint32_t _max_events;
            uint32_t _dumped_events;

            std::ostringstream _log;

            Level _format_level;

            boost::shared_ptr<Format> _format;
    };
}

#endif
