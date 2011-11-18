#include <sstream>

#include <TCanvas.h>
#include <TPad.h>

#include "interface/JESSystematic.h"

using namespace boost;

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

JESSystematic::~JESSystematic()
{
    for(vector<TObject *>::iterator obj = _heap.begin();
            _heap.end() != obj;
            ++obj)
    {
        delete *obj;
    }
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

void JESSystematic::plot(const Input::Type &type)
{
    if (_jes_none.end() != _jes_none.find(type)
            && _jes_plus.end() != _jes_plus.find(type)
            && _jes_minus.end() != _jes_minus.find(type))
    {
        ostringstream name;
        name << "canvas" << _heap.size();
        TCanvas *canvas = new TCanvas(name.str().c_str(), "JES", 600, 700);
        _heap.push_back(canvas);

        name.str("");
        name << "pad" << _heap.size();
        TPad *plot = new TPad(name.str().c_str(), "pad", 0, 0.3, 1, 1);
        _heap.push_back(plot);

        plot->SetBottomMargin(3);
        plot->Draw();
        plot->cd();

        TH1 *plus = _jes_plus[type];
        plus->Draw("hist");

        TH1 *none = _jes_none[type];
        none->Draw("hist same");

        TH1 *minus = _jes_minus[type];
        minus->Draw("hist same");

        string input_name = static_cast<string>(Input(type));
        TLegend *legend = createLegend(input_name);
        legend->AddEntry(plus, "PLUS", "fe");
        legend->AddEntry(none, "nominal", "fe");
        legend->AddEntry(minus, "MINUS", "fe");
        legend->Draw();

        canvas->cd();

        name.str("");
        name << "pad" << _heap.size();
        TPad *ratio = new TPad(name.str().c_str(), "pad", 0, 0, 1, 0.3);
        _heap.push_back(ratio);

        ratio->SetTopMargin(3);
        ratio->Draw();
        ratio->cd();

        TH1 *plus_ratio = dynamic_cast<TH1 *>(plus->Clone());
        plus_ratio->Divide(none);
        plus_ratio->Draw("hist");

        TH1 *none_ratio = dynamic_cast<TH1 *>(none->Clone());
        none_ratio->Divide(none);
        none_ratio->Draw("hist same");

        TH1 *minus_ratio = dynamic_cast<TH1 *>(minus->Clone());
        minus_ratio->Divide(none);
        minus_ratio->Draw("hist same");

        canvas->cd();
        canvas->Update();

        canvas->SaveAs(("jes_" + Input(type).repr() + ".pdf").c_str());
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

TLegend *JESSystematic::createLegend(const string &text)
{
    TLegend *legend = new TLegend( .68, .53, .88, .88);
    _heap.push_back(legend);
    if (!text.empty())
        legend->SetHeader(text.c_str());

    legend->SetMargin(0.12);
    legend->SetTextSize(0.03);
    legend->SetFillColor(10);
    legend->SetBorderSize(0);

    return legend;
}
