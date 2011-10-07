// Trigger Analyzer
//
// Apply trigger to events
//
// Created by Samvel Khalatyan, May 26, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_TRIGGER_ANALYZER
#define BSM_TRIGGER_ANALYZER

#include <stdint.h>

#include <iosfwd>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "bsm_input/interface/Trigger.pb.h"
#include "interface/bsm_fwd.h"
#include "interface/Analyzer.h"
#include "interface/AppController.h"

namespace bsm
{
    class TriggerDelegate
    {
        public:
            typedef uint64_t Hash;

            virtual ~TriggerDelegate() {}

            virtual void setTrigger(const Trigger &) {}
            virtual void setFilter(const Hash &) {}
            virtual void setProducer(const Hash &) {}
    };

    class TriggerOptions : public Options
    {
        public:
            TriggerOptions();

            void setDelegate(TriggerDelegate *);
            TriggerDelegate *delegate() const;

            // Options interface
            //
            virtual DescriptionPtr description() const;

        private:
            void setTrigger(std::string) const;
            void setFilter(std::string) const;
            void setProducer(std::string) const;

            TriggerDelegate *_delegate;

            DescriptionPtr _description;
    };

    class TriggerAnalyzer : public Analyzer
    {
        public:
            TriggerAnalyzer();

            // Analyzer interface
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
            // Prevent copying
            //
            TriggerAnalyzer &operator =(const TriggerAnalyzer &);

            typedef std::map<std::size_t, std::string> HLTMap;

            typedef std::map<Trigger, uint32_t> HLTCutflow;

            HLTMap _hlt_map;
            HLTCutflow _hlt_cutflow;
    };

    // Helpers
    //
    bool operator <(const Trigger &, const Trigger &);

    std::ostream &operator <<(std::ostream &, const Trigger &);
}

#endif
