// Plot templates
//
// Created by Samvel Khalatyan, Dec 1, 2011
// Copyright 2011, All rights reserved

#include <sstream>

#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <THStack.h>
#include <TLegend.h>
#include <TObject.h>
#include <TSystem.h>

#include "interface/Templates.h"

using namespace std;

Templates::Templates()
{
}

Templates::~Templates()
{
    for(Heap::iterator obj = _heap.begin();
            _heap.end() != obj;
            ++obj)
    {
        delete *obj;
    }
}

void Templates::load()
{
    FileStat_t buf;
    for(Input input(Input::QCD_BC_PT20_30), end(Input::UNKNOWN);
            end > input;
            ++input)
    {
        string filename = folder(input) + "/output_signal_p250_hlt.root";
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

        loadHistograms(file, input);
    }
}

void Templates::draw()
{
    for(Template hist_template(Template::MET), end(Template::HTOP_MT);
            end >= hist_template;
            ++hist_template)
    {
        plot(hist_template);
    }
}

// Privates
//
void Templates::loadHistograms(TFile *file, const Input &input)
{
    for(Template plot(Template::MET), end(Template::HTOP_MT); end >= plot; ++plot)
    {
        InputPlots &input_plots = _plots[plot];

        // extract histogram
        //
        TObject *object = file->Get(static_cast<string>(plot).c_str());
        if (!object)
        {
            cerr << "failed to extract plot: " << plot
                << " from: " << file->GetName()<< endl;

            continue;
        }

        TH1 *histogram = dynamic_cast<TH1 *>(object->Clone());
        if (!histogram)
        {
            cerr << "extracted plot: " << plot
                << " is not TH1" << endl;

            continue;
        }

        scale(histogram, input);
        Templates::style(histogram, input);

        const int merge_bins = rebin(plot);
        if (1 < merge_bins)
            histogram->Rebin(merge_bins);

        // store plot
        //
        input_plots[input] = histogram;
    }
}

void Templates::plot(const Template &plot)
{
    const InputPlots &input_plots = _plots[plot];

    Channels channel;
    channel[Channel::DATA] = get(input_plots,
            Input::RERECO_2011A_MAY10,
            Input::PROMPT_2011B_V1);

    channel[Channel::STOP] = get(input_plots,
            Input::STOP_S,
            Input::SATOP_TW);

    channel[Channel::WJETS] = get(input_plots, Input::WJETS);
    channel[Channel::ZJETS] = get(input_plots, Input::ZJETS);
    channel[Channel::TTBAR] = get(input_plots, Input::TTJETS);

    TCanvas *canvas = draw(plot, channel);
    canvas->SaveAs(("template_" + plot.repr() + ".pdf").c_str());
}

TCanvas *Templates::draw(const Template &plot, Channels &channels)
{
    ostringstream name;
    name << "canvas" << _heap.size();
    TCanvas *canvas = new TCanvas(name.str().c_str(), "", 800, 700);
    _heap.push_back(canvas);

    TVirtualPad *pad = canvas->cd(1);
    pad->SetRightMargin(10);
    pad->SetTopMargin(10);

    TH1 *data = 0;
    if (channels.end() != channels.find(Channel::DATA)
            && channels[Channel::DATA])
    {
        data = channels[Channel::DATA];
        data->Draw("9");
    }

    THStack *stack = new THStack();
    _heap.push_back(stack);

    TLegend *legend = createLegend();

    for(Channels::const_iterator channel = channels.begin();
            channels.end() != channel;
            ++channel)
    {
        if (!channel->second)
        {
            cerr << channel->first << " is not available" << endl;
            continue;
        }

        if (Channel::DATA == channel->first)
        {
            continue;
        }
        else
            legend->AddEntry(channel->second,
                    static_cast<string>(channel->first).c_str(),
                    "fe");

        stack->Add(channel->second);
    }

    if (data)
        stack->Draw("9 hist same");
    else
        stack->Draw("9 hist");

    legend->Draw("9");

    TH1 *mc_sigma = dynamic_cast<TH1 *>(stack->GetHistogram()->Clone());
    _heap.push_back(mc_sigma);

    mc_sigma->Sumw2();

    // copy stack bin content to mc_sigma
    //
    TIter next(stack->GetHists());
    for(TObject *obj = next(); obj; obj = next())
    {
        TH1 *hist = dynamic_cast<TH1 *>(obj);
        if (!hist)
        {
            cout << "failed hist: " << obj->GetName() << " " << obj->GetTitle() << endl;

            continue;
        }

        mc_sigma->Add(hist);
    }

    // Adjust plot to max
    //
    {
        int mc_bin = mc_sigma->GetMaximumBin();
        float max_y = max(data->GetBinContent(data->GetMaximumBin()),
                mc_sigma->GetBinContent(mc_bin) + mc_sigma->GetBinError(mc_bin));

        data->GetYaxis()->SetRangeUser(0, max_y * 1.2);
    }

    mc_sigma->SetMarkerSize(0);
    mc_sigma->SetLineWidth(2);
    mc_sigma->SetFillColor(1);
    mc_sigma->SetFillStyle(3004);
    mc_sigma->Draw("9 e2 same");

    legend->AddEntry(mc_sigma, "Uncertainty", "fe");
    legend->AddEntry(data,
            static_cast<string>(Channel(Channel::DATA)).c_str(),
            "lpe");

    data->GetXaxis()->SetLabelSize(0.04);
    data->GetXaxis()->SetTitleOffset(1.1);
    data->GetYaxis()->SetLabelSize(0.04);
    data->GetYaxis()->SetTitleOffset(1.7);
    data->SetMarkerStyle(20);
    data->SetMarkerSize(1);

    setYaxisTitle(data, plot);
    data->Draw("9 same");

    if (data)
    {
        const float ks = data->KolmogorovTest(mc_sigma);
        const float chi2 = data->Chi2Test(mc_sigma);

        histTestLegend(ks, chi2);
    }

    cmsLegend();

    canvas->Update();

    return canvas;
}

TH1 *Templates::get(const InputPlots &plots,
        const Input::Type &from,
        const Input::Type &to)
{

    TH1 *hist = 0;

    if (Input::UNKNOWN == to)
    {
        if (plots.end() != plots.find(from))
            hist = plots.at(from);
    }
    else
    {
        for(Input input(from), end(to);
                end >= input;
                ++input)
        {
            if (plots.end() == plots.find(input))
                continue;

            if (hist)
                hist->Add(plots.at(input));
            else
                hist = plots.at(input);
        }
    }

    return hist;
}

TLegend *Templates::createLegend(const string &text)
{
    TLegend *legend = new TLegend( .67, .65, .9, .88);
    _heap.push_back(legend);

    if (!text.empty())
        legend->SetHeader(text.c_str());

    legend->SetMargin(0.12);
    legend->SetTextSize(0.03);
    legend->SetFillColor(10);
    legend->SetBorderSize(0);

    return legend;
}

void Templates::style(TH1 *h, const Input &input)
{
    h->GetYaxis()->SetTitleSize(0.04);

    h->GetXaxis()->SetTitleSize(0.04);
    ::style(h, input);

    if (input < Input::RERECO_2011A_MAY10)
        h->SetFillColor(h->GetLineColor());
}

void Templates::cmsLegend()
{
    TLegend *legend = new TLegend(.20, .9, .60, .95);
    _heap.push_back(legend);

    legend->SetTextAlign(12);

    ostringstream title;
    title.precision(2);
    title << fixed << "L = " << luminosity() / 1000 << " /fb";
    legend->SetHeader(title.str().c_str());

    legend->SetMargin(0.12);
    legend->SetTextSize(0.03);
    legend->SetFillColor(10);
    legend->SetBorderSize(0);
    
    legend->Draw();

    legend = new TLegend(.20, .83, .60, .88);
    _heap.push_back(legend);

    legend->SetTextAlign(12);

    title.str("");
    title << "#splitline{CMS Preliminary}{#sqrt{s}=7 TeV}";
    legend->SetHeader(title.str().c_str());

    legend->SetMargin(0.12);
    legend->SetTextSize(0.03);
    legend->SetFillColor(10);
    legend->SetBorderSize(0);
    
    legend->Draw();
}

void Templates::histTestLegend(const float &ks, const float &chi2)
{
    TLegend *legend = new TLegend(.20, .9, .90, .95);
    _heap.push_back(legend);

    legend->SetTextAlign(32);

    ostringstream title;
    title.precision(3);
    title << fixed << "KS p: " << ks << ", #chi^{2} p: " << chi2;
    legend->SetHeader(title.str().c_str());

    legend->SetMargin(0.12);
    legend->SetTextSize(0.03);
    legend->SetFillColor(10);
    legend->SetBorderSize(0);
    
    legend->Draw();
}

int Templates::rebin(const Template &plot) const
{
    switch(plot.type())
    {
        case Template::MET: return 16;
        case Template::HTLEP: return 20;
        case Template::NPV: return 1;
        case Template::NJET: return 1;
        case Template::TTBAR_MASS: return 100;
        case Template::TTBAR_PT: return 20;
        case Template::WLEP_MT: return 10;
        case Template::WLEP_MASS: return 10;
        case Template::JET1_PT: return 5;
        case Template::JET1_ETA: return 2;
        case Template::JET2_PT: return 5;
        case Template::JET2_ETA: return 2;
        case Template::JET3_PT: return 5;
        case Template::JET3_ETA: return 2;
        case Template::ELECTRON_PT: return 5;
        case Template::ELECTRON_ETA: return 2;
        case Template::LTOP_PT: return 20;
        case Template::LTOP_ETA: return 2;
        case Template::LTOP_MASS: return 10;
        case Template::LTOP_MT: return 20;
        case Template::HTOP_PT: return 20;
        case Template::HTOP_ETA: return 2;
        case Template::HTOP_MASS: return 10;
        case Template::HTOP_MT: return 20;

        default: return 1;
    }
}

void Templates::setYaxisTitle(TH1 *h, const Template &plot)
{
    ostringstream title;
    title.precision(0);

    title << "event yield";
    const float width = h->GetBinWidth(1);
    if (1 < width)
       title << " / " << fixed << h->GetBinWidth(1) << " " << plot.unit();

    h->GetYaxis()->SetTitle(title.str().c_str());
}
