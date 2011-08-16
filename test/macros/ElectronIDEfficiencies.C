
void ElectronIDEfficiencies(TString filename, TString id, TString variable, TString xtitle)
{
   gROOT->SetStyle("Plain");   
   gStyle->SetOptStat(0);

   TFile * file = new TFile(filename);

   TH1D * Loose = (TH1D*) file->Get(id+"Loose"+variable)->Clone();
   TH1D * Medium = (TH1D*) file->Get(id+"Medium"+variable)->Clone();
   TH1D * Tight = (TH1D*) file->Get(id+"Tight"+variable)->Clone(); 
   TH1D * SuperTight = (TH1D*) file->Get(id+"SuperTight"+variable)->Clone();
   TH1D * HyperTight1 = (TH1D*) file->Get(id+"HyperTight1"+variable)->Clone();
   TH1D * HyperTight2 = (TH1D*) file->Get(id+"HyperTight2"+variable)->Clone();
   TH1D * HyperTight3 = (TH1D*) file->Get(id+"HyperTight3"+variable)->Clone();
   TH1D * HyperTight4 = (TH1D*) file->Get(id+"HyperTight4"+variable)->Clone();

   TCanvas * canvas = new TCanvas();

   Loose->GetXaxis()->SetTitle(xtitle);
   Loose->GetYaxis()->SetTitle("Efficiency respect to Loose");
   Loose->GetYaxis()->SetRangeUser(0.0,1.1);

   Loose->Draw("axis");

   TEfficiency * MediumEff =  new TEfficiency();
   MediumEff->SetTotalHistogram(*Loose, "f");
   MediumEff->SetPassedHistogram(*Medium, "f");
   MediumEff->SetMarkerStyle(kFullTriangleUp);
   MediumEff->SetMarkerColor(kBlue);
   MediumEff->SetLineColor(kBlue);
   MediumEff->Draw("samep");

   TEfficiency * TightEff =  new TEfficiency();
   TightEff->SetTotalHistogram(*Loose, "f");
   TightEff->SetPassedHistogram(*Tight, "f");
   TightEff->SetMarkerStyle(kFullTriangleDown);
   TightEff->SetMarkerColor(kYellow);
   TightEff->SetLineColor(kYellow);
   TightEff->Draw("samep");

   TEfficiency * SuperTightEff =  new TEfficiency();
   SuperTightEff->SetTotalHistogram(*Loose, "f");
   SuperTightEff->SetPassedHistogram(*SuperTight, "f");
   SuperTightEff->SetMarkerStyle(kOpenCircle);
   SuperTightEff->SetMarkerColor(kMagenta);
   SuperTightEff->SetLineColor(kMagenta);
   SuperTightEff->Draw("samep");

   TEfficiency * HyperTight1Eff =  new TEfficiency();
   HyperTight1Eff->SetTotalHistogram(*Loose, "f");
   HyperTight1Eff->SetPassedHistogram(*HyperTight1, "f");
   HyperTight1Eff->SetMarkerStyle(kOpenSquare);
   HyperTight1Eff->SetMarkerColor(kCyan);
   HyperTight1Eff->SetLineColor(kCyan);
   HyperTight1Eff->Draw("samep");

   TEfficiency * HyperTight2Eff =  new TEfficiency();
   HyperTight2Eff->SetTotalHistogram(*Loose, "f");
   HyperTight2Eff->SetPassedHistogram(*HyperTight2, "f");
   HyperTight2Eff->SetMarkerStyle(kOpenTriangleUp);
   HyperTight2Eff->SetMarkerColor(kGreen);
   HyperTight2Eff->SetLineColor(kGreen);
   HyperTight2Eff->Draw("samep");

   TEfficiency * HyperTight3Eff =  new TEfficiency();
   HyperTight3Eff->SetTotalHistogram(*Loose, "f");
   HyperTight3Eff->SetPassedHistogram(*HyperTight3, "f");
   HyperTight3Eff->SetMarkerStyle(kOpenDiamond);
   HyperTight3Eff->SetMarkerColor(kBlue);
   HyperTight3Eff->SetLineColor(kCyan);
   HyperTight3Eff->Draw("samep");

   TEfficiency * HyperTight4Eff =  new TEfficiency();
   HyperTight4Eff->SetTotalHistogram(*Loose, "f");
   HyperTight4Eff->SetPassedHistogram(*HyperTight4, "f");
   HyperTight4Eff->SetMarkerStyle(kOpenCross);
   HyperTight4Eff->SetMarkerColor(kGray);
   HyperTight4Eff->SetLineColor(kGray);
   HyperTight4Eff->Draw("samep");

   TLegend * leg = new TLegend(0.60,0.15,0.86,0.45);
   leg->SetFillColor(kWhite);
   leg->SetLineColor(kWhite);
   leg->AddEntry(MediumEff, "Medium", "p");
   leg->AddEntry(TightEff, "Tight", "p");
   leg->AddEntry(SuperTightEff, "SuperTight", "p");
   leg->AddEntry(HyperTight1Eff, "HyperTight1", "p");
   leg->AddEntry(HyperTight2Eff, "HyperTight2", "p");
   leg->AddEntry(HyperTight3Eff, "HyperTight3", "p");
   leg->AddEntry(HyperTight4Eff, "HyperTight4", "p");
   leg->Draw();

   canvas->SaveAs(id+variable+"Efficiencies.png");
}

