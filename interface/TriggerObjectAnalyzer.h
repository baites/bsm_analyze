// Extract Trigger associated objects
//
// Created by Samvel Khalatyan, Oct 07, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_TRIGGER_OBJECT
#define BSM_TRIGGER_OBJECT

#include <stdint.h>
#include <map>
#include <string>
#include <sstream>

#include "interface/Analyzer.h"
#include "interface/TriggerAnalyzer.h"
#include "interface/bsm_fwd.h"

namespace bsm
{
    class TriggerObjectAnalyzer: public Analyzer,
        public TriggerDelegate
    {
        public:
            TriggerObjectAnalyzer();
            TriggerObjectAnalyzer(const TriggerObjectAnalyzer &);

            // Trigger Delegate interface
            //
            virtual void setTrigger(const Trigger &);
            virtual void setFilter(const Hash &);

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
            typedef std::map<Hash, std::string> HashTable;

            HashTable _trigger_map;
            HashTable _filter_map;

            std::ostringstream _out;

            Trigger _trigger;
            Hash _filter;
    };
}

#endif
