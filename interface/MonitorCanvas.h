// Monitors Visualizers
//
// Generate standartized Canvases for monitors
//
// Created by Samvel Khalatyan, May 17, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_MONITOR_CANVAS
#define BSM_MONITOR_CANVAS

#include <string>
#include <stack>

#include <boost/shared_ptr.hpp>

#include "bsm_core/interface/bsm_core_fwd.h"
#include "bsm_stat/interface/bsm_stat_fwd.h"
#include "interface/bsm_fwd.h"

class TCanvas;
class TDirectory;

namespace bsm
{
    class Canvas
    {
        public:
            typedef boost::shared_ptr<TCanvas> TCanvasPtr;

            typedef stat::TH1Ptr TH1Ptr;
            typedef stat::TH2Ptr TH2Ptr;

            Canvas(const std::string &title = "",
                    const std::string &axis_subtitle = ""):
                _title(title),
                _axis_subtitle(axis_subtitle)
                {
                }

            virtual ~Canvas()
            {
            }

            inline const std::string title() const
            {
                return _title;
            }

            inline const std::string axis_subtitle() const
            {
                return _axis_subtitle;
            }

            bool pushd(TDirectory *parent = 0);
            bool popd();

            std::string folder();

            TCanvasPtr canvas();

        private:
            typedef boost::shared_ptr<core::IDCounter> IDPtr;

            static uint32_t _id;

            std::string _title;
            std::string _axis_subtitle;

            std::string _folder;
            TCanvasPtr _canvas;

            std::stack<TDirectory *> _dirs;
    };

    class DeltaCanvas: public Canvas
    {
        public:
            DeltaCanvas(const std::string &title = "",
                        const std::string &axis_subtitle = ""):
                Canvas(title, axis_subtitle)
                {
                }

            void draw(const DeltaMonitor &);
            void write(const DeltaMonitor &, TDirectory *parent = 0);

        private:
            TH1Ptr _r;
            TH1Ptr _ptrel;
            TH1Ptr _angle;
            TH2Ptr _ptrel_vs_r;
            TH1Ptr _eta;
            TH1Ptr _phi;
    };

    class ElectronCanvas: public Canvas
    {
        public:
            ElectronCanvas(const std::string &title = "",
                           const std::string &axis_subtitle = ""):
                Canvas(title, axis_subtitle)
                {
                }

            void draw(const ElectronsMonitor &);
            void write(const ElectronsMonitor &, TDirectory *parent = 0);

        private:
            TH1Ptr _multiplicity;
            TH1Ptr _leading_pt;
            TH1Ptr _pt;
    };

    class JetCanvas: public Canvas
    {
        public:
            JetCanvas(const std::string &title = "",
                      const std::string &axis_subtitle = ""):
                Canvas(title, axis_subtitle)
                {
                }

            void draw(const JetsMonitor &);
            void write(const JetsMonitor &, TDirectory *parent = 0);

        private:
            TH1Ptr _multiplicity;
            TH1Ptr _leading_pt;
            TH1Ptr _leading_uncorrected_pt;
            TH1Ptr _uncorrected_pt;
            TH1Ptr _pt;
            TH1Ptr _children;
    };

    class P4Canvas: public Canvas
    {
        public:
            P4Canvas(const std::string &title = "",
                     const std::string &axis_subtitle = ""):
                Canvas(title, axis_subtitle)
                {
                }

            void draw(const P4Monitor &);
            void write(const P4Monitor &, TDirectory *parent = 0);

        private:
            TH1Ptr _energy;
            TH1Ptr _px;
            TH1Ptr _py;
            TH1Ptr _pz;

            TH1Ptr _pt;
            TH1Ptr _eta;
            TH1Ptr _phi;
            TH1Ptr _mass;

            TH1Ptr _mt;
            TH1Ptr _et;
    };

    class GenParticleCanvas: public P4Canvas
    {
        public:
            GenParticleCanvas(const std::string &title = "",
                              const std::string &axis_subtitle = ""):
                P4Canvas(title, axis_subtitle)
                {
                }

            void draw(const GenParticleMonitor &);
            void write(const GenParticleMonitor &, TDirectory *parent = 0);

        private:
            TH1Ptr _pdg_id;
            TH1Ptr _status;
    };

    class MissingEnergyCanvas: public Canvas
    {
        public:
            MissingEnergyCanvas(const std::string &title = "",
                                const std::string &axis_subtitle = ""):
                Canvas(title, axis_subtitle)
                {
                }

            void draw(const MissingEnergyMonitor &);
            void write(const MissingEnergyMonitor &, TDirectory *parent = 0);

        private:
            TH1Ptr _pt;
            TH1Ptr _x;
            TH1Ptr _y;
            TH1Ptr _z;
    };

    class MuonCanvas: public Canvas
    {
        public:
            MuonCanvas(const std::string &title = "",
                       const std::string &axis_subtitle = ""):
                Canvas(title, axis_subtitle)
                {
                }

            void draw(const MuonsMonitor &);
            void write(const MuonsMonitor &, TDirectory *parent = 0);

        private:
            TH1Ptr _multiplicity;
            TH1Ptr _leading_pt;
            TH1Ptr _pt;
    };

    class PrimaryVertexCanvas: public Canvas
    {
        public:
            PrimaryVertexCanvas(const std::string &title = "",
                                const std::string &axis_subtitle = ""):
                Canvas(title, axis_subtitle)
                {
                }

            void draw(const PrimaryVerticesMonitor &);
            void write(const PrimaryVerticesMonitor &, TDirectory *parent = 0);

        private:
            TH1Ptr _multiplicity;
            TH1Ptr _x;
            TH1Ptr _y;
            TH1Ptr _z;
    };
}

#endif
