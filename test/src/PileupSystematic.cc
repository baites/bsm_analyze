#include "interface/PileupSystematic.h"

using namespace boost;

using namespace std;

void PileupSystematic::draw()
{
    _canvas.reset(new TCanvas());
    _canvas->SetWindowSize(1200, 800);
    _canvas->Divide(4, 2);

    _canvas->cd(1);
    plot(Input::TTJETS);

    _canvas->cd(2);
    plot(Input::WJETS);

    _canvas->cd(3);
    plot(Input::ZJETS);

    // canvas 4 is reserved for single-top(s)
    //

    _canvas->cd(5);
    plot(Input::ZPRIME1000);

    _canvas->cd(6);
    plot(Input::ZPRIME1500);

    _canvas->cd(7);
    plot(Input::ZPRIME2000);

    _canvas->cd(8);
    plot(Input::ZPRIME3000);

    _canvas->Update();
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
        _pileup_plus[type]->SetFillStyle(30004);
        _pileup_plus[type]->Draw("hist");

        _pileup_none[type]->Draw("hist same");

        _pileup_minus[type]->SetFillStyle(3005);
        _pileup_minus[type]->Draw("hist same");

        TLegend *legend = createLegend(static_cast<string>(Input(type)));
        legend->AddEntry(_pileup_plus[type], "PLUS", "fe");
        legend->AddEntry(_pileup_none[type], "nominal", "fe");
        legend->AddEntry(_pileup_minus[type], "MINUS", "fe");
        legend->Draw();
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

TLegend *PileupSystematic::createLegend(const string &text)
{
    TLegend *legend = new TLegend( .68, .53, .88, .88);
    if (!text.empty())
        legend->SetHeader(text.c_str());

    legend->SetMargin(0.12);
    legend->SetTextSize(0.03);
    legend->SetFillColor(10);
    legend->SetBorderSize(0);

    return legend;
}
