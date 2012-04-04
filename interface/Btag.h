// b-tagging utils
//
// Created by Samvel Khalatyan, Mar 25, 2011
// Copyright 2011, All rights reserved

#ifndef BTAG_H
#define BTAG_H

#include <boost/random.hpp>

#include "bsm_core/interface/Object.h"
#include "interface/bsm_fwd.h"
#include "interface/AppController.h"
#include "interface/DelegateManager.h"

namespace bsm
{
    class BtagDelegate
    {
        public:
            enum Systematic
            {
                DOWN = -1,
                NONE = 0,
                UP = 1
            };

            virtual ~BtagDelegate()
            {
            }

            virtual void setUseBtagSF()
            {
            }

            virtual void setSystematic(const Systematic &)
            {
            }
    };

    class BtagOptions:
        public Options,
        public DelegateManager<BtagDelegate>
    {
        public:
            BtagOptions();

            // Options interface
            //
            virtual DescriptionPtr description() const;

        private:
            void setUseBtagSF();
            void setSystematic(const BtagDelegate::Systematic &);

            DescriptionPtr _description;
    };

    class Btag: public core::Object,
                public BtagDelegate
    {
        public:
            Btag();
            Btag(const Btag &);

            static float discriminator();

            static float btag_efficiency(const float &discriminator);
            static float btag_scale(const float &discriminator);

            static float mistag_efficiency(const float &jet_pt);
            static float mistag_scale(const float &jet_pt);
            static float mistag_scale_sigma_up(const float &jet_pt);
            static float mistag_scale_sigma_down(const float &jet_pt);

            float btag_scale_with_systematic(const float &discriminator,
                                             const float &uncertainty);
            float mistag_scale_with_systematic(const float &jet_pt);

            bool is_tagged(const CorrectedJet &jet);

            // BtagDelegate interface
            //
            virtual void setUseBtagSF();
            virtual void setSystematic(const Systematic &);

            // Object interface
            //
            virtual uint32_t id() const;
            virtual ObjectPtr clone() const;
            virtual void print(std::ostream &) const;

        private:
            // Prevent copying
            //
            Btag &operator =(const Btag &);

            bool correct(const bool &is_tagged,
                         const float &scale,
                         const float &efficiency);

            boost::shared_ptr<boost::mt19937> _generator;

            Systematic _systematic;

            bool _use_sf;
    };
}

#endif
