// Monitors Visualizers
//
// Generate standartized Canvases for monitors
//
// Created by Samvel Khalatyan, May 17, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <sstream>

#include <TCanvas.h>
#include <TDirectory.h>
#include <TH1.h>
#include <TH2.h>

#include "bsm_core/interface/ID.h"
#include "bsm_stat/interface/Utility.h"

#include "interface/Monitor.h"
#include "interface/MonitorCanvas.h"

using namespace std;

using namespace bsm;

// -- Canvas Base --------------------------------------------------------------
//
bool Canvas::pushd(TDirectory *dir)
{
    TDirectory *pwd = gDirectory->GetDirectory("");

    if (!dir)
        dir = gDirectory;

    TDirectory *subdir = 0;
    TObject *object = dir->FindObject(folder().c_str());
    if (object)
    {
        subdir = dynamic_cast<TDirectory *>(object);
    }
    else
    {
        subdir = dir->mkdir(folder().c_str());
        if (!subdir)
        {
            cerr << "failed ot create output folder: " << folder()
                << " in " << dir->GetName() << endl;
        }
    }

    bool result = subdir->cd();
    if (result)
        _dirs.push(pwd);

    return result;
}

bool Canvas::popd()
{
    bool result = _dirs.empty() ? false : _dirs.top()->cd();
    if (result)
        _dirs.pop();
    else
        cerr << "failed to change folder to: " << _dirs.top()->GetName()
            << endl;

    return result;
}

string Canvas::folder()
{
    if (_folder.empty())
    {
        _folder = title();
        replace(_folder.begin(), _folder.end(), ' ', '_');
    }

    return _folder;
}

Canvas::TCanvasPtr Canvas::canvas()
{
    if (!_canvas)
    {
        ostringstream name;
        name << "delta_canvas_" << ++Canvas::_id;

        _canvas.reset(new TCanvas(name.str().c_str(), title().c_str()));
    }
    
    return _canvas;
}

// Private
//
uint32_t Canvas::_id = 0;




// Delta Canvas
//
void DeltaCanvas::draw(const DeltaMonitor &monitor)
{
    TCanvasPtr canvas_ = canvas();
    canvas_->SetWindowSize(1024, 640);
    canvas_->Divide(3, 2);

    canvas_->cd(1);
    _r = convert(*monitor.r());
    _r->GetXaxis()->SetTitle("#Delta R");
    _r->Draw("hist");

    canvas_->cd(2);
    _ptrel = convert(*monitor.ptrel());
    _ptrel->GetXaxis()->SetTitle("p_{T}^{rel} [GeV/c]");
    _ptrel->Draw("hist");

    canvas_->cd(3);
    _ptrel_vs_r = convert(*monitor.ptrel_vs_r());
    _ptrel_vs_r->GetXaxis()->SetTitle("p_{T}^{rel} [GeV/c]");
    _ptrel_vs_r->GetYaxis()->SetTitle("#Delta R");
    _ptrel_vs_r->Draw("colz");

    canvas_->cd(4);
    _phi = convert(*monitor.phi());
    _phi->GetXaxis()->SetTitle("#Delta #phi [rad]");
    _phi->Draw("hist");

    canvas_->cd(5);
    _eta = convert(*monitor.eta());
    _eta->GetXaxis()->SetTitle("#Delta #eta");
    _eta->Draw("hist");
}

void DeltaCanvas::write(const DeltaMonitor &monitor, TDirectory *parent)
{
    if (!pushd(parent))
        return;

    _r = convert(*monitor.r());
    _r->SetName("dr");
    _r->Write();

    _ptrel = convert(*monitor.ptrel());
    _ptrel->SetName("ptrel");
    _ptrel->Write();

    _eta = convert(*monitor.eta());
    _eta->SetName("deta");
    _eta->Write();

    _phi = convert(*monitor.phi());
    _phi->SetName("dphi");
    _phi->Write();

    _ptrel_vs_r = convert(*monitor.ptrel_vs_r());
    _ptrel_vs_r->SetName("ptrel_vs_dr");
    _ptrel_vs_r->Write();

    popd();
}



// Electron Canvas
//
void ElectronCanvas::draw(const ElectronsMonitor &monitor)
{
    TCanvasPtr canvas_ = canvas();
    canvas_->SetWindowSize(1024, 480);
    canvas_->Divide(3);

    canvas_->cd(1);
    _multiplicity = convert(*monitor.multiplicity());
    _multiplicity->GetXaxis()->SetTitle("N_{e}");
    _multiplicity->Draw("hist");

    canvas_->cd(2);
    _leading_pt = convert(*monitor.leading_pt());
    _leading_pt->GetXaxis()->SetTitle("leading p^{e}_{T} [GeV/c]");
    _leading_pt->Draw("hist");

    canvas_->cd(3);
    _pt = convert(*monitor.pt());
    _pt->GetXaxis()->SetTitle("p^{e}_{T} [GeV/c]");
    _pt->Draw("hist");
}

void ElectronCanvas::write(const ElectronsMonitor &monitor, TDirectory *parent)
{
    if (!pushd(parent))
        return;

    _multiplicity = convert(*monitor.multiplicity());
    _multiplicity->SetName("multiplicity");
    _multiplicity->Write();

    _leading_pt = convert(*monitor.leading_pt());
    _leading_pt->SetName("leading_pt");
    _leading_pt->Write();

    _pt = convert(*monitor.pt());
    _pt->SetName("pt");
    _pt->Write();

    popd();
}



// Jet Canvas
//
void JetCanvas::draw(const JetsMonitor &monitor)
{
    TCanvasPtr canvas_ = canvas();
    canvas_->SetWindowSize(1024, 640);
    canvas_->Divide(3, 2);

    canvas_->cd(1);
    _multiplicity = convert(*monitor.multiplicity());
    _multiplicity->GetXaxis()->SetTitle("N_{jet}");
    _multiplicity->Draw("hist");

    canvas_->cd(4);
    _children = convert(*monitor.children());
    _children->GetXaxis()->SetTitle("N_{children}");
    _children->Draw("hist");

    canvas_->cd(2);
    _leading_pt = convert(*monitor.leading_pt());
    _leading_pt->GetXaxis()->SetTitle("leading p^{jet}_{T} [GeV/c]");
    _leading_pt->Draw("hist");

    canvas_->cd(5);
    _leading_uncorrected_pt = convert(*monitor.leading_uncorrected_pt());
    _leading_uncorrected_pt->GetXaxis()->SetTitle("leading uncorrected p^{jet}_{T} [GeV/c]");
    _leading_uncorrected_pt->Draw("hist");

    canvas_->cd(3);
    _pt = convert(*monitor.pt());
    _pt->GetXaxis()->SetTitle("p^{jet}_{T} [GeV/c]");
    _pt->Draw("hist");

    canvas_->cd(6);
    _uncorrected_pt = convert(*monitor.uncorrected_pt());
    _uncorrected_pt->GetXaxis()->SetTitle("Uncorrected p^{jet}_{T} [GeV/c]");
    _uncorrected_pt->Draw("hist");
}

void JetCanvas::write(const JetsMonitor &monitor, TDirectory *parent)
{
    if (!pushd(parent))
        return;

    _multiplicity = convert(*monitor.multiplicity());
    _multiplicity->SetName("multiplicity");
    _multiplicity->Write();

    _leading_pt = convert(*monitor.leading_pt());
    _leading_pt->SetName("leading_pt");
    _leading_pt->Write();

    _pt = convert(*monitor.pt());
    _pt->SetName("pt");
    _pt->Write();

    _children = convert(*monitor.children());
    _children->SetName("children");
    _children->Write();

    popd();
}



// LorentzVector Canvas
//
void P4Canvas::draw(const P4Monitor &monitor)
{
    TCanvasPtr canvas_ = canvas();
    canvas_->SetWindowSize(1024, 640);
    canvas_->Divide(3, 2);

    canvas_->cd(1);
    _pt = convert(*monitor.pt());
    _pt->SetName("pt");
    string title = "p_{T}";
    if (!axis_subtitle().empty())
        title += "^{" + axis_subtitle() + "}";
    title += " [GeV/c]";
    _pt->GetXaxis()->SetTitle(title.c_str());
    _pt->Draw("hist");

    canvas_->cd(2);
    _eta = convert(*monitor.eta());
    _eta->SetName("eta");
    title = "#eta";
    if (!axis_subtitle().empty())
        title += "^{" + axis_subtitle() + "}";
    _eta->GetXaxis()->SetTitle(title.c_str());
    _eta->Draw("hist");

    canvas_->cd(3);
    _phi = convert(*monitor.phi());
    _phi->SetName("phi");
    title = "#phi";
    if (!axis_subtitle().empty())
        title += "^{" + axis_subtitle() + "}";
    title += " [rad]";
    _phi->GetXaxis()->SetTitle(title.c_str());
    _phi->Draw("hist");

    canvas_->cd(4);
    _mass = convert(*monitor.mass());
    _mass->SetName("mass");
    title = "M";
    if (!axis_subtitle().empty())
        title += "^{" + axis_subtitle() + "}";
    title += " [GeV/c^{2}]";
    _mass->GetXaxis()->SetTitle(title.c_str());
    _mass->Draw("hist");

    canvas_->cd(5);
    _mass = convert(*monitor.mt());
    _mass->SetName("mt");
    _mass->SetName("mass");
    title = "M_{T}";
    if (!axis_subtitle().empty())
        title += "^{" + axis_subtitle() + "}";
    title += " [GeV/c^{2}]";
    _mass->GetXaxis()->SetTitle(title.c_str());
    _mass->Draw("hist");
}

void P4Canvas::write(const P4Monitor &monitor, TDirectory *parent)
{
    if (!pushd(parent))
        return;

    _energy = convert(*monitor.energy());
    _energy->SetName("energy");
    _energy->Write();

    _px = convert(*monitor.px());
    _px->SetName("px");
    _px->Write();

    _py = convert(*monitor.py());
    _py->SetName("py");
    _py->Write();

    _pz = convert(*monitor.pz());
    _pz->SetName("pz");
    _pz->Write();

    _pt = convert(*monitor.pt());
    _pt->SetName("pt");
    _pt->Write();

    _eta = convert(*monitor.eta());
    _eta->SetName("eta");
    _eta->Write();

    _phi = convert(*monitor.phi());
    _phi->SetName("phi");
    _phi->Write();

    _mass = convert(*monitor.mass());
    _mass->SetName("mass");
    _mass->Write();

    _mt = convert(*monitor.mt());
    _mt->SetName("mt");
    _mt->Write();

    _et = convert(*monitor.et());
    _et->SetName("et");
    _et->Write();

    popd();
}



// GenParticle Canvas
//
void GenParticleCanvas::draw(const GenParticleMonitor &monitor)
{
    P4Canvas::draw(monitor);
}

void GenParticleCanvas::write(const GenParticleMonitor &monitor, TDirectory *parent)
{
    P4Canvas::write(monitor, parent);

    if (!pushd(parent))
        return;

    _pdg_id = convert(*monitor.pdg_id());
    _pdg_id->SetName("pdg_id");
    _pdg_id->Write();

    _status = convert(*monitor.status());
    _status->SetName("status");
    _status->Write();

    popd();
}



// MissingEnergy Canvas
//
void MissingEnergyCanvas::draw(const MissingEnergyMonitor &monitor)
{
    TCanvasPtr canvas_ = canvas();
    canvas_->SetWindowSize(800, 640);
    canvas_->Divide(2, 2);

    canvas_->cd(1);
    _pt = convert(*monitor.pt());
    _pt->GetXaxis()->SetTitle("p^{MET}_{T} [GeV/c]");
    _pt->Draw("hist");

    canvas_->cd(2);
    _x = convert(*monitor.x());
    _x->GetXaxis()->SetTitle("X^{MET} [cm]");
    _x->Draw("hist");

    canvas_->cd(3);
    _y = convert(*monitor.y());
    _y->GetXaxis()->SetTitle("Y^{MET} [cm]");
    _y->Draw("hist");

    canvas_->cd(4);
    _z = convert(*monitor.z());
    _z->GetXaxis()->SetTitle("Z^{MET} [cm]");
    _z->Draw("hist");
}

void MissingEnergyCanvas::write(const MissingEnergyMonitor &monitor, TDirectory *parent)
{
    if (!pushd(parent))
        return;

    _pt = convert(*monitor.pt());
    _pt->SetName("pt");
    _pt->Write();

    _x = convert(*monitor.x());
    _x->SetName("x");
    _x->Write();

    _y = convert(*monitor.y());
    _y->SetName("y");
    _y->Write();

    _z = convert(*monitor.z());
    _z->SetName("z");
    _z->Write();

    popd();
}



// Muon Canvas
//
void MuonCanvas::draw(const MuonsMonitor &monitor)
{
    TCanvasPtr canvas_ = canvas();
    canvas_->SetWindowSize(1024, 480);
    canvas_->Divide(3);

    canvas_->cd(1);
    _multiplicity = convert(*monitor.multiplicity());
    _multiplicity->GetXaxis()->SetTitle("N_{#mu}");
    _multiplicity->Draw("hist");

    canvas_->cd(2);
    _leading_pt = convert(*monitor.leading_pt());
    _leading_pt->GetXaxis()->SetTitle("leading p^{#mu}_{T} [GeV/c]");
    _leading_pt->Draw("hist");

    canvas_->cd(3);
    _pt = convert(*monitor.pt());
    _pt->GetXaxis()->SetTitle("p^{#mu}_{T} [GeV/c]");
    _pt->Draw("hist");
}

void MuonCanvas::write(const MuonsMonitor &monitor, TDirectory *parent)
{
    if (!pushd(parent))
        return;

    _multiplicity = convert(*monitor.multiplicity());
    _multiplicity->SetName("multiplicity");
    _multiplicity->Write();

    _leading_pt = convert(*monitor.leading_pt());
    _leading_pt->SetName("leading_pt");
    _leading_pt->Write();

    _pt = convert(*monitor.pt());
    _pt->SetName("pt");
    _pt->Write();

    popd();
}



// PrimaryVertex Canvas
//
void PrimaryVertexCanvas::draw(const PrimaryVerticesMonitor &monitor)
{
    TCanvasPtr canvas_ = canvas();
    canvas_->SetWindowSize(800, 640);
    canvas_->Divide(2, 2);

    canvas_->cd(1);
    _multiplicity = convert(*monitor.multiplicity());
    _multiplicity->GetXaxis()->SetTitle("N_{PV}");
    _multiplicity->Draw("hist");

    canvas_->cd(2);
    _x = convert(*monitor.x());
    _x->GetXaxis()->SetTitle("X^{PV} [cm]");
    _x->Draw("hist");

    canvas_->cd(3);
    _y = convert(*monitor.y());
    _y->GetXaxis()->SetTitle("Y^{PV} [cm]");
    _y->Draw("hist");

    canvas_->cd(4);
    _z = convert(*monitor.z());
    _z->GetXaxis()->SetTitle("Z^{PV} [cm]");
    _z->Draw("hist");
}


void PrimaryVertexCanvas::write(const PrimaryVerticesMonitor &monitor, TDirectory *parent)
{
    if (!pushd(parent))
        return;

    _multiplicity = convert(*monitor.multiplicity());
    _multiplicity->SetName("multiplicity");
    _multiplicity->Write();

    _x = convert(*monitor.x());
    _x->SetName("x");
    _x->Write();

    _y = convert(*monitor.y());
    _y->SetName("y");
    _y->Write();

    _z = convert(*monitor.z());
    _z->SetName("z");
    _z->Write();

    popd();
}
