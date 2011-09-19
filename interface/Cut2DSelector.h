// 2D Cut with sygnal and differente background regions
//
// Created by Samvel Khalatyan, Aug 24, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_CUT2D_SELECTOR
#define BSM_CUT2D_SELECTOR

#include <boost/shared_ptr.hpp>

#include "bsm_input/interface/bsm_input_fwd.h"
#include "interface/AppController.h"
#include "interface/Selector.h"

namespace bsm
{
    class Cut2DSelectorDelegate
    {
        public:
            enum Region
            {
                SIGNAL = 0, // pTre: 25..   DeltaR: 0.5..
                S1,         // pTrel ..25   DeltaR: 0.1..0.2
                S2,         // pTrel ..25   DeltaR: 0.2..0.3
                S3,         // pTrel ..25   DeltaR: 0.3..0.5
                S1S2,
                S2S3,
                S1S2S3
            };

            virtual ~Cut2DSelectorDelegate() {}

            virtual void setRegion(const Region &) {}
    };

    class Cut2DSelectorOptions : public Options
    {
        public:
            Cut2DSelectorOptions();
            virtual ~Cut2DSelectorOptions();

            void setDelegate(Cut2DSelectorDelegate *);
            Cut2DSelectorDelegate *delegate() const;

            // Options interface
            //
            virtual DescriptionPtr description() const;

        private:
            void setRegion(std::string);

            Cut2DSelectorDelegate *_delegate;

            DescriptionPtr _description;
    };

    class Cut2DSelector : public Selector, public Cut2DSelectorDelegate
    {
        public:
            Cut2DSelector(const Region &region = SIGNAL);

            enum Cut
            {
                DELTA_R = 0,
                PTREL
            };

            Region region() const;

            // Cuts accessors
            //
            CutPtr cut(const Cut &) const;

            // Test if object passes the selector
            //
            virtual bool apply(const LorentzVector &lepton,
                    const LorentzVector &jet);

            // Delegate Interface
            //
            virtual void setRegion(const Region &);

            // Selector interface
            //
            virtual void enable();
            virtual void disable();

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;

        private:
            Region _region;
    };
}

#endif
