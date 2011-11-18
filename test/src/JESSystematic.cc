#include "interface/JESSystematic.h"

using namespace boost;

using namespace std;

void JESSystematic::draw()
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
        style(histogram, input, systematic);

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
        _jes_plus[type]->SetFillStyle(30004);
        _jes_plus[type]->Draw("hist");

        _jes_none[type]->Draw("hist same");

        _jes_minus[type]->SetFillStyle(3005);
        _jes_minus[type]->Draw("hist same");
    }
    else
    {
        cerr << "can not plot input: " << Input(type) << endl;
    }
}
