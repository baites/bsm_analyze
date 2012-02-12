// Study hadronic top
//
// Created by Samvel Khalatyan, Feb 09, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_HADRONIC_TOP_ANALYZER
#define BSM_HADRONIC_TOP_ANALYZER

#include <string>

#include <boost/shared_ptr.hpp>

#include "bsm_stat/interface/bsm_stat_fwd.h"
#include "interface/bsm_fwd.h"

#include "interface/Analyzer.h"

namespace bsm
{
    class HadronicTopDelegate 
    {
        public:
            virtual ~HadronicTopDelegate()
            {
            }

            virtual void setHtopNjets(const int &min, const int &max = 0)
            {
            }
    };

    class HadronicTopOptions : public Options
    {
        public:
            HadronicTopOptions();

            void setDelegate(HadronicTopDelegate *);
            HadronicTopDelegate *delegate() const;

            // Options interface
            //
            virtual DescriptionPtr description() const;

        private:
            void setHtopNjets(const std::string &);

            HadronicTopDelegate *_delegate;

            DescriptionPtr _description;
    };

    class HadronicTopAnalyzer : public Analyzer, public HadronicTopDelegate
    {
        public:
            typedef boost::shared_ptr<stat::H1> H1Ptr;
            typedef boost::shared_ptr<stat::H2> H2Ptr;

            typedef boost::shared_ptr<P4Monitor> P4MonitorPtr;
            typedef boost::shared_ptr<DeltaMonitor> DeltaMonitorPtr;
            typedef boost::shared_ptr<GenParticleMonitor> GenParticleMonitorPtr;

            HadronicTopAnalyzer();
            HadronicTopAnalyzer(const HadronicTopAnalyzer &);

            // Histograms
            //
            const P4MonitorPtr top() const;
            const P4MonitorPtr jet1() const;
            const P4MonitorPtr jet2() const;
            const P4MonitorPtr jet3() const;
            const P4MonitorPtr jet4() const;

            const GenParticleMonitorPtr jet1_parton() const;

            const DeltaMonitorPtr jet1_vs_jet2() const;
            const DeltaMonitorPtr jet1_vs_jet3() const;
            const DeltaMonitorPtr jet1_vs_jet4() const;
            const DeltaMonitorPtr jet2_vs_jet3() const;
            const DeltaMonitorPtr jet2_vs_jet4() const;
            const DeltaMonitorPtr jet3_vs_jet4() const;

            const H1Ptr njets() const;

            const H2Ptr njets_vs_mass() const;
            const H2Ptr pt_vs_mass() const;
            const H2Ptr njets_vs_pt() const;

            JetEnergyCorrectionDelegate *getJetEnergyCorrectionDelegate() const;
            SynchSelectorDelegate *getSynchSelectorDelegate() const;
            PileupDelegate *getPileupDelegate() const;
            TriggerDelegate *getTriggerDelegate() const;

            // Hadronic Top delegate interface
            //
            virtual void setHtopNjets(const int &min, const int &max);

            // Anlayzer interface
            //
            virtual void onFileOpen(const std::string &filename, const Input *);
            virtual void process(const Event *);

            // Object interface
            //
            virtual uint32_t id() const;
            virtual ObjectPtr clone() const;
            virtual void print(std::ostream &) const;

        private:
            typedef boost::shared_ptr<H1Proxy> H1ProxyPtr;
            typedef boost::shared_ptr<H2Proxy> H2ProxyPtr;

            typedef ResonanceReconstructor::Mttbar Mttbar;

            Mttbar mttbar() const;

            boost::shared_ptr<SynchSelector> _synch_selector;

            // map: counter pointer to SynchSelector selection
            //
            P4MonitorPtr _top;
            P4MonitorPtr _jet1;
            P4MonitorPtr _jet2;
            P4MonitorPtr _jet3;
            P4MonitorPtr _jet4;

            GenParticleMonitorPtr _jet1_parton;

            DeltaMonitorPtr _jet1_vs_jet2;
            DeltaMonitorPtr _jet1_vs_jet3;
            DeltaMonitorPtr _jet1_vs_jet4;
            DeltaMonitorPtr _jet2_vs_jet3;
            DeltaMonitorPtr _jet2_vs_jet4;
            DeltaMonitorPtr _jet3_vs_jet4;

            H1ProxyPtr _njets;

            H2ProxyPtr _njets_vs_mass;
            H2ProxyPtr _pt_vs_mass;
            H2ProxyPtr _njets_vs_pt;

            boost::shared_ptr<Pileup> _pileup;

            bool _use_pileup;
            float _pileup_weight;

            boost::shared_ptr<ResonanceReconstructor> _reconstructor;

            struct {
                uint32_t min;
                uint32_t max;
            } _htop_njets;
    };
}

#endif
