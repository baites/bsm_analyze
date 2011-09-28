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
    class JetEnergyCorrectionDelegate
    {
        public:
            enum Level
            {
                L1 = 0,
                L2,
                L3
            };

            virtual ~JetEnergyCorrectionDelegate() {}

            virtual void setCorrection(const Level &,
                    const std::string &file_name) {}

            virtual void setDeltaRCorrection() {}
    };

    class JetEnergyCorrectionOptions : public Options
    {
        public:
            JetEnergyCorrectionOptions();

            void setDelegate(JetEnergyCorrectionDelegate *);
            JetEnergyCorrectionDelegate *delegate() const;

            // Options interface
            //
            virtual DescriptionPtr description() const;

        private:
            void setCorrection(const JetEnergyCorrectionDelegate::Level &,
                    const std::string &file_name); 
            
            void setDeltaRCorrection();

            JetEnergyCorrectionDelegate *_delegate;

            DescriptionPtr _description;
    };

    class JetEnergyCorrections : public core::Object,
        public JetEnergyCorrectionDelegate
    {
        public:
            // Inputs
            //
            typedef std::vector<const Electron *> Electrons;
            typedef std::vector<const Muon *> Muons;

            // Output
            //
            typedef boost::shared_ptr<LorentzVector> LorentzVectorPtr;

            JetEnergyCorrections();
            JetEnergyCorrections(const JetEnergyCorrections &);

            struct CorrectedJet
            {
                CorrectedJet()
                {
                    jet = 0;
                    correction = 0;
                }

                const Jet *jet;
                LorentzVectorPtr corrected_p4;
                LorentzVectorPtr subtracted_p4;

                Electrons subtracted_electrons;
                Muons subtracted_muons;
                float correction;
            };

            // IMPORTANT: Invalid pointer will be returned if Jet Energy
            //            Corrections are not loaded. As such, always check
            //            returned value for validity, e.g.:
            //
            //            LorentzVectorPtr corrected_p4 = jec->correctJet(...);
            //            if (!corrected_p4)
            //              cerr << failed to correct jet" << endl;
            //            else
            //              cout << "work with jet" << endl;
            //
            CorrectedJet correctJet(const Jet *,
                    const Event *,
                    const Electrons &,
                    const Muons &);

            // Jet Energy Correction Delegate interface
            //
            // Note: the same level correction will be loaded only once (!)
            //
            virtual void setCorrection(const Level &,
                    const std::string &file_name);

            virtual void setDeltaRCorrection();

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;

            virtual void print(std::ostream &) const;

        private:
            typedef std::map<Level, std::string> CorrectionFiles;
            typedef std::map<Level, JetCorrectorParameters> Corrections;
            typedef boost::shared_ptr<FactorizedJetCorrector> CorrectorPtr;

            CorrectorPtr corrector();
            void correct(CorrectedJet &, const Event *);

            CorrectorPtr _jec;

            Corrections _corrections;
            CorrectionFiles _correction_files;

            bool _use_dr_correction;
    };

    // Helpers
    //
    std::ostream &operator <<(std::ostream &,
            const JetEnergyCorrectionDelegate::Level &);
}

#endif
