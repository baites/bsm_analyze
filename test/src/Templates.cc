// Plot templates
//
// Created by Samvel Khalatyan, Dec 1, 2011
// Copyright 2011, All rights reserved

#include <cmath>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include <TCanvas.h>
#include <TFile.h>
#include <TFractionFitter.h>
#include <TH1.h>
#include <TH2.h>
#include <THStack.h>
#include <TLegend.h>
#include <TObject.h>
#include <TObjArray.h>
#include <TSystem.h>

#include "interface/ROOTStyles.h"
#include "interface/Templates.h"

using namespace std;
using namespace boost;

namespace fs = boost::filesystem;

Templates::Templates(const std::string &input_file,
        const std::string &theta_scale):
    _input_file(input_file),
    _mc_fraction(1.0),
    _mc_fraction_error(0.0),
    _mc_error(false),
    _qcd_fraction(1.0),
    _qcd_fraction_error(0.0),
    _qcd_type(QCD_NONE),
    _pull_plots(0.0),
    _ks_chi2(false),
    _log_scale(false)
{
    if (!theta_scale.empty())
    {
        _theta_scale.load(theta_scale);

        cout << "Loaded theta scales: " << endl;
        cout << _theta_scale << endl;
    }

    setTDRStyle();
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

string Templates::input_file() const
{
    return _input_file;
}

void Templates::load()
{
    FileStat_t buf;
    for(Input input(Input::QCD_BC_PT20_30), end(Input::UNKNOWN);
            end > input;
            ++input)
    {
        string filename = folder(input) + "/" + input_file();
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
    if (_qcd_type == QCD_FROM_DATA)
        normalize();

    for(Template hist_template(Template::MET), end(Template::HTOP_MT);
    //for(Template hist_template(Template::TTBAR_MASS), end(Template::TTBAR_MASS);
            end >= hist_template;
            ++hist_template)
    {
        plot(hist_template);
    }

    // 2D plot
    //
    for(Template hist_template(Template::DPHI_ELECTRON_VS_MET),
                end(Template::DPHI_JET_VS_MET_BEFORE_TRICUT);
            end >= hist_template;
            ++hist_template)
    {
        plot2D(hist_template);
    }
}

// Privates
//
void Templates::loadHistograms(TFile *file, const Input &input)
{
    for(Template plot(Template::MET), end(Template::DPHI_JET_VS_MET_BEFORE_TRICUT);
            end >= plot;
            ++plot)
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

        float lumi_scale = scale(histogram, input);
        if (Template::TTBAR_MASS == plot)
            cout << input.repr() << " lumi_scale: " << lumi_scale << endl;

        Templates::style(histogram, input);

        // store plot
        //
        input_plots[input] = histogram;
    }
}

void Templates::plot(const Template &plot)
{
    if (plot == Template::MET_QCD
            || plot == Template::MET_QCD_NOWEIGHT)
        return;

    const InputPlots &input_plots = _plots[plot];

    Channels channel;
    channel[Channel::DATA] = get(input_plots,
            Input::RERECO_2011A_MAY10,
            Input::PROMPT_2011B_V1);

    channel[Channel::STOP] = get(input_plots, Input::STOP_S, Input::SATOP_TW);

    TH1 *h = get(input_plots, Input::TTJETS);
    if (h)
        channel[Channel::TTBAR] = h;
    else
    {
        h = get(input_plots, Input::TTJETS_POWHEG);
        channel[Channel::TTBAR_POWHEG] = h;
    }

    channel[Channel::WJETS] = get(input_plots, Input::WJETS);
    channel[Channel::ZJETS] = get(input_plots, Input::ZJETS); 

    if (_qcd_type == QCD_FROM_DATA)
    {
        h = get(input_plots, Input::QCD_FROM_DATA);
        channel[Channel::QCD] = h;
    }
    else if (_qcd_type == QCD_FROM_MC)
    {
        h = get(input_plots, Input::QCD);
        channel[Channel::QCD] = h;
    }

    channel[Channel::ZPRIME1000] = get(input_plots, Input::ZPRIME1000);
    channel[Channel::ZPRIME2000] = get(input_plots, Input::ZPRIME2000);

    if (Template::TTBAR_MASS == plot)
        channel[Channel::ZPRIME3000] = get(input_plots, Input::ZPRIME3000);

    TCanvas *canvas = draw(plot, channel);
    canvas->SaveAs(("template_" + plot.repr() + (_log_scale ? "_log" : "")
                + ".png").c_str());
    canvas->SaveAs(("template_" + plot.repr() + (_log_scale ? "_log" : "") 
                + ".pdf").c_str());
}

void Templates::plot2D(const Template &plot)
{
    const InputPlots &input_plots = _plots[plot];

    Channels channel;
    channel[Channel::DATA] = get(input_plots,
            Input::RERECO_2011A_MAY10,
            Input::PROMPT_2011B_V1);

    TCanvas *canvas = draw2D(plot, channel);
    canvas->SaveAs(("template_" + plot.repr()
                + "_" + channel.begin()->first.repr()
                + ".png").c_str());
    canvas->SaveAs(("template_" + plot.repr()
                + "_" + channel.begin()->first.repr()
                + ".pdf").c_str());

    channel.clear();
    channel[Channel::QCD] = get(input_plots,
            Input::QCD_BC_PT20_30,
            Input::QCD_EM_PT80_170);

    canvas = draw2D(plot, channel);
    canvas->SaveAs(("template_" + plot.repr()
                + "_" + channel.begin()->first.repr()
                + ".png").c_str());
    canvas = draw2D(plot, channel);
    canvas->SaveAs(("template_" + plot.repr()
                + "_" + channel.begin()->first.repr()
                + ".pdf").c_str());

    channel.clear();
    channel[Channel::ZPRIME1000] = get(input_plots, Input::ZPRIME1000);

    canvas = draw2D(plot, channel);

    canvas->SaveAs(("template_" + plot.repr()
                + "_" + channel.begin()->first.repr()
                + ".png").c_str());
    canvas->SaveAs(("template_" + plot.repr()
                + "_" + channel.begin()->first.repr()
                + ".pdf").c_str());

    channel.clear();
    channel[Channel::ZPRIME2000] = get(input_plots, Input::ZPRIME2000);

    canvas = draw2D(plot, channel);

    canvas->SaveAs(("template_" + plot.repr()
                + "_" + channel.begin()->first.repr()
                + ".png").c_str());
    canvas->SaveAs(("template_" + plot.repr()
                + "_" + channel.begin()->first.repr()
                + ".pdf").c_str());

    return;
}

TCanvas *Templates::draw(const Template &plot, Channels &channels)
{
    // Create Canvas, assign name and prepare pads
    //
    ostringstream name;
    name << "canvas" << _heap.size();
    TCanvas *canvas = new TCanvas(name.str().c_str(), "", 800, 700);
    _heap.push_back(canvas);

    if (_pull_plots)
    {
        canvas->Divide(1, 2);
        TVirtualPad *pad = canvas->cd(1);
        pad->SetPad(0, 0.25, 1, 1);

        pad = canvas->cd(2);
        pad->SetPad(0, 0, 1, 0.25);
        pad->SetTopMargin(10);
        pad->SetRightMargin(10);
        pad->SetGrid();
    }

    {
        TVirtualPad *pad = canvas->cd(1);
        pad->SetTopMargin(10);
        pad->SetRightMargin(10);

        if (_log_scale)
            pad->SetLogy();
    }

    Scales scales = (Template::CUTFLOW == plot)
        ? getCutflowScales(channels)
        : getScales(channels);

    // print the scales for mttbar
    //
    if (plot == Template::TTBAR_MASS)
    {
        cout << "Scales for mttbar" << endl;
        cout << scales << endl;
    }

    if (plot == Template::CUTFLOW)
    {
        cout << "Scales for cutflow" << endl;
        cout << scales << endl;
    }

    // Scale all templates
    //
    Channels scaled_channels;
    for(Channels::const_iterator channel = channels.begin();
            channels.end() != channel;
            ++channel)
    {
        if (!channel->second)
            continue;

        float scale = 1;
        switch(channel->first.type())
        {
            case Channel::QCD: scale = _theta_scale.qcd; break;
            case Channel::STOP: scale = _theta_scale.stop; break;
            case Channel::TTBAR_POWHEG: // fall through
            case Channel::TTBAR: scale = _theta_scale.ttjets; break;
            case Channel::WJETS: scale = _theta_scale.wjets; break;
            case Channel::ZJETS: scale = _theta_scale.zjets; break;

            // Skip all other channels
            //
            default: continue;
        }

        scale *= (Channel::QCD == channel->first) ? scales.qcd : scales.mc;

        if (Template::TTBAR_MASS == plot)
            cout << "scale " << channel->first.repr() << ": " << scale << endl;

        TH1 *h = dynamic_cast<TH1 *>(channel->second->Clone());
        _heap.push_back(h);

        h->SetDirectory(0);
        h->Scale(scale);
        rebin(h, plot);

        scaled_channels[channel->first] = h;
    }

    // Stack all histograms: MC + QCD
    //
    THStack *stack = new THStack();
    _heap.push_back(stack);

    TLegend *legend = createLegend();

    // Calculate MC sum with correct scales
    //
    TH1 *mc_sum = 0;
    for(Channels::const_iterator channel = scaled_channels.begin();
            scaled_channels.end() != channel;
            ++channel)
    {
        legend->AddEntry(channel->second,
            static_cast<string>(channel->first).c_str(),
            "fe");
            
        stack->Add(channel->second);

        if (Channel::QCD == channel->first)
            continue;

        if (mc_sum)
            mc_sum->Add(channel->second);
        else
        {
            mc_sum = dynamic_cast<TH1 *>(channel->second->Clone());
            _heap.push_back(mc_sum);

            mc_sum->SetDirectory(0);
        }
    }

    if (mc_sum)
    {
        // Add Trigger and Luminositry errors: only applicable to MC
        //
        for(int bin = 1, bins = mc_sum->GetNbinsX(); bins >= bin; ++bin)
        {
            mc_sum->SetBinError(bin, sqrt(pow(mc_sum->GetBinError(bin), 2)
                        + pow(mc_sum->GetBinContent(bin) * 0.045, 2)
                        + pow(mc_sum->GetBinContent(bin) * 0.04, 2)));
        }

        // Add QCD
        //
        mc_sum->Add(scaled_channels[Channel::QCD]);

        TH1 *data = dynamic_cast<TH1 *>(channels[Channel::DATA]->Clone());
        if (data)
        {
            _heap.push_back(data);
            data->SetDirectory(0);

            rebin(data, plot);

            data->GetXaxis()->SetLabelSize(0.04);
            data->GetXaxis()->SetTitleOffset(1.1);
            data->GetYaxis()->SetLabelSize(0.04);
            data->GetYaxis()->SetTitleOffset(1.7);
            data->SetMarkerStyle(20);
            data->SetMarkerSize(1);
            setYaxisTitle(data, plot);

            mc_sum->SetMarkerSize(0);
            mc_sum->SetLineColor(1);
            mc_sum->SetLineWidth(2);
            mc_sum->SetFillColor(1);
            mc_sum->SetFillStyle(3004);

            data->Draw("9 axis");
            stack->Draw("9 hist same");

            int mc_max_bin = mc_sum->GetMaximumBin();
            int data_max_bin = data->GetMaximumBin();
            data->GetYaxis()->SetRangeUser(0, 1.2 * max(mc_sum->GetBinContent(mc_max_bin) + mc_sum->GetBinError(mc_max_bin), 
                data->GetBinContent(data_max_bin) + data->GetBinError(data_max_bin)));

            // Draw Zprimes: entries will be added to legend
            //
            for(Channel channel(Channel::ZPRIME1000);
                    Channel::ZPRIME3000 >= channel;
                    ++channel)
                drawSignal(plot, channels, channel, legend);

            if (_mc_error)
            {
                legend->AddEntry(mc_sum, "Uncertainty", "fe");

                mc_sum->Draw("9 e2 same");
            }

            legend->AddEntry(data,
                    static_cast<string>(Channel(Channel::DATA)).c_str(),
                    "lpe");

            data->Draw("9 same");

            legend->Draw("9");

            cmsLegend();

            if (_pull_plots)
            {
                TH1 *ratio = dynamic_cast<TH1*>(data->Clone());
                _heap.push_back(ratio);

                ratio->SetDirectory(0);

                // Background sustraction to data (special treatment for errors)
                // Note: include overflow bin
                //
                for (int i = 1, bins = data->GetNbinsX() + 1; i <= bins; ++i)
                {
                    float value = 0;
                    float error = 0;

                    if (mc_sum->GetBinContent(i) > 0.0)
                    {
                        value = (data->GetBinContent(i) - mc_sum->GetBinContent(i)) 
                            / mc_sum->GetBinContent(i);

                        error = sqrt(data->GetBinContent(i) 
                                + pow(mc_sum->GetBinError(i), 2))
                            / mc_sum->GetBinContent(i);
                    }

                    ratio->SetBinContent(i, value);
                    ratio->SetBinError(i, error);
                }

                float ymax =
                    min(static_cast<float>(ratio->GetBinContent(ratio->GetMaximumBin())),
                            _pull_plots);
                float ymin =
                    max(static_cast<float>(ratio->GetBinContent(ratio->GetMinimumBin())),
                        -_pull_plots);
         
                for (int i = 1, bins = ratio->GetNbinsX(); i <= bins; ++i)
                {
                    Float_t value = 1.05 * (ratio->GetBinContent(i) + ratio->GetBinError(i));
                    if (value > _pull_plots)
                        value = _pull_plots;

                    ymax = max(ymax, value);

                    value = 1.05 * (ratio->GetBinContent(i) - ratio->GetBinError(i));
                    if (value < -_pull_plots)
                        value = -_pull_plots;        

                    ymin = min(ymin, value);
                }

                ratio->GetXaxis()->SetTitle("");
                ratio->GetXaxis()->SetLabelSize(0.0);
                ratio->GetYaxis()->SetTitle("#frac{Data - BKGD}{BKGD}");
                ratio->GetYaxis()->SetTitleOffset(0.5);
                ratio->GetYaxis()->SetTitleSize(0.11);
                ratio->GetYaxis()->SetRangeUser(ymin, ymax);
                ratio->GetYaxis()->SetLabelSize(0.09);
                ratio->GetYaxis()->SetNdivisions(8);
                ratio->SetLineWidth(2);
         
                // draw bkg sustracted background
                //
                canvas->cd(2);

                ratio->Draw("9 e");
            }

            canvas->Update();

            if (plot == Template::TTBAR_MASS)
            {
                TFile *output = TFile::Open("mttbar.root", "recreate");
                if (!output->IsZombie())
                {
                    output->WriteObject(mc_sum, "mttbar_mc");
                    output->WriteObject(data, "mttbar_data");
                }
            }

            if (Template::CUTFLOW == plot)
            {
                saveCutflow(data, scaled_channels);

                TFile *output = TFile::Open("cutflow.root", "recreate");
                if (!output->IsZombie())
                {
                    output->WriteObject(mc_sum, "cutflow_mc");
                    output->WriteObject(data, "cutflow_data");
                }
            }
        } // end if data
    } // end if mc_sum

    return canvas;
}

void Templates::drawSignal(const Template &plot,
        Channels &channels,
        const Channel &channel,
        TLegend *legend)
{
    if (channels.end() != channels.find(channel)
            && channels[channel])
    {
        TH1 *h = dynamic_cast<TH1 *>(channels[channel]->Clone());
        _heap.push_back(h);

        h->SetDirectory(0);
        h->Scale(10);
        rebin(h, plot);

        legend->AddEntry(h, static_cast<string>(channel).c_str(), "fe");

        h->Draw("9 hist same");
    }
}

TCanvas *Templates::draw2D(const Template &plot, Channels &channels)
{
    ostringstream name;
    name << "canvas" << _heap.size();
    TCanvas *canvas = new TCanvas(name.str().c_str(), "", 800, 700);
    _heap.push_back(canvas);

    TVirtualPad *pad = canvas->cd(1);
    pad->SetRightMargin(10);
    pad->SetTopMargin(10);

    TH1 *h = channels.begin()->second;
    rebin2D(h, plot);
    h->GetXaxis()->SetLabelSize(0.04);
    h->GetXaxis()->SetTitleOffset(1.1);
    h->GetYaxis()->SetLabelSize(0.04);
    h->GetYaxis()->SetTitleOffset(1.7);
    h->SetContour(50);
    h->Draw("col 9");

    cmsLegend();

    canvas->Update();

    return canvas;
}

void Templates::normalize()
{
    Template plot(Template::MET_QCD);
    Template uwplot(Template::MET_QCD_NOWEIGHT);

    const InputPlots &input_plots = _plots[plot];
    const InputPlots &input_plots_noweight = _plots[uwplot];

    Channels channel, uwchannel;
    channel[Channel::DATA] = get(input_plots,
            Input::RERECO_2011A_MAY10,
            Input::PROMPT_2011B_V1);

    channel[Channel::STOP] = get(input_plots,
            Input::STOP_S,
            Input::SATOP_TW);

    channel[Channel::WJETS] = get(input_plots, Input::WJETS);
    channel[Channel::ZJETS] = get(input_plots, Input::ZJETS);

    TH1 *h = get(input_plots, Input::TTJETS);
    if (h)
        channel[Channel::TTBAR] = h;
    else
    {
        h = get(input_plots, Input::TTJETS_POWHEG);
        channel[Channel::TTBAR_POWHEG] = h;
    }

    channel[Channel::QCD] = get(input_plots, Input::QCD_FROM_DATA);

    uwchannel[Channel::STOP] = get(input_plots_noweight,
            Input::STOP_S,
            Input::SATOP_TW);

    uwchannel[Channel::WJETS] = get(input_plots_noweight, Input::WJETS);
    uwchannel[Channel::ZJETS] = get(input_plots_noweight, Input::ZJETS);

    h = get(input_plots_noweight, Input::TTJETS);
    if (h)
        uwchannel[Channel::TTBAR] = h;
    else
    {
        h = get(input_plots_noweight, Input::TTJETS_POWHEG);
        uwchannel[Channel::TTBAR_POWHEG] = h;
    }

    TCanvas *canvas = normalize(plot, channel, uwchannel);
    canvas->SaveAs(("template_" + plot.repr() + (_log_scale ? "_log" : "")
                + ".png").c_str());
    canvas->SaveAs(("template_" + plot.repr() + (_log_scale ? "_log" : "")
                + ".pdf").c_str());
}

TCanvas *Templates::normalize(const Template &plot, Channels &channels, Channels &uwchannels)
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
        data = dynamic_cast<TH1 *>(channels[Channel::DATA]->Clone());
        _heap.push_back(data);
    }

    TH1 *qcd = 0;
    if (channels.end() != channels.find(Channel::QCD)
        && channels[Channel::QCD])
    {
        qcd = dynamic_cast<TH1 *>(channels[Channel::QCD]->Clone());
        _heap.push_back(qcd);
    }
    
    TH1 * mc = 0;
    for(Channels::const_iterator channel = channels.begin();
            channels.end() != channel;
            ++channel)
    {
        if (!channel->second)
        {
            cerr << channel->first << " is not available" << endl;
            continue;
        }

        if (Channel::DATA == channel->first
            || Channel::QCD == channel->first)

            continue;
 
        if (mc)
            mc->Add(channel->second);
        else
        {
            mc = dynamic_cast<TH1 *>(channel->second->Clone());
            _heap.push_back(mc);
        }
    }

    TH1 *uwmc = 0;
    for(Channels::const_iterator channel = uwchannels.begin();
            uwchannels.end() != channel;
            ++channel)
    {
        if (!channel->second)
        {
            cerr << channel->first << " is not available" << endl;
            continue;
        }

        if (uwmc)
            uwmc->Add(channel->second);
        else
        {
            uwmc = dynamic_cast<TH1 *>(channel->second->Clone());
            _heap.push_back(uwmc);
        }
    }

    rebin(data, plot);
    rebin(qcd, plot);
    rebin(mc, plot);
    rebin(uwmc, plot);
 
    TH1 *wmc = dynamic_cast<TH1 *>(mc->Clone());
    _heap.push_back(wmc);

    wmc->Divide(uwmc);
    for(Int_t i=1; i<=wmc->GetNbinsX(); ++i)
        if (wmc->GetBinContent(i) <= 0.0) wmc->SetBinContent(i,1.0);

    TObjArray *templates = new TObjArray(2);
    _heap.push_back(templates);

    templates->Add(uwmc);
    templates->Add(qcd);
    
    TFractionFitter * fitter = new TFractionFitter(data, templates);
    _heap.push_back(fitter);

    fitter->SetWeight(0,wmc);
    Int_t status = fitter->Fit();

    if (status == 0)
    {   
        fitter->GetResult(0, _mc_fraction, _mc_fraction_error);
        fitter->GetResult(1, _qcd_fraction, _qcd_fraction_error); 

        cout << "MC  fraction: " << _mc_fraction << " +- " << _mc_fraction_error << endl;
        cout << "QCD fraction: " << _qcd_fraction << " +- " << _qcd_fraction_error << endl;

        ostringstream mclabel, qcdlabel;
        mclabel.precision(1);
        mclabel << fixed << "MC (" << (100*_mc_fraction) << "%)";
        qcdlabel.precision(1);
        qcdlabel << fixed << static_cast<string>(Channel(Channel::QCD))
            << " (" << (100*_qcd_fraction) << "%)";
        
        float mc_scale = _mc_fraction*data->Integral()/mc->Integral();
        float qcd_scale = _qcd_fraction*data->Integral()/qcd->Integral();

        data->GetXaxis()->SetLabelSize(0.04);
        data->GetXaxis()->SetTitleOffset(1.1);
        data->GetYaxis()->SetLabelSize(0.04);
        data->GetYaxis()->SetTitleOffset(1.7);
        data->SetMarkerStyle(20);
        data->SetMarkerSize(1);
        setYaxisTitle(data, plot);

        TH1 * result = (TH1*) fitter->GetPlot();
        result->SetFillStyle(0);
        result->SetLineWidth(2);

        float maxy = max(
            data->GetBinContent(data->GetMaximumBin()),
            result->GetBinContent(result->GetMaximumBin())
        );
        data->GetYaxis()->SetRangeUser(0, maxy * 1.2);
        data->Draw("9");
        result->Draw("9 same");
        
        qcd->SetFillStyle(0);
        qcd->SetLineWidth(2);
        qcd->SetLineColor(kBlue);
        qcd->SetLineStyle(2);
        qcd->Scale(qcd_scale);
        qcd->Draw("9 histsame");
        
        mc->SetFillStyle(0);
        mc->SetLineWidth(2);
        mc->SetLineColor(kRed);
        mc->SetLineStyle(3);
        mc->Scale(mc_scale);
        mc->Draw("9 histsame");
        
        data->Draw("9 sameEp");

        cmsLegend();
        
        TLegend *legend = createLegend("",true);

        legend->AddEntry(data, "Data", "pe");
        legend->AddEntry(result, "Fit", "l");
        legend->AddEntry(mc, mclabel.str().c_str(), "l");
        legend->AddEntry(qcd, qcdlabel.str().c_str(), "l");
        legend->Draw();
    }

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

TLegend *Templates::createLegend(const string &text, bool left)
{
    TLegend *legend = 0;

    if (left)
        legend = new TLegend( .57, .60, .79, .88);
    else
        legend = new TLegend( .67, .60, .89, .88);
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

    if (input < Input::RERECO_2011A_MAY10
            || Input::QCD == input)
        h->SetFillColor(h->GetLineColor());
}

void Templates::cmsLegend()
{
    TLegend *legend = new TLegend(.18, .91, .50, .96);
    _heap.push_back(legend);

    legend->SetTextAlign(12);
    legend->SetTextSize(0.12);

    ostringstream title;
    title.precision(2);
    title << fixed << "L = " << (luminosity()/1000) << " fb^{-1}, e+jets";
    legend->SetHeader(title.str().c_str());

    legend->SetMargin(0.12);
    legend->SetTextSize(0.03);
    legend->SetFillColor(10);
    legend->SetBorderSize(0);
    
    legend->Draw();

    legend = new TLegend(.55, .91, .90, .96);
    _heap.push_back(legend);

    legend->SetTextAlign(32);
    legend->SetTextSize(0.12);

    title.str("");
    title << "CMS Preliminary  #sqrt{s} = 7 TeV";
    legend->SetHeader(title.str().c_str());

    legend->SetMargin(0.12);
    legend->SetTextSize(0.03);
    legend->SetFillColor(10);
    legend->SetBorderSize(0);
    
    legend->Draw();
}

void Templates::histTestLegend(const float &ks, const float &chi2)
{
    TLegend *legend = new TLegend(.18, .84, .40, .89);
    _heap.push_back(legend);

    legend->SetTextAlign(12);

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

void Templates::rebin(TH1 *histogram, const Template &plot) const
{
    const int merge_bins = rebin(plot);
    if (1 < merge_bins)
        histogram->Rebin(merge_bins);
}

void Templates::rebin2D(TH1 *histogram, const Template &plot) const
{
    TH2 *h = dynamic_cast<TH2 *>(histogram);

    const Rebin merge_bins = rebin2D(plot);
    if (1 < merge_bins.first)
        h->RebinX(merge_bins.first);
    if (1 < merge_bins.second)
        h->RebinY(merge_bins.second);
}

int Templates::rebin(const Template &plot) const
{
    switch(plot.type())
    {
        case Template::MET: return 25;
        case Template::MET_QCD: return 5;
        case Template::HTALL: return 25;
        case Template::HTLEP: return 25;
        case Template::HTLEP_BEFORE_HTLEP: return 5;
        case Template::HTLEP_AFTER_HTLEP: return 25;
        case Template::NPV: return 1;
        case Template::NPV_NO_PU: return 1;
        case Template::NJET: return 1;
        case Template::TTBAR_MASS: return 100;
        case Template::TTBAR_PT: return 25;
        case Template::WLEP_MT: return 25;
        case Template::WLEP_MASS: return 25;
        case Template::JET1_PT: return 25;
        case Template::JET1_ETA: return 50;
        case Template::JET2_PT: return 25;
        case Template::JET2_ETA: return 50;
        case Template::JET3_PT: return 25;
        case Template::JET3_ETA: return 50;
        case Template::ELECTRON_PT: return 25;
        case Template::ELECTRON_ETA: return 50;
        case Template::ELECTRON_PT_BEFORE_TRICUT: return 25;
        case Template::ELECTRON_ETA_BEFORE_TRICUT: return 50;
        case Template::LTOP_PT: return 25;
        case Template::LTOP_ETA: return 50;
        case Template::LTOP_MASS: return 25;
        case Template::LTOP_MT: return 25;
        case Template::HTOP_NJETS: return 1;
        case Template::HTOP_DELTA_R: return 25;
        case Template::HTOP_PT: return 25;
        case Template::HTOP_ETA: return 50;
        case Template::HTOP_MASS: return 25;
        case Template::HTOP_MT: return 25;

        default: return 1;
    }
}

Templates::Rebin Templates::rebin2D(const Template &plot) const
{
    switch(plot.type())
    {
        case Template::DPHI_ELECTRON_VS_MET: return make_pair(25, 10);
        case Template::DPHI_JET_VS_MET: return make_pair(25, 10);
        case Template::DPHI_ELECTRON_VS_MET_BEFORE_TRICUT: return make_pair(25, 10);
        case Template::DPHI_JET_VS_MET_BEFORE_TRICUT: return make_pair(25, 10);

        default: return make_pair(1, 1);
    }
}

void Templates::setYaxisTitle(TH1 *h, const Template &plot)
{
    ostringstream title;

    title << "event yield";
    
    const float bin_width = h->GetBinWidth(1);
    cout << plot << " bin width: " << bin_width << endl;
    if (bin_width < 1.0)
        title.precision(2);
    else if (bin_width < 10)
        title.precision(1);
    else
        title.precision(0);

    title << " / " << fixed << bin_width << " " << plot.unit();

    h->GetYaxis()->SetTitle(title.str().c_str());
}

Templates::Scales Templates::getScales(Channels &channels)
{
    Scales scales;

    if (channels.end() != channels.find(Channel::DATA)
        && channels[Channel::DATA]
        && _qcd_type == QCD_FROM_DATA)
    {
        TH1 *data = channels[Channel::DATA];

        // Add all MC samples (no QCD)
        //
        TH1 *mc = 0;
        for(Channels::const_iterator channel = channels.begin();
                channels.end() != channel;
                ++channel)
        {
            if (!channel->second)
                continue;

            if (Channel::DATA == channel->first
                || Channel::QCD == channel->first
                || Channel::ZPRIME1000 == channel->first
                || Channel::ZPRIME1500 == channel->first
                || Channel::ZPRIME2000 == channel->first
                || Channel::ZPRIME3000 == channel->first)

                continue;
     
            if (!mc)
                mc = dynamic_cast<TH1 *>(channel->second->Clone());
            else
                mc->Add(channel->second);
        }

        if (mc)
        {
            // mc and qcd scales computed from the mc fraction
            //
            scales.mc = _mc_fraction * data->Integral() / mc->Integral();

            if (channels.end() != channels.find(Channel::QCD)
                    && channels[Channel::QCD])
                scales.qcd = _qcd_fraction * data->Integral()
                    / channels[Channel::QCD]->Integral();

            mc->SetDirectory(0);
            delete mc;
        }
    }

    return scales;
}

Templates::Scales Templates::getCutflowScales(Channels &channels)
{
    Scales scales;

    if (channels.end() != channels.find(Channel::DATA)
        && channels[Channel::DATA]
        && _qcd_type == QCD_FROM_DATA)
    {
        TH1 *data = channels[Channel::DATA];

        // Add all MC samples (no QCD)
        //
        TH1 *mc = 0;
        for(Channels::const_iterator channel = channels.begin();
                channels.end() != channel;
                ++channel)
        {
            if (!channel->second)
                continue;

            if (Channel::DATA == channel->first
                || Channel::QCD == channel->first
                || Channel::ZPRIME1000 == channel->first
                || Channel::ZPRIME1500 == channel->first
                || Channel::ZPRIME2000 == channel->first
                || Channel::ZPRIME3000 == channel->first)

                continue;
     
            if (!mc)
                mc = dynamic_cast<TH1 *>(channel->second->Clone());
            else
                mc->Add(channel->second);
        }

        if (mc)
        {
            // mc and qcd scales computed from the mc fraction
            //
            int bins = data->GetXaxis()->GetNbins();
            scales.mc = _mc_fraction * data->GetBinContent(bins)
                / mc->GetBinContent(bins);

            if (channels.end() != channels.find(Channel::QCD)
                    && channels[Channel::QCD])
                scales.qcd = _qcd_fraction * data->GetBinContent(bins)
                    / channels[Channel::QCD]->GetBinContent(bins);

            mc->SetDirectory(0);
            delete mc;
        }
    }

    return scales;
}

void Templates::saveCutflow(const TH1 *data, const Channels &bg) const
{
    ofstream cutflow("cutflow.txt");
    cutflow << "data " << getCutflow(data) << endl;

    for(Channels::const_iterator channel = bg.begin();
            bg.end() != channel;
            ++channel)
    {
        cutflow << channel->first.repr() << " "
            << getCutflow(channel->second) << endl;
    }
}

string Templates::getCutflow(const TH1 *h) const
{
    ostringstream out;
    for(int bin = 1, bins = h->GetXaxis()->GetNbins() + 1;
            bins > bin;
            ++bin)

        out << " " << fixed << h->GetBinContent(bin)
            << "+" << h->GetBinError(bin);

    return out.str();
}

ostream &operator <<(ostream &out, const Templates::Scales &scales)
{
    out << " mc: " << scales.mc << endl;
    out << "qcd: " << scales.qcd;

    return out;
}
