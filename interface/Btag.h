// b-tagging utils
//
// Created by Samvel Khalatyan, Mar 25, 2011
// Copyright 2011, All rights reserved

#ifndef BTAG_H
#define BTAG_H

#include <boost/random.hpp>

#include "bsm_core/interface/Object.h"
#include "interface/bsm_fwd.h"

namespace bsm
{
    class Btag: public core::Object
    {
        public:
            Btag();

            static float discriminator();

            static float btag_efficiency(const float &discriminator);
            static float btag_scale(const float &discriminator);

            static float mistag_efficiency(const float &jet_pt);
            static float mistag_scale(const float &jet_pt);

            bool is_tagged(const CorrectedJet *jet);

            // Object interface
            //
            virtual uint32_t id() const;
            virtual ObjectPtr clone() const;
            virtual void print(std::ostream &) const;

        private:
            bool correct(const bool &is_tagged,
                         const float &scale,
                         const float &efficiency);

            boost::shared_ptr<boost::mt19937> _generator;
    };
}

#endif
