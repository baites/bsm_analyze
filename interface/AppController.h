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
            typedef boost::shared_ptr<po::options_description> DescriptionPtr;

            virtual ~Options() {}

            virtual DescriptionPtr description() const = 0;
    };

    class AppController
    {
        public:
            typedef std::vector<std::string> Inputs;
            typedef boost::shared_ptr<Analyzer> AnalyzerPtr;

            AppController();

            void setAnalyzer(const AnalyzerPtr &);

            void addOptions(const Options &);

            void addInputs(const Inputs &);

            bool run(int &argc, char *argv[]);

        private:
            // Prevent Copying
            //
            AppController(const AppController &);
            AppController &operator =(const AppController &);

            typedef Options::DescriptionPtr DescriptionPtr;

            enum RunMode
            {
                SINGLE_THREAD = 0,
                MULTI_THREAD
            };

            void setRunMode(const bool &);

            void processSingleThread();
            void processMultiThread();

            RunMode _run_mode;

            DescriptionPtr _generic_options;
            DescriptionPtr _hidden_options;

            std::vector<DescriptionPtr> _custom_options;

            AnalyzerPtr _analyzer;

            Inputs _input_files;
    };
}

#endif
