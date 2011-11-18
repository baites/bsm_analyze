#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TObject.h>
#include <TSystem.h>

#include "interface/PileupSystematic.h"

using namespace std;

void PileupSystematic::draw()
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

void PileupSystematic::load(Plots &plots,
        const string &suffix,
        const int &systematic)
{
    FileStat_t buf;
    for(Input input(Input::QCD_BC_PT20_30), end(Input::UNKNOWN);
            end > input;
            ++input)
    {
        string filename = folder(input) + "/output_signal_p250_hlt" + suffix + ".root";
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

void PileupSystematic::plot(const Input::Type &type)
{
    if (_pileup_none.end() != _pileup_none.find(type)
            && _pileup_plus.end() != _pileup_plus.find(type)
            && _pileup_minus.end() != _pileup_minus.find(type))
    {
        Input input(type);
        TCanvas *canvas = Systematic::draw(input,
                _pileup_none[type],
                _pileup_plus[type],
                _pileup_minus[type]);

        canvas->SaveAs(("pileup_" + input.repr() + ".pdf").c_str());
    }
    else
    {
        cerr << "can not plot input: " << Input(type) << endl;
    }
}

void PileupSystematic::style(TH1 *hist, const int &systematic)
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
