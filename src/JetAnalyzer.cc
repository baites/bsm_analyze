// Compare Offline and HLT PF Jets 
//
// Created by Samvel Khalatyan, Oct 12, 2011
// Copyright 2011, All rights reserved

#include <cfloat>
#include <vector>

#include <boost/functional/hash.hpp>
#include <boost/pointer_cast.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Electron.pb.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Jet.pb.h"
#include "bsm_input/interface/Muon.pb.h"
#include "bsm_stat/interface/Axis.h"
#include "bsm_stat/interface/H1.h"
#include "interface/Algorithm.h"
#include "interface/Cut.h"
#include "interface/DecayGenerator.h"
#include "interface/Monitor.h"
#include "interface/Selector.h"
#include "interface/SynchSelector.h"
#include "interface/Utility.h"
#include "interface/JetAnalyzer.h"

using namespace std;
using namespace boost;
using namespace bsm;

struct PtLess
{
    public:
        typedef shared_ptr<LorentzVector> P4Ptr;

        bool operator()(const P4Ptr &v1, const P4Ptr &v2)
        {
            return pt_less(*v1, *v2);
        }
};

struct PtGreater
{
    public:
        typedef shared_ptr<LorentzVector> P4Ptr;

        bool operator()(const P4Ptr &v1, const P4Ptr &v2)
        {
            return pt_greater(*v1, *v2);
        }
};

// Jet Analyzer
//
JetAnalyzer::JetAnalyzer()
{
    boost::hash<string> make_hash;
    _hlt_jet_producer = make_hash("hltantikt5convpfjets::hlt");

    _synch_selector.reset(new SynchSelector());
    _synch_selector->htlep()->disable();
    _synch_selector->leadingJet()->disable();
    _synch_selector->cut()->disable();
    monitor(_synch_selector);

    _hlt_jet_selector.reset(new P4Selector());
    monitor(_hlt_jet_selector);

    _hlt_leading_jet.reset(new P4Monitor());
    _selected_hlt_leading_jet.reset(new P4Monitor());
    _reco_leading_jet.reset(new P4Monitor());

    //_hlt_leading_jet->pt()->mutable_axis()->init(100, 0, 1000);
    //_selected_hlt_leading_jet->pt()->mutable_axis()->init(100, 0, 1000);
    //_reco_leading_jet->pt()->mutable_axis()->init(100, 0, 1000);

    monitor(_hlt_leading_jet);
    monitor(_selected_hlt_leading_jet);
    monitor(_reco_leading_jet);
}

JetAnalyzer::JetAnalyzer(const JetAnalyzer &object):
    _hlt_jet_producer(object._hlt_jet_producer)
{
    _synch_selector = 
        dynamic_pointer_cast<SynchSelector>(object._synch_selector->clone());
    monitor(_synch_selector);

    _hlt_jet_selector = 
        dynamic_pointer_cast<P4Selector>(object._hlt_jet_selector->clone());
    monitor(_hlt_jet_selector);

    _hlt_leading_jet =
        dynamic_pointer_cast<P4Monitor>(object._hlt_leading_jet->clone());

    _selected_hlt_leading_jet =
        dynamic_pointer_cast<P4Monitor>(object._selected_hlt_leading_jet->clone());

    _reco_leading_jet =
        dynamic_pointer_cast<P4Monitor>(object._reco_leading_jet->clone());

    monitor(_hlt_leading_jet);
    monitor(_selected_hlt_leading_jet);
    monitor(_reco_leading_jet);
}

bsm::JetEnergyCorrectionDelegate *JetAnalyzer::getJetEnergyCorrectionDelegate() const
{
    return _synch_selector->getJetEnergyCorrectionDelegate();
}

bsm::SynchSelectorDelegate *JetAnalyzer::getSynchSelectorDelegate() const
{
    return _synch_selector.get();
}

const JetAnalyzer::P4MonitorPtr JetAnalyzer::hltLeadingJetMonitor() const
{
    return _hlt_leading_jet;
}

const JetAnalyzer::P4MonitorPtr JetAnalyzer::selectedHltLeadingJetMonitor() const
{
    return _selected_hlt_leading_jet;
}

const JetAnalyzer::P4MonitorPtr JetAnalyzer::recoLeadingJetMonitor() const
{
    return _reco_leading_jet;
}

void JetAnalyzer::onFileOpen(const std::string &filename, const Input *input)
{
}

void JetAnalyzer::process(const Event *event)
{
    // Test if HLT PF Jets are available in the event
    //
    if (!event->has_hlt())
        return;

    typedef ::google::protobuf::RepeatedPtrField<TriggerProducer> Producers;
    const Producers &producers = event->hlt().producer();
    Producers::const_iterator hlt_jet_producer = producers.end();
    for(Producers::const_iterator producer = producers.begin();
           producers.end() != producer;
           ++producer)
    {
        if (_hlt_jet_producer == producer->hash())
        {
            hlt_jet_producer = producer;
            break;
        }
    }

    if (producers.end() == hlt_jet_producer)
        return;

    // Process only events, that pass the synch selector
    //
    if (_synch_selector->apply(event))
    {
        // process HLT jets made by producer
        //

        typedef ::google::protobuf::RepeatedPtrField<TriggerObject> Objects;
        const Objects &objects = event->hlt().object();

        vector<LorentzVector> hlt_p4;
        vector<LorentzVector> selected_hlt_p4;
        LockSelectorEventCounterOnUpdate lock_hlt_jets(*_hlt_jet_selector);

        Objects::const_iterator from = objects.begin();
        advance(from, hlt_jet_producer->from());

        Objects::const_iterator to = objects.begin();
        advance(to, hlt_jet_producer->to());
        for(; to > from; ++from)
        {
            if (!from->has_p4())
                continue;

            hlt_p4.push_back(from->p4());

            if (_hlt_jet_selector->apply(from->p4()))
                selected_hlt_p4.push_back(from->p4());
        }

        if (!hlt_p4.empty())
        {
            sort(hlt_p4.begin(), hlt_p4.end(), pt_greater);
            //sort(hlt_p4.begin(), hlt_p4.end(), pt_less);

            _hlt_leading_jet->fill(hlt_p4[0]);
        }

        if (!selected_hlt_p4.empty())
        {
            sort(selected_hlt_p4.begin(), selected_hlt_p4.end(), pt_greater);
            //sort(selected_hlt_p4.begin(), selected_hlt_p4.end(), pt_less);

            _selected_hlt_leading_jet->fill(selected_hlt_p4[0]);
        }



        // Sort corrected jet p4's by pt
        //
        typedef SynchSelector::GoodJets GoodJets;
        typedef SynchSelector::LorentzVectorPtr LorentzVectorPtr;

        vector<LorentzVectorPtr> corrected_p4;
        for(GoodJets::const_iterator good_jet = _synch_selector->goodJets().begin();
                _synch_selector->goodJets().end() != good_jet;
                ++good_jet)
        {
            corrected_p4.push_back(good_jet->corrected_p4);
        }

        sort(corrected_p4.begin(), corrected_p4.end(), PtGreater());
        //sort(corrected_p4.begin(), corrected_p4.end(), PtLess());

        // Monitor leading corrected jet
        //
        _reco_leading_jet->fill(*corrected_p4[0]);
    }
}

uint32_t JetAnalyzer::id() const
{
    return core::ID<JetAnalyzer>::get();
}

JetAnalyzer::ObjectPtr JetAnalyzer::clone() const
{
    return ObjectPtr(new JetAnalyzer(*this));
}

void JetAnalyzer::print(std::ostream &out) const
{
    out << "Jet Analyzer" << endl;
    out << *_synch_selector;
}
