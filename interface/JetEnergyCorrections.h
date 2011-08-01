// Jet Energy Corrections Package
//
// Wrapper around CMSSW Jet Energy Corrections with options
//
// Created by Samvel Khalatyan, Jul 31, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_JET_ENERGY_CORRECTIONS
#define BSM_JET_ENERGY_CORRECTIONS

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include "bsm_core/interface/Object.h"
#include "bsm_input/interface/bsm_input_fwd.h"
#include "JetMETObjects/interface/JetCorrectorParameters.h"
#include "interface/AppController.h"

class FactorizedJetCorrector;

namespace bsm
{
    enum JetEnergyCorrectionLevel
    {
        L1 = 0,
        L2,
        L3
    };

    class JetEnergyCorrectionDelegate
    {
        public:
            virtual ~JetEnergyCorrectionDelegate() {}

            virtual void setCorrection(const JetEnergyCorrectionLevel &,
                    const std::string &file_name) {}
    };

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

    class JetEnergyCorrections : public JetEnergyCorrectionDelegate,
        public core::Object
    {
        public:
            typedef std::vector<const Electron *> Electrons;
            typedef std::vector<const Muon *> Muons;

            JetEnergyCorrections();
            JetEnergyCorrections(const JetEnergyCorrections &);

            virtual void setCorrection(const JetEnergyCorrectionLevel &,
                    const std::string &file_name);

            LorentzVector correctJet(const Jet *,
                    const Event *,
                    const Electrons &,
                    const Muons &);

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
            using Object::merge;

            virtual void print(std::ostream &) const;

        private:
            typedef std::map<JetEnergyCorrectionLevel, JetCorrectorParameters> Corrections;
            typedef boost::shared_ptr<FactorizedJetCorrector> CorrectorPtr;

            CorrectorPtr corrector();

            CorrectorPtr _jec;

            Corrections _corrections;
    };

    // Helpers
    //
    std::ostream &operator <<(std::ostream &, const JetEnergyCorrectionLevel &);
}

#endif
