// PileUp re-weighting for Summer11 MC/Data
//
// Created by Samvel Khalatyan, Oct 21, 2011
// Copyright 2011, All rights reserved

#include <algorithm>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include <TFile.h>
#include <TH3D.h>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Event.pb.h"
#include "interface/Pileup.h"

using namespace bsm;
using namespace std;
using namespace boost;

namespace fs = boost::filesystem;

// Pileup options
//
PileupOptions::PileupOptions()
{
    _delegate = 0;

    _description.reset(new po::options_description("Pile-up Options"));
    _description->add_options()
        ("pileup",
         po::value<string>()->notifier(
             boost::bind(&PileupOptions::setPileup, this, _1)),
         "Pile-up correction(s) file")
    ;
}

void PileupOptions::setDelegate(PileupDelegate *delegate)
{
    if (_delegate != delegate)
        _delegate = delegate;
}

PileupDelegate *PileupOptions::delegate() const
{
    return _delegate;
}

// Options interface
//
PileupOptions::DescriptionPtr PileupOptions::description() const
{
    return _description;
}

// Private
//
void PileupOptions::setPileup(const string &filename)
{
    if (!delegate())
        return;

    if (fs::exists(filename))
        delegate()->setPileup(filename);
    else
        cerr << "pileup correction(s) file does not exist: "
            << filename << endl;
}



// Pileup
//
Pileup::Pileup()
{
}

Pileup::Pileup(const Pileup &obj):
    _weight(obj._weight)
{
}

void Pileup::setPileup(const string &filename)
{
    shared_ptr<TFile> in(new TFile(filename.c_str(), "readonly"));
    if (!in->IsOpen())
    {
        cerr << "failed to open pileup file: " << filename << endl;

        return;
    }

    TH3D *weights = dynamic_cast<TH3D *>(in->Get("WHist"));

    // Translate weights into local array
    //
    _weight.clear();

    for(uint32_t prev_bunch = 0,
            max_prev_bunch = weights->GetXaxis()->GetNbins();
            max_prev_bunch > prev_bunch;
            ++prev_bunch)
    {
        Weight2D weight2d;
        for(uint32_t curr_bunch = 0,
                max_curr_bunch = weights->GetYaxis()->GetNbins();
                max_curr_bunch > curr_bunch;
                ++curr_bunch)
        {
            Weight1D weight1d;
            for(uint32_t next_bunch = 0,
                    max_next_bunch = weights->GetZaxis()->GetNbins();
                    max_next_bunch > next_bunch;
                    ++next_bunch)
            {
                weight1d.push_back(static_cast<float>(
                        weights->GetBinContent(prev_bunch,
                            curr_bunch,
                            next_bunch)));
            }

            weight2d.push_back(weight1d);
        }

        _weight.push_back(weight2d);
    }

    clog << "pileup loaded " << filename << endl;
}

const float Pileup::scale(const Event *event) const
{
    return !_weight.empty()
        && event->has_pileup()
        && event->pileup().has_interactions_prev_bunch()
        && event->pileup().has_interactions_curr_bunch()
        && event->pileup().has_interactions_next_bunch()

        ? _weight[min(static_cast<int>(event->pileup().interactions_prev_bunch()), 34)]
                 [min(static_cast<int>(event->pileup().interactions_curr_bunch()), 34)]
                 [min(static_cast<int>(event->pileup().interactions_next_bunch()), 34)]

        : 0;
}

// Object interface
//
uint32_t Pileup::id() const
{
    return core::ID<Pileup>::get();
}

Pileup::ObjectPtr Pileup::clone() const
{
    return ObjectPtr(new Pileup(*this));
}

void Pileup::print(ostream &) const
{
}
