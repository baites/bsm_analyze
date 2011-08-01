// Application Controller
//
// Apply user-specified analyzer to given input file(s) in single- or
// multi-threads mode.
//
// Created by Samvel Khalatyan, Jul 31, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_APP_CONTROLLER
#define BSM_APP_CONTROLLER

#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include "interface/bsm_fwd.h"

namespace po = boost::program_options;

namespace bsm
{
    class Options
    {
        public:
            typedef boost::shared_ptr<po::options_description> OptionsPtr;

            virtual ~Options() {}

            virtual OptionsPtr options() const = 0;
    };

    class AppController
    {
        public:
            typedef std::vector<std::string> Inputs;
            typedef boost::shared_ptr<Analyzer> AnalyzerPtr;

            enum RunMode
            {
                SINGLE_THREAD = 0,
                MULTI_THREAD
            };

            AppController();

            void setAnalyzer(const AnalyzerPtr &);

            void addOptions(const Options &);

            void addInputs(const Inputs &);

            bool run(int &argc, char *argv[]);

        private:
            typedef Options::OptionsPtr OptionsPtr;

            // Prevent Copying
            //
            AppController(const AppController &);
            AppController &operator =(const AppController &);

            void setRunMode(const bool &);

            void processSingleThread();
            void processMultiThread();

            RunMode _run_mode;

            OptionsPtr _generic_options;
            OptionsPtr _hidden_options;

            std::vector<OptionsPtr> _custom_options;

            AnalyzerPtr _analyzer;

            Inputs _input_files;
    };
}

#endif
