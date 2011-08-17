// Mttbar reconstruction with DeltaR selection
//
// Created by Samvel Khalatyan, Jun 07, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_MTTBAR_ANALYZER
#define BSM_MTTBAR_ANALYZER

#include <sstream>

#include <boost/shared_ptr.hpp>

#include "bsm_stat/interface/bsm_stat_fwd.h"
#include "bsm_input/interface/bsm_input_fwd.h"
#include "bsm_input/interface/GenParticle.pb.h"
#include "interface/Analyzer.h"
#include "interface/bsm_fwd.h"

namespace bsm
{
    class MttbarDelegate
    {
        public:
            virtual ~MttbarDelegate()
            {
            }

            virtual void setUseGeneratorMass(const bool &)
            {
            }
    };

    class MttbarOptions : public Options
    {
        public:
            MttbarOptions();
            virtual ~MttbarOptions();

            void setDelegate(MttbarDelegate *);
            MttbarDelegate *delegate() const;

            // Options interface
            //
            virtual DescriptionPtr description() const;

        private:
            void setUseGeneratorMass(const bool &) const;

            MttbarDelegate *_delegate;
            DescriptionPtr _description;
    };

    class MttbarAnalyzer : public Analyzer, public MttbarDelegate
    {
        public:
            typedef boost::shared_ptr<stat::H1> H1Ptr;
            typedef boost::shared_ptr<stat::H2> H2Ptr;

            typedef boost::shared_ptr<LorentzVectorMonitor> P4MonitorPtr;
            typedef boost::shared_ptr<DeltaMonitor> DeltaMonitorPtr;

            MttbarAnalyzer();
            MttbarAnalyzer(const MttbarAnalyzer &);

            JetEnergyCorrectionDelegate *getJetEnergyCorrectionDelegate() const;
            SynchSelectorDelegate *getSynchSelectorDelegate() const;

            // Getters
            //
            const H1Ptr mreco() const;
            const H2Ptr mltopVsMhtop() const;

            const H1Ptr mgen() const;
            const H1Ptr mrecoMinusMgen() const;
            const H2Ptr mrecoVsMgen() const;

            const P4MonitorPtr missingEnergyMonitor() const;
            const P4MonitorPtr lwbosonMonitor() const;
            const P4MonitorPtr ltopMonitor() const;
            const P4MonitorPtr htopMonitor() const;
            const DeltaMonitorPtr topDeltaMonitor() const;

            // Mttbar Delegate interface
            // 
            virtual void setUseGeneratorMass(const bool &);

            // Analyzer interface
            //
            virtual void onFileOpen(const std::string &filename, const Input *);
            virtual void process(const Event *);

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
            virtual void merge(const ObjectPtr &);

            virtual void print(std::ostream &) const;

        private:
            typedef ::google::protobuf::RepeatedPtrField<GenParticle> GenParticles;
            typedef boost::shared_ptr<H1Proxy> H1ProxyPtr;
            typedef boost::shared_ptr<H2Proxy> H2ProxyPtr;

            enum
            {
                TOP = 6,
                ELECTRON = 11,
                MUON = 13,
                WBOSON = 24
            };

            float getMttbarGen(const Event *);

            GenParticles::const_iterator find(const GenParticles &,
                    const uint32_t &id);

            GenParticles::const_iterator find(const GenParticles &,
                    const uint32_t &id, 
                    const GenParticles::const_iterator &from);

            bool isLeptonicDecay(const GenParticle &);

            boost::shared_ptr<SynchSelector> _synch_selector;

            P4MonitorPtr _missing_energy_monitor;
            P4MonitorPtr _lwboson_monitor;
            P4MonitorPtr _ltop_monitor;
            P4MonitorPtr _htop_monitor;

            DeltaMonitorPtr _top_delta_monitor;

            H1ProxyPtr _mreco;
            H2ProxyPtr _mltop_vs_mhtop;

            H1ProxyPtr _mgen;
            H1ProxyPtr _mreco_minus_mgen;
            H2ProxyPtr _mreco_vs_mgen;

            std::ostringstream _out;

            bool _use_generator_mass;
    };
}

#endif
