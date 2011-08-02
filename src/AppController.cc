// Application Controller
//
// Apply user-specified analyzer to given input file(s) in single- or
// multi-threads mode.
//
// Created by Samvel Khalatyan, Jul 31, 2011
// Copyright 2011, All rights reserved

#include <iomanip>
#include <iostream>

#include "boost/bind.hpp"
#include <boost/filesystem.hpp>

#include "bsm_input/interface/Reader.h"
#include "bsm_input/interface/Event.pb.h"
#include "interface/Analyzer.h"
#include "interface/AppController.h"
#include "interface/Thread.h"

using namespace std;

using bsm::AppController;

namespace fs = boost::filesystem;

AppController::AppController():
    _run_mode(SINGLE_THREAD)
{
    // Generic Options: common to all executables
    //
    _generic_options.reset(new po::options_description("Generic Options"));
    _generic_options->add_options()
        ("help,h",
         "Help message")

        ("multi-thread",
         po::value<bool>()->implicit_value(true)->notifier(
             boost::bind(&AppController::setRunMode, this, _1)),
         "Run Analysis with multi-threads")
    ;

    // Hidden options: necessary for the positional arguments
    //
    _hidden_options.reset(new po::options_description("Hidden Options"));
    _hidden_options->add_options()
        ("input",
         po::value<Inputs>()->notifier(
             boost::bind(&AppController::addInputs, this, _1)),
         "input file(s)")
    ;
}

void AppController::setAnalyzer(const AnalyzerPtr &analyzer)
{
    _analyzer = analyzer;
}

void AppController::addOptions(const Options &options)
{
    // Add options only in case the pointer is valid
    //
    if (options.description())
        _custom_options.push_back(options.description());
}

void AppController::addInputs(const Inputs &inputs)
{
    // Check file for existance and add it to the list of input files
    //
    for(Inputs::const_iterator input = inputs.begin();
            inputs.end() != input;
            ++input)
    {
        if (!fs::exists(*input))
        {
            cerr << "input does not exist: " << *input << endl;

            continue;
        }

        _input_files.push_back(*input);
    }
}

bool AppController::run(int &argc, char *argv[])
{
    if (!_analyzer)
    {
        cerr << "analyzer is not set" << endl;

        return false;
    }

    // Create group of visible options for help message
    //
    OptionsPtr visible_options(new po::options_description());
    visible_options->add(*_generic_options);
    
    for(vector<DescriptionPtr>::const_iterator options = _custom_options.begin();
            _custom_options.end() != options;
            ++options)
    {
        visible_options->add(options->operator *());
    }

    OptionsPtr cmdline_options(new po::options_description());
    cmdline_options->add(*visible_options).add(*_hidden_options);

    boost::shared_ptr<po::positional_options_description>
        positional_options(new po::positional_options_description());
    positional_options->add("input", -1);

    // Parse arguments
    //
    boost::shared_ptr<po::variables_map> arguments(new po::variables_map());
    po::store(po::command_line_parser(argc, argv).
            options(*cmdline_options).
            positional(*positional_options).
            run(),
            *arguments);
    po::notify(*arguments);

    if (arguments->count("help")
            || _input_files.empty())
        cout << *visible_options << endl;
    else
    {
        if (SINGLE_THREAD == _run_mode)
            processSingleThread();
        else
            processMultiThread();

        cout << *_analyzer << endl;
    }

    return true;
}

// Privates
//
void AppController::setRunMode(const bool &is_multi_thread)
{
    _run_mode = (is_multi_thread
            ? MULTI_THREAD
            : SINGLE_THREAD);
}

void AppController::processSingleThread()
{
    for(Inputs::const_iterator input = _input_files.begin();
            _input_files.end() != input;
            ++input)
    {
        boost::shared_ptr<Reader> reader(new Reader(*input));
        reader->open();
        
        if (!reader->isOpen())
        {
            cerr << "failed to open: " << *input << endl;

            continue;
        }

        for(boost::shared_ptr<Event> event(new Event());
                reader->read(event);
                event->Clear())
        {
            _analyzer->process(event.get());
        }
    }
}

void AppController::processMultiThread()
{
    boost::shared_ptr<ThreadController> controller(new ThreadController());
    for(Inputs::const_iterator input = _input_files.begin();
            _input_files.end() != input;
            ++input)
    {
        controller->push(*input);
    }

    controller->use(_analyzer);
    controller->start();
}
