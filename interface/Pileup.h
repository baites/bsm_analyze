// PileUp re-weighting for Summer11 MC/Data
//
// Created by Samvel Khalatyan, Oct 21, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_PILEUP
#define BSM_PILEUP

#include <string>
#include <vector>

#include "bsm_core/interface/Object.h"
#include "bsm_input/interface/bsm_input_fwd.h"
#include "interface/AppController.h"

namespace bsm
{
    class PileupDelegate
    {
        public:
            enum Systematic
            {
                DOWN = 0,
                UP,
                NONE
            };

            virtual ~PileupDelegate() {}

            virtual void setPileup(const std::string &filename,
                    const Systematic &) {}
    };

    class PileupOptions : public Options
    {
        public:
            PileupOptions();

            void setDelegate(PileupDelegate *);
            PileupDelegate *delegate() const;

            // Options interface
            //
            virtual DescriptionPtr description() const;

        private:
            void setPileup(const std::string &filename,
                    const PileupDelegate::Systematic & = PileupDelegate::NONE);

            PileupDelegate *_delegate;

            DescriptionPtr _description;
    };

    class Pileup : public core::Object,
                   public PileupDelegate
    {
        public:
            Pileup();
            Pileup(const Pileup &);

            virtual void setPileup(const std::string &filename,
                    const Systematic &systematic);

            const float scale(const Event *) const;

            // Object interface
            //
            virtual uint32_t id() const;

            virtual ObjectPtr clone() const;
            virtual void print(std::ostream &) const;

        private:
            typedef std::vector<float> Weight1D;
            typedef std::vector<Weight1D> Weight2D;
            typedef std::vector<Weight2D> Weight3D;

            Weight3D _weight;
    };
}

#endif
