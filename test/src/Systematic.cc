// Draw systematics with ratio plot
//
// Created by Samvel Khalatyan, Nov 18, 2011
// Copyright 2011, All rights reserved

#include <sstream>

#include <TCanvas.h>
#include <TH1.h>
#include <THStack.h>
#include <TLegend.h>
#include <TObject.h>
#include <TPad.h>

#include "interface/Input.h"
#include "interface/Systematic.h"

using namespace std;

Systematic::~Systematic()
{
    for(Heap::iterator obj = _heap.begin();
            _heap.end() != obj;
            ++obj)
    {
        delete *obj;
    }
}

TCanvas *Systematic::draw(const Input &input,
        TH1 *central,
        TH1 *plus,
        TH1 *minus)
{
    ostringstream name;
    name << "canvas" << _heap.size();
    TCanvas *canvas = new TCanvas(name.str().c_str(), "", 600, 700);
    _heap.push_back(canvas);

    name.str("");
    name << "pad" << _heap.size();
    TPad *plot = new TPad(name.str().c_str(), "pad", 0, 0.3, 1, 1);
    _heap.push_back(plot);

    plot->SetBottomMargin(3);
    plot->Draw();
    plot->cd();

    THStack *stack = new THStack();
    _heap.push_back(stack);

    stack->Add(plus);
    stack->Add(central);
    stack->Add(minus);
    stack->Draw("nostack hist");

    string input_name = static_cast<string>(input);
    TLegend *legend = createLegend(input_name);
    legend->AddEntry(plus, "PLUS", "fe");
    legend->AddEntry(central, "CENTRAL", "fe");
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

    stack = new THStack();
    _heap.push_back(stack);

    TH1 *plus_ratio = dynamic_cast<TH1 *>(plus->Clone());
    plus_ratio->Divide(central);
    stack->Add(plus_ratio);

    TH1 *central_ratio = dynamic_cast<TH1 *>(central->Clone());
    central_ratio->Divide(central);
    stack->Add(central_ratio);

    TH1 *minus_ratio = dynamic_cast<TH1 *>(minus->Clone());
    minus_ratio->Divide(central);
    stack->Add(minus_ratio);

    stack->Draw("nostack hist");

    canvas->cd();
    canvas->Update();

    return canvas;
}

TLegend *Systematic::createLegend(const string &text)
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
