// Plot templates
//
// Created by Samvel Khalatyan, Dec 1, 2011
// Copyright 2011, All rights reserved

#include <cmath>
#include <fstream>
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
    _mc_scale(1.0),
    _mc_scale_error(0.0),
    _qcd_scale(1.0),
    _qcd_scale_error(0.0)
{
    if (!theta_scale.empty())
    {
        if (!fs::exists(theta_scale))
        {
            cerr << "theta scale file does not exist: " << theta_scale << endl;
        }
        else
        {
            ifstream in(theta_scale.c_str());
            if (!in.is_open())
            {
                cerr << "failed to open theta scale file: " << theta_scale
                    << endl;
            }
            else
            {
                char buf[512];
                while(in.getline(buf, 512))
                {
                    string input(buf);

                    smatch matches;
                    regex pattern("^(wjets|zjets|singletop|ttbar|eleqcd):\\s+(\\d+\\.\\d+)$", regex_constants::icase | regex_constants::perl);
                    if (!regex_match(input, matches, pattern))
                    {
                        cerr << "didn't understand line: " << buf << endl;
                    }
                    else
                    {
                        float scale = lexical_cast<float>(matches[2]);
                        if ("wjets" == matches[1])
                        {
                            _theta_scale.wjets = scale;
                        }
                        else if ("zjets" == matches[1])
                        {
                            _theta_scale.zjets = scale;
                        }
                        else if ("singletop" == matches[1])
                        {
                            _theta_scale.stop = scale;
                        }
                        else if ("ttbar" == matches[1])
                        {
                            _theta_scale.ttjets = scale;
                        }
                        else if ("eleqcd" == matches[1])
                        {
                            _theta_scale.qcd = scale;
                        }
                    }
                }

                cout << "Read Theta scales" << endl << _theta_scale << endl;
            }
        }
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
    normalize();

    for(Template hist_template(Template::MET), end(Template::HTOP_MT);
    //for(Template hist_template(Template::MET), end(Template::WLEP_MT);
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

        scale(histogram, input);
        Templates::style(histogram, input);

        if (Template::DPHI_ELECTRON_VS_MET > plot)
        {
            const int merge_bins = rebin(plot);
            if (1 < merge_bins)
                histogram->Rebin(merge_bins);
        }
        else
        {
            const Rebin merge_bins = rebin2D(plot);

            TH2 *h = dynamic_cast<TH2 *>(histogram);

            if (1 < merge_bins.first)
                h->RebinX(merge_bins.first);

            if (1 < merge_bins.second)
                h->RebinY(merge_bins.second);
        }

        // store plot
        //
        input_plots[input] = histogram;
    }
}

void Templates::plot(const Template &plot)
{
    if (
        plot == Template::MET_QCD ||
        plot == Template::MET_QCD_NOWEIGHT
    )
        return;

    const InputPlots &input_plots = _plots[plot];

    Channels channel;
    channel[Channel::DATA] = get(input_plots,
            Input::RERECO_2011A_MAY10,
            Input::PROMPT_2011B_V1);

    TH1 *h = get(input_plots,
            Input::STOP_S,
            Input::SATOP_TW);
    if (h && _theta_scale.stop)
        h->Scale(_theta_scale.stop);
    channel[Channel::STOP] = h;

    h = get(input_plots, Input::TTJETS);
    if (h && _theta_scale.ttjets)
        h->Scale(_theta_scale.ttjets);
    channel[Channel::TTBAR] = h;

    h = get(input_plots, Input::WJETS);;
    if (h && _theta_scale.wjets)
        h->Scale(_theta_scale.wjets);
    channel[Channel::WJETS] = h;

    h = get(input_plots, Input::ZJETS);
    if (h && _theta_scale.zjets)
        h->Scale(_theta_scale.zjets);
    channel[Channel::ZJETS] = h; 

    h = get(input_plots, Input::QCD);
    if (h && _theta_scale.qcd)
        h->Scale(_theta_scale.qcd);
    channel[Channel::QCD] = h;

    channel[Channel::QCD] = get(input_plots, Input::QCD_FROM_DATA);

    TCanvas *canvas = draw(plot, channel);
    canvas->SaveAs(("template_" + plot.repr() + ".pdf").c_str());
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
                + ".pdf").c_str());

    channel.clear();
    channel[Channel::QCD] = get(input_plots,
            Input::QCD_BC_PT20_30,
            Input::QCD_EM_PT80_170);

    canvas = draw2D(plot, channel);
    canvas->SaveAs(("template_" + plot.repr()
                + "_" + channel.begin()->first.repr()
                + ".pdf").c_str());

    channel.clear();
    channel[Channel::ZPRIME1000] = get(input_plots, Input::ZPRIME1000);

    canvas = draw2D(plot, channel);
    canvas->SaveAs(("template_" + plot.repr()
                + "_" + channel.begin()->first.repr()
                + ".pdf").c_str());

    return;
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
        
        if (Channel::QCD == channel->first)
        {
            channel->second->Scale(_qcd_scale);
        }
        else
        {
            channel->second->Scale(_mc_scale);
        }

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

    // Add systematics
    //
    for(int bin = 1, bins = mc_sigma->GetNbinsX(); bins >= bin; ++bin)
    {
        mc_sigma->SetBinError(bin, sqrt(pow(mc_sigma->GetBinError(bin), 2)
                    + pow(mc_sigma->GetBinContent(bin) * 0.045, 2)
                    + pow(mc_sigma->GetBinContent(bin) * 0.04, 2)));
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
    channel[Channel::TTBAR] = get(input_plots, Input::TTJETS);

    channel[Channel::QCD] = get(input_plots, Input::QCD_FROM_DATA);

    uwchannel[Channel::STOP] = get(input_plots_noweight,
            Input::STOP_S,
            Input::SATOP_TW);

    uwchannel[Channel::WJETS] = get(input_plots_noweight, Input::WJETS);
    uwchannel[Channel::ZJETS] = get(input_plots_noweight, Input::ZJETS);
    uwchannel[Channel::TTBAR] = get(input_plots_noweight, Input::TTJETS);

    TCanvas *canvas = normalize(plot, channel, uwchannel);
    canvas->SaveAs(("template_" + plot.repr() + ".pdf").c_str());
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
    if (
        channels.end() != channels.find(Channel::DATA) && 
        channels[Channel::DATA]
    )
        data = channels[Channel::DATA];

    TH1 *qcd = 0;
    if (
        channels.end() != channels.find(Channel::QCD) && 
        channels[Channel::QCD]
    )
        qcd = channels[Channel::QCD];
    
    TH1 * mc = 0;
    bool isclone = false;
    
    for(Channels::const_iterator channel = channels.begin();
            channels.end() != channel;
            ++channel)
    {
        if (!channel->second)
        {
            cerr << channel->first << " is not available" << endl;
            continue;
        }

        if (
            Channel::DATA == channel->first || 
            Channel::QCD == channel->first
        )
        {
            continue;
        }            
 
        if (!isclone)
        {
            mc = dynamic_cast<TH1 *>(channel->second->Clone());
            isclone = true;
            continue;
        }
        
        mc->Add(channel->second);
    }

    TH1 *uwmc = 0;
    isclone = false;

    for(Channels::const_iterator channel = uwchannels.begin();
            uwchannels.end() != channel;
            ++channel)
    {
        if (!channel->second)
        {
            cerr << channel->first << " is not available" << endl;
            continue;
        }

        if (!isclone)
        {
            uwmc = dynamic_cast<TH1 *>(channel->second->Clone());
            isclone = true;
            continue;
        }

        uwmc->Add(channel->second);
    }

    TH1 *wmc = dynamic_cast<TH1 *>(mc->Clone());
    wmc->Divide(uwmc);
    for(Int_t i=1; i<=wmc->GetNbinsX(); ++i)
        if (wmc->GetBinContent(i) <= 0.0) wmc->SetBinContent(i,1.0);

    TObjArray *templates = new TObjArray(2);
    templates->Add(uwmc);
    templates->Add(qcd);
    
    TFractionFitter * fitter = new TFractionFitter(data, templates);
    fitter->SetWeight(0,wmc);
    Int_t status = fitter->Fit();

    if (status == 0)
    {   
        fitter->GetResult(0, _mc_scale, _mc_scale_error);
        fitter->GetResult(1, _qcd_scale, _qcd_scale_error); 

        cout << "MC  fraction: " << _mc_scale << " +- " << _mc_scale_error << endl;
        cout << "QCD fraction: " << _qcd_scale << " +- " << _qcd_scale_error << endl;

        ostringstream mclabel, qcdlabel;
        mclabel.precision(1);
        mclabel << fixed << "MC (" << (100*_mc_scale) << "%)";
        qcdlabel.precision(1);
        qcdlabel << fixed << "QCD (" << (100*_qcd_scale) << "%)";
        
        _mc_scale = _mc_scale*data->Integral()/mc->Integral();
        _qcd_scale = _qcd_scale*data->Integral()/qcd->Integral();
       
        cout << "MC scale: " << _qcd_scale << endl; 
        cout << "QCD scale: " << _mc_scale << endl;

        data->GetXaxis()->SetLabelSize(0.04);
        data->GetXaxis()->SetTitleOffset(1.1);
        data->GetYaxis()->SetLabelSize(0.04);
        data->GetYaxis()->SetTitleOffset(1.7);
        data->SetMarkerStyle(20);
        data->SetMarkerSize(1);
        data->Rebin(rebin(Template::MET)/rebin(Template::MET_QCD));
        setYaxisTitle(data, plot);

        TH1 * result = (TH1*) fitter->GetPlot();
        result->SetFillStyle(0);
        result->SetLineWidth(2);
        result->Rebin(rebin(Template::MET)/rebin(Template::MET_QCD));

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
        qcd->Scale(_qcd_scale);
        qcd->Rebin(rebin(Template::MET)/rebin(Template::MET_QCD));       
        qcd->Draw("9 histsame");
        
        mc->SetFillStyle(0);
        mc->SetLineWidth(2);
        mc->SetLineColor(kRed);
        mc->SetLineStyle(3);
        mc->Scale(_mc_scale);
        mc->Rebin(rebin(Template::MET)/rebin(Template::MET_QCD));
        mc->Draw("9 histsame");
        
        data->Draw("9 sameEp");

        cmsLegend();
        
        TLegend *legend = createLegend();

        legend->AddEntry(data, "Data", "pe");
        legend->AddEntry(result, "Fit", "l");
        legend->AddEntry(mc, mclabel.str().c_str(), "l");
        legend->AddEntry(qcd, qcdlabel.str().c_str(), "l");
        legend->Draw();
    }
    
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
    TLegend *legend = new TLegend( .67, .65, .89, .88);
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
    TLegend *legend = new TLegend(.20, .91, .60, .95);
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
    TLegend *legend = new TLegend(.20, .91, .90, .95);
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
        case Template::MET: return 25;
        case Template::MET_QCD: return 5;
        case Template::MET_QCD_NOWEIGHT: return 5;
        case Template::HTLEP: return 25;
        case Template::NPV: return 1;
        case Template::NPV_NO_PU: return 1;
        case Template::NJET: return 1;
        case Template::TTBAR_MASS: return 200;
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
    title.precision(1);

    title << "event yield";
    if (1 < rebin(plot))
    {
       const float bin_width = h->GetBinWidth(1);
       cout << plot << " bin width: " << bin_width << endl;
       if (1 > bin_width)
           title.precision(2);

       title << " / " << fixed << bin_width << " " << plot.unit();
    }

    h->GetYaxis()->SetTitle(title.str().c_str());
}

ostream &operator <<(ostream &out, const Templates::ThetaScale &scale)
{
    return out << "wjets: " << scale.wjets << endl
        << "zjets: " << scale.zjets << endl
        << "stop: " << scale.stop << endl
        << "ttjets: " << scale.ttjets << endl
        << "qcd: " << scale.qcd;
}
