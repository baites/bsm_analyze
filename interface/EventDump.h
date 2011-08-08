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
            virtual ~EventDumpDelegate() {}

            // Event_Extra is the same as Event::Extra
            //
            virtual void setEventNumber(const Event_Extra &) {}
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
            void dump(const Event *);

            void dumpPrimaryVertices(const Event *);
            void dumpJets(const Event *);
            void dumpElectrons(const Event *);
            void dumpMuons(const Event *);

            std::vector<Event::Extra> _events;

            std::ostringstream _out;

            boost::shared_ptr<PrimaryVertexSelector> _primary_vertex_selector;
            boost::shared_ptr<JetSelector> _jet_selector;
            boost::shared_ptr<ElectronSelector> _electron_selector;
            boost::shared_ptr<MuonSelector> _muon_selector;
    };
}

#endif
