// Draw jet energy scale systematics
//
// Created by Samvel Khalatyan, Nov 18, 2011
// Copyright 2011, All rights reserved

#include <iostream>

#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TObject.h>
#include <TSystem.h>

#include "interface/JESSystematic.h"

using namespace std;

void JESSystematic::draw()
{
    plot(Input::TTJETS);
    plot(Input::WJETS);
    plot(Input::ZJETS);

    // canvas 4 is reserved for single-top(s)
    //
    plot(Input::ZPRIME1000);
    plot(Input::ZPRIME1500);
    plot(Input::ZPRIME2000);
    plot(Input::ZPRIME3000);
}

void JESSystematic::load(Plots &plots,
        const string &suffix,
        const int &systematic)
{
    FileStat_t buf;
    for(Input input(Input::QCD_BC_PT20_30), end(Input::UNKNOWN);
            end > input;
            ++input)
    {
        string filename = folder(input) + "/" + file_mask() + suffix + ".root";
        if (gSystem->GetPathInfo(filename.c_str(), buf))
        {
            cerr << "skip file: " << filename << endl;

            continue;
        }

        TFile *file = TFile::Open(filename.c_str());
        if (!file
                || !file->IsOpen())
        {
            cerr << "failed to open: " << filename << endl;

            continue;
        }

        // extract histogram
        //
        TObject *object = file->Get(plotname().c_str());
        if (!object)
        {
            cerr << "failed to extract plot: " << plotname()
                << " from: " << filename << endl;

            continue;
        }

        TH1 *histogram = dynamic_cast<TH1 *>(object->Clone());
        if (!histogram)
        {
            cerr << "extracted plot: " << plotname()
                << " is not TH1" << endl;

            continue;
        }

        scale(histogram, input);

        style(histogram, systematic);

        histogram->Rebin(100);

        // store plot
        //
        plots[input.type()] = histogram;
    }
}

void JESSystematic::plot(const Input::Type &type)
{
    if (_jes_none.end() != _jes_none.find(type)
            && _jes_plus.end() != _jes_plus.find(type)
            && _jes_minus.end() != _jes_minus.find(type))
    {
        Input input(type);
        TCanvas *canvas = Systematic::draw(input,
                _jes_none[type],
                _jes_plus[type],
                _jes_minus[type]);

        canvas->SaveAs(("jes_" + input.repr() + ".pdf").c_str());
    }
    else
    {
        cerr << "can not plot input: " << Input(type) << endl;
    }
}

void JESSystematic::style(TH1 *hist, const int &systematic)
{
    int color = 1;
    switch(systematic)
    {
        case 1:
            color = 2;
            break;

        case -1:
            color = 8;
            break;

        default:
            break;
    }

    hist->SetLineColor(color);
    hist->SetMarkerColor(color);
    hist->SetMarkerSize(0.5);
    hist->SetLineWidth(2);
}
