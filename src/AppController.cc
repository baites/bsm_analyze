// Application Controller
//
// Apply user-specified analyzer to given input file(s) in single- or
// multi-threads mode.
//
// Created by Samvel Khalatyan, Jul 31, 2011
// Copyright 2011, All rights reserved

#include <fstream>
#include <iomanip>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include <TFile.h>

#include "bsm_core/interface/Debug.h"
#include "bsm_input/interface/Reader.h"
#include "bsm_input/interface/Event.pb.h"
#include "interface/Analyzer.h"
#include "interface/AppController.h"
#include "interface/Thread.h"

using namespace std;

using bsm::AppController;

namespace fs = boost::filesystem;

using boost::regex;

AppController::AppController():
    _run_mode(SINGLE_THREAD),
    _disable_multithread(false),
    _number_of_threads(0),
    _interactive(false)
{
    // Generic Options: common to all executables
    //
    _generic_options.reset(new po::options_description("Generic Options"));
    _generic_options->add_options()
        ("help,h",
         "Help message")

        ("multi-thread",
         po::value<uint32_t>()->implicit_value(0)->notifier(
             boost::bind(&AppController::setNumberOfThreads, this, _1)),
         "Run Analysis with multi-threads: 0 - auto, otherwise max number of threads")

        ("debug",
         po::value<string>()->implicit_value("debug.log")->notifier(
             boost::bind(&AppController::setDebugFile, this, _1)),
         "save debug info in file")

        ("interactive",
         po::value<bool>()->implicit_value(false)->notifier(
             boost::bind(&AppController::setInteractive, this, _1)),
         "run in interactive mode")

        ("output",
         po::value<string>()->notifier(
             boost::bind(&AppController::setOutput, this, _1)),
         "save output plots in file")
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

    // Suppress log
    //
    _debug.reset(new core::Debug());
}

AppController::~AppController()
{
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

        if (regex_search(*input, regex("\\.txt$")))
        {
            ifstream in(input->c_str());
            if (!in)
            {
                cerr << "failed to read input TXT file: " << *input << endl;

                continue;
            }

            Inputs files;
            for(string file; in >> file; )
                files.push_back(file);

            addInputs(files);

            continue;
        }

        _input_files.push_back(*input);
    }
}

bool AppController::isInteractive() const
{
    return _interactive;
}

AppController::TFilePtr AppController::output() const
{
    return _output;
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
    DescriptionPtr visible_options(new po::options_description());
    visible_options->add(*_generic_options);
    
    for(vector<DescriptionPtr>::const_iterator options = _custom_options.begin();
            _custom_options.end() != options;
            ++options)
    {
        visible_options->add(options->operator *());
    }

    DescriptionPtr cmdline_options(new po::options_description());
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

    // Initialize debug before any other options are passed: this is done in
    // order to ensure all later clog, cerr, cout prints to be logged in
    // debug file
    //
    if (arguments->count("debug"))
        setDebugFile((*arguments)["debug"].as<string>());

    po::notify(*arguments);

    if (arguments->count("help")
            || _input_files.empty())
    {
        cout << *visible_options << endl;

        return false;
    }
    else
    {
        clog << _input_files.size() << " input files" << endl;
        for(Inputs::const_iterator input = _input_files.begin();
                _input_files.end() != input;
                ++input)
        {
            clog << " [+] " << *input << endl;
        }
        clog << endl;

        if (SINGLE_THREAD == _run_mode
                || (MULTI_THREAD == _run_mode
                    && (1 == _number_of_threads
                        || 1 == _input_files.size())))
            processSingleThread();
        else
            processMultiThread();

        cout << *_analyzer << endl;

        if (!_output_filename.empty())
        {
            _output.reset(new TFile(_output_filename.c_str(), "RECREATE"));
            if (!_output->IsOpen())
                _output.reset();
        }
    }

    return true;
}

void AppController::disableMutlithread()
{
    _disable_multithread = true;
}

// Privates
//
void AppController::setDebugFile(const string &filename)
{
    if (!_debug->isInitialized())
        _debug->init(filename);
}

void AppController::setNumberOfThreads(const uint32_t &number_of_threads)
{
    if (_disable_multithread)
        return;

    _number_of_threads = number_of_threads;

    _run_mode = MULTI_THREAD;
}

void AppController::setInteractive(const bool &value)
{
    _interactive = value;
}

void AppController::setOutput(const string &filename)
{
    _output_filename = filename;
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
        else
            _analyzer->onFileOpen(reader->filename(), reader->input().get());

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
    boost::shared_ptr<ThreadController>
        controller(new ThreadController(_number_of_threads));

    for(Inputs::const_iterator input = _input_files.begin();
            _input_files.end() != input;
            ++input)
    {
        controller->push(*input);
    }

    controller->use(_analyzer);
    controller->start();
}
