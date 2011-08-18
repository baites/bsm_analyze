// Dumpt event content
//
// Created by Samvel Khalatyan, Aug 04, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_EVENT_DUMP
#define BSM_EVENT_DUMP

#include <sstream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "bsm_input/interface/bsm_input_fwd.h"
#include "bsm_input/interface/Event.pb.h"
#include "interface/Analyzer.h"
#include "interface/AppController.h"
#include "interface/bsm_fwd.h"

namespace bsm
{
    class EventDumpDelegate
    {
        public:
            enum Level
            {
                SHORT,
                MEDIUM,
                FULL
            };

            virtual ~EventDumpDelegate()
            {
            }

            // Event_Extra is the same as Event::Extra
            //
            virtual void setEventNumber(const Event_Extra &)
            {
            }

            virtual void setFormatLevel(const Level &)
            {
            }
    };

    class EventDumpOptions: public Options
    {
        public:
            EventDumpOptions();
            virtual ~EventDumpOptions();

            void setDelegate(EventDumpDelegate *);
            EventDumpDelegate *delegate() const;

            // Options interface
            //
            virtual DescriptionPtr description() const;

        private:
            typedef std::vector<std::string> Events;

            void setEvents(const Events &);
            void setFormatLevel(std::string);

            EventDumpDelegate *_delegate;
            DescriptionPtr _description;
    };

    class EventDumpAnalyzer : public Analyzer,
        public EventDumpDelegate
    {
        public:
            EventDumpAnalyzer();
            EventDumpAnalyzer(const EventDumpAnalyzer &);

            void addEvent(const uint32_t &id,
                    const uint32_t &lumi = 0,
                    const uint32_t &runi = 0);

            // Event Dump Delegate interface
            //
            virtual void setEventNumber(const Event_Extra &);
            virtual void setFormatLevel(const Level &);

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
            Level _format_level;

            boost::shared_ptr<Format> _format;

            std::vector<Event::Extra> _events;

            std::ostringstream _out;
    };
}

#endif
