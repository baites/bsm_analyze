// Jet Energy Corrections Package
//
// Wrapper around CMSSW Jet Energy Corrections with options
//
// Created by Samvel Khalatyan, Jul 31, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_JET_ENERGY_CORRECTIONS
#define BSM_JET_ENERGY_CORRECTIONS

#include <iosfwd>
#include <string>

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include "interface/AppController.h"

namespace bsm
{
    enum JetEnergyCorrectionLevel
    {
        L1 = 0,
        L2,
        L3
    };

    class JetEnergyCorrectionDelegate;

    class JetEnergyCorrectionOptions : public Options
    {
        public:
            typedef boost::shared_ptr<po::options_description> OptionsPtr;

            JetEnergyCorrectionOptions();
            virtual ~JetEnergyCorrectionOptions();

            // Options interface
            //
            virtual OptionsPtr options() const;

            void setCorrections(const JetEnergyCorrectionLevel &,
                    const std::string &file_name); 

            void setDelegate(JetEnergyCorrectionDelegate *);
            JetEnergyCorrectionDelegate *delegate() const;

        private:
            JetEnergyCorrectionDelegate *_delegate;

            OptionsPtr _options;
    };

    // Helpers
    //
    std::ostream &operator <<(std::ostream &, const JetEnergyCorrectionLevel &);
}

#endif
