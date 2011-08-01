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

namespace po = boost::program_options;

namespace bsm
{
    class AppController
    {
        public:
            typedef boost::shared_ptr<po::options_description> OptionsPtr;
            typedef std::vector<std::string> Inputs;

            enum RunMode
            {
                SINGLE_THREAD = 0,
                MULTI_THREAD
            };

            AppController();

            void addOptions(const OptionsPtr &);

            void addInputs(const Inputs &);

            void setRunMode(const RunMode &);

            bool run(int &argc, char *argv[]);

        private:
            // Prevent Copying
            //
            AppController(const AppController &);
            AppController &operator =(const AppController &);

            void setMultiThreadMode();

            RunMode _run_mode;

            OptionsPtr _generic_options;
            OptionsPtr _hidden_options;

            std::vector<OptionsPtr> _custom_options;

            Inputs _input_files;
    };
}

#endif
