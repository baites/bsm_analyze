// Extract Trigger filters and associated trigger objects
//
// Created by Samvel Khalatyan, Oct 07, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_TRIGGER_FILTER
#define BSM_TRIGGER_FILTER

#include <stdint.h>
#include <map>
#include <string>
#include <sstream>

#include "interface/Analyzer.h"
#include "interface/bsm_fwd.h"

namespace bsm
{
    class TriggerFilterAnalyzer: public Analyzer
    {
        public:
            TriggerFilterAnalyzer();
            TriggerFilterAnalyzer(const TriggerFilterAnalyzer &);

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
            typedef uint32_t Hash;
            typedef std::map<Hash, std::string> HashTable;

            HashTable _filter_map;

            std::ostringstream _out;
    };
}

#endif
