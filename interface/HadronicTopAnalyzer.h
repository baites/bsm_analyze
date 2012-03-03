// Study hadronic top
//
// Created by Samvel Khalatyan, Feb 09, 2012
// Copyright 2012, All rights reserved

#ifndef BSM_HADRONIC_TOP_ANALYZER
#define BSM_HADRONIC_TOP_ANALYZER

#include <sstream>
#include <string>

#include <boost/shared_ptr.hpp>

#include "bsm_stat/interface/bsm_stat_fwd.h"
#include "interface/bsm_fwd.h"

#include "interface/Analyzer.h"
#include "interface/DelegateManager.h"
#include "interface/Monitor.h"
#include "interface/TemplateAnalyzer.h"

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

    class HadronicTopOptions:
        public Options, 
        public DelegateManager<HadronicTopDelegate>
    {
        public:
            HadronicTopOptions();

            // Options interface
            //
            virtual DescriptionPtr description() const;

        private:
            void setHtopNjets(const std::string &);

            DescriptionPtr _description;
    };

    class HadronicTopAnalyzer:
        public Analyzer,
        public HadronicTopDelegate,
        public TemplatesDelegate
    {
        public:
            typedef boost::shared_ptr<stat::H1> H1Ptr;
            typedef boost::shared_ptr<stat::H2> H2Ptr;

            typedef boost::shared_ptr<P4Monitor> P4MonitorPtr;
            typedef boost::shared_ptr<DeltaMonitor> DeltaMonitorPtr;
            typedef boost::shared_ptr<GenParticleMonitor> GenParticleMonitorPtr;

            HadronicTopAnalyzer();
            HadronicTopAnalyzer(const HadronicTopAnalyzer &);

            virtual void setBtagReconstruction();
            virtual void setSimpleDrReconstruction();
            virtual void setHemisphereReconstruction();
            virtual void setReconstructionWithMass();
            virtual void setReconstructionWithPhi();
            virtual void setReconstructionWithMassAndPhi();
            virtual void setSimpleReconstructionWithMassAndPhi();
            virtual void setSimpleReconstructionWithMass();
            virtual void setCollimatedSimpleReconstructionWithMass();

            // Histograms
            //
            const P4MonitorPtr top() const;
            const P4MonitorPtr jet1() const;
            const P4MonitorPtr jet2() const;
            const P4MonitorPtr jet3() const;
            const P4MonitorPtr jet4() const;

            const GenParticleMonitorPtr jet1_parton() const;
            const GenParticleMonitorPtr jet2_parton() const;

            const DeltaMonitorPtr jet1_parton_vs_jet2_parton() const;

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

                // Gen Particles
            const GenParticleMonitorPtr gen_top() const;
            const GenParticleMonitorPtr gen_jet1() const;
            const GenParticleMonitorPtr gen_jet2() const;
            const GenParticleMonitorPtr gen_jet3() const;

            const H1Ptr njets_gen() const;
            const H2Ptr njets_gen_vs_gen_mass() const;
            const H2Ptr pt_gen_vs_gen_mass() const;
            const H2Ptr njets_gen_vs_gen_pt() const;

                // TTbar system
            const P4MonitorPtr ttbar_reco() const;
            const P4MonitorPtr ttbar_gen() const;

            const DeltaMonitorPtr ttbar_reco_delta() const;
            const DeltaMonitorPtr ttbar_gen_delta() const;

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
            void merge(const ObjectPtr &);
            virtual void print(std::ostream &) const;

        private:
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
            GenParticleMonitorPtr _jet2_parton;

            DeltaMonitorPtr _jet1_parton_vs_jet2_parton;

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

            // Generator particles
            //
            GenParticleMonitorPtr _gen_top;
            GenParticleMonitorPtr _gen_jet1;
            GenParticleMonitorPtr _gen_jet2;
            GenParticleMonitorPtr _gen_jet3;

            H1ProxyPtr _njets_gen;
            H2ProxyPtr _njets_gen_vs_gen_mass;
            H2ProxyPtr _pt_gen_vs_gen_mass;
            H2ProxyPtr _njets_gen_vs_gen_pt;


            // TTbar system
            //
            P4MonitorPtr _ttbar_reco;
            P4MonitorPtr _ttbar_gen;

            DeltaMonitorPtr _ttbar_reco_delta;
            DeltaMonitorPtr _ttbar_gen_delta;

            boost::shared_ptr<Pileup> _pileup;

            bool _use_pileup;
            float _pileup_weight;

            boost::shared_ptr<ResonanceReconstructor> _reconstructor;

            struct {
                uint32_t min;
                uint32_t max;
            } _htop_njets;

            std::ostringstream _log;
    };
}

#endif
