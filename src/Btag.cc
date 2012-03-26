// b-tagging utils
//
// Created by Samvel Khalatyan, Mar 25, 2011
// Copyright 2011, All rights reserved

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Jet.pb.h"

#include "interface/Btag.h"
#include "interface/CorrectedJet.h"

using namespace bsm;

Btag::Btag()
{
}

float Btag::discriminator()
{
    return 0.898;
}

float Btag::btag_efficiency(const float &discriminator)
{
    return 1.04305075822 -
           1.03328577451 * discriminator +
           0.784721653518 * pow(discriminator, 2) +
           1.26161794785 * pow(discriminator, 3) -
           1.73338329789 * pow(discriminator, 4);
}

float Btag::btag_scale(const float &discriminator)
{
    return 1.04926963159 - 0.113472343605 * discriminator;
}

float Btag::mistag_efficiency(const float &jet_pt)
{
    return 0.00315116 * (1 -
                         0.00769281 * jet_pt +
                         2.58066e-05 * pow(jet_pt, 2) -
                         2.02149e-08 * pow(jet_pt, 3));
}

float Btag::mistag_scale(const float &jet_pt)
{
    return 0.948463 +
           0.00288102 * jet_pt -
           7.98091e-06 * pow(jet_pt, 2) +
           5.50157e-09 * pow(jet_pt, 3);
}

bool Btag::is_tagged(const CorrectedJet *jet)
{
    typedef ::google::protobuf::RepeatedPtrField<Jet::BTag> BTags;

    for(BTags::const_iterator btag = jet->jet->btag().begin();
            jet->jet->btag().end() != btag;
            ++btag)
    {
        if (Jet::BTag::CSV == btag->type())
        {
            bool result = Btag::discriminator() < btag->discriminator();

            if (jet->jet->has_gen_parton())
            {
                switch(abs(jet->jet->gen_parton().id()))
                {
                    case 5: // fall through
                    case 4:
                        result = correct(
                            result,
                            Btag::btag_scale(Btag::discriminator()),
                            Btag::btag_efficiency(Btag::discriminator()));
                        break;

                    case 3: // fall through
                    case 2: // fall through
                    case 1:
                        const float jet_pt = 0;
                        result = correct(result,
                                         Btag::mistag_scale(jet_pt),
                                         Btag::mistag_efficiency(jet_pt));
                        break;
                }
            }

            return result;
        }
    }

    return false;
}

// Object interface
//
uint32_t Btag::id() const
{
    return core::ID<Btag>::get();
}

Btag::ObjectPtr Btag::clone() const
{
    return ObjectPtr(new Btag(*this));
}

void Btag::print(std::ostream &out) const
{
}



// Private
//
bool Btag::correct(const bool &is_tagged,
                   const float &scale,
                   const float &efficiency)
{
    if (1 == scale)
        return is_tagged;

    bool result = is_tagged;
    float random_value = 0;
    if (1 < scale)
    {
        if (!is_tagged)
        {
            if (random_value < (1 - scale) / (1 - scale / efficiency))
                result = true;
        }
    }
    else
    {
        if (is_tagged &&
            random_value > scale)

            result = false;
    }

    return result;
}
