// b-tagging utils
//
// Created by Samvel Khalatyan, Mar 25, 2011
// Copyright 2011, All rights reserved

#include <stdexcept>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Jet.pb.h"

#include "interface/Btag.h"
#include "interface/CorrectedJet.h"

using namespace bsm;
using namespace std;

BtagOptions::BtagOptions()
{
    _description.reset(new po::options_description("Btag Options"));
    _description->add_options()
        ("use-btag-sf",
         po::value<bool>()->implicit_value(true)->notifier(
             boost::bind(&BtagOptions::setUseBtagSF, this)),
         "Use b-tagging Scale Factors")

        ("btag-up",
         po::value<bool>()->implicit_value(true)->notifier(
             boost::bind(&BtagOptions::setSystematic, this, BtagDelegate::UP)),
         "Change btag SF one sigma up")

        ("btag-down",
         po::value<bool>()->implicit_value(true)->notifier(
             boost::bind(&BtagOptions::setSystematic, this, BtagDelegate::DOWN)),
         "Change btag SF one sigma down")
    ;
}

// Options interface
//
BtagOptions::DescriptionPtr BtagOptions::description() const
{
    return _description;
}

// Private
//
void BtagOptions::setUseBtagSF()
{
    if (delegate())
        delegate()->setUseBtagSF();
}

void BtagOptions::setSystematic(const BtagDelegate::Systematic &systematic)
{
    if (!delegate())
        return;

    delegate()->setSystematic(systematic);
}



// Btag
//
Btag::Btag():
    _systematic(NONE),
    _use_sf(false)
{
    _generator.reset(new boost::mt19937());
}

Btag::Btag(const Btag &object):
    _systematic(object._systematic),
    _use_sf(object._use_sf)
{
    _generator.reset(new boost::mt19937());
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

float Btag::mistag_scale_sigma_down(const float &jet_pt)
{
    return 0.899715 +
           0.00102278 * jet_pt -
           2.46335e-06 * pow(jet_pt, 2) +
           9.71143e-10 * pow(jet_pt, 3);
}

float Btag::mistag_scale_sigma_up(const float &jet_pt)
{
    return 0.997077 +
           0.00473953 * jet_pt -
           1.34985e-05 * pow(jet_pt, 2) +
           1.0032e-08 * pow(jet_pt, 3);
}

float Btag::btag_scale_with_systematic(const float &discriminator,
                                       const float &uncertainty)
{
    float scale = Btag::btag_scale(discriminator);
    
    if (_systematic)
        scale += _systematic * uncertainty;

    return scale;
}

float Btag::mistag_scale_with_systematic(const float &jet_pt)
{
    switch(_systematic)
    {
        case DOWN: return Btag::mistag_scale_sigma_down(jet_pt);
        case NONE: return Btag::mistag_scale(jet_pt);
        case UP: return Btag::mistag_scale_sigma_up(jet_pt);

        default: throw runtime_error("unsupported systematic type used");
    }
}

bool Btag::is_tagged(const CorrectedJet &jet)
{
    typedef ::google::protobuf::RepeatedPtrField<Jet::BTag> BTags;

    for(BTags::const_iterator btag = jet.jet->btag().begin();
            jet.jet->btag().end() != btag;
            ++btag)
    {
        if (Jet::BTag::CSV == btag->type())
        {
            const float discriminator = btag->discriminator();
            bool result = Btag::discriminator() < discriminator;

            if (_use_sf && jet.jet->has_gen_parton())
            {
                float scale = 0;
                float efficiency = 0;

                switch(abs(jet.jet->gen_parton().id()))
                {
                    case 5:
                        scale = btag_scale_with_systematic(discriminator, 0.04);
                        efficiency = Btag::btag_efficiency(discriminator);

                        break;

                    case 4:
                        scale = btag_scale_with_systematic(discriminator, 0.08);
                        efficiency = Btag::btag_efficiency(discriminator);

                        break;

                    case 3: // fall through
                    case 2: // fall through
                    case 1:
                        const float jet_pt = pt(*jet.corrected_p4);

                        scale = mistag_scale_with_systematic(jet_pt);
                        efficiency = Btag::mistag_efficiency(jet_pt);

                        break;
                }

                if (scale && efficiency)
                {
                    _generator->seed(static_cast<unsigned int>(
                                        sin(phi(*jet.corrected_p4) * 1000000)));

                    result = correct(result, scale, efficiency);
                }
            }

            return result;
        }
    }

    return false;
}

// BtagDelegate interface
//
void Btag::setUseBtagSF()
{
    _use_sf = true;
}

void Btag::setSystematic(const Systematic &systematic)
{
    _systematic = systematic;
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
    if (1 < scale)
    {
        if (!is_tagged)
        {
            if ((*_generator)() < (1 - scale) / (1 - scale / efficiency))
                result = true;
        }
    }
    else
    {
        if (is_tagged &&
            (*_generator)() > scale)

            result = false;
    }

    return result;
}
