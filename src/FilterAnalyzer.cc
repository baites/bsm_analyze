// Apply selectors and filter events
//
// Created by Samvel Khalatyan, May 20, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <ostream>
#include <sstream>

#include <boost/functional/hash.hpp>
#include <boost/filesystem.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/version.hpp>

#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Input.pb.h"
#include "bsm_input/interface/Trigger.pb.h"
#include "bsm_input/interface/Writer.h"
#include "interface/FilterAnalyzer.h"
#include "interface/SynchSelector.h"

using namespace std;

using boost::dynamic_pointer_cast;
namespace fs = boost::filesystem;

using bsm::FilterAnalyzer;

FilterAnalyzer::FilterAnalyzer()
{
    _synch_selector.reset(new SynchSelector());
    _synch_selector->htlep()->disable();
    monitor(_synch_selector);
}

FilterAnalyzer::FilterAnalyzer(const FilterAnalyzer &object)
{
    _synch_selector = 
        dynamic_pointer_cast<SynchSelector>(object._synch_selector->clone());
    monitor(_synch_selector);
}

bsm::JetEnergyCorrectionDelegate *FilterAnalyzer::getJetEnergyCorrectionDelegate() const
{
    return _synch_selector->getJetEnergyCorrectionDelegate();
}

bsm::SynchSelectorDelegate *FilterAnalyzer::getSynchSelectorDelegate() const
{
    return _synch_selector.get();
}

void FilterAnalyzer::onFileOpen(const string &filename, const Input *input)
{
    if (_writer)
    {
        _writer->close();
        _writer.reset();
    }

    fs::path path(filename);

    ostringstream file_name;

    boost::hash<string> make_hash;

    file_name << make_hash(filename);

#if BOOST_VERSION < 104600
    file_name << path.extension();
#else
    file_name << path.extension();
#endif

    _writer.reset(new Writer(file_name.str()));
    _writer->open();
    if (!_writer->isOpen())
    {
        _writer.reset();

        return;
    }

    _writer->input()->CopyFrom(*input);
    _writer->input()->set_events(0);
}

void FilterAnalyzer::process(const Event *event)
{
    if (!_writer)
        return;

    if (!_synch_selector->apply(event))
        return;

    _writer->write(event);
}

void FilterAnalyzer::fileWillClose(const Reader *)
{
    cout << "close file" << endl;
    _writer->close();
}

uint32_t FilterAnalyzer::id() const
{
    return core::ID<FilterAnalyzer>::get();
}

FilterAnalyzer::ObjectPtr FilterAnalyzer::clone() const
{
    return ObjectPtr(new FilterAnalyzer(*this));
}

void FilterAnalyzer::print(ostream &out) const
{
    out << *_synch_selector;
}
