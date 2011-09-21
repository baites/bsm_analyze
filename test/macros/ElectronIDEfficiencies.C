
void ElectronIDEfficiencies(TString filename, TString id, TString variable, TString xtitle)
{
   gROOT->SetStyle("Plain");   
   gStyle->SetOptStat(0);

   TFile * file = new TFile(filename);

   TH1D * All = (TH1D*) file->Get(TString("EIDAll")+variable)->Clone();
   TH1D * VeryLoose = (TH1D*) file->Get(id+"VeryLoose"+variable)->Clone();
   TH1D * Loose = (TH1D*) file->Get(id+"Loose"+variable)->Clone();
   TH1D * Medium = (TH1D*) file->Get(id+"Medium"+variable)->Clone();
   TH1D * Tight = (TH1D*) file->Get(id+"Tight"+variable)->Clone(); 
   TH1D * SuperTight = (TH1D*) file->Get(id+"SuperTight"+variable)->Clone();
   TH1D * HyperTight1 = (TH1D*) file->Get(id+"HyperTight1"+variable)->Clone();
   TH1D * HyperTight2 = (TH1D*) file->Get(id+"HyperTight2"+variable)->Clone();
   TH1D * HyperTight3 = (TH1D*) file->Get(id+"HyperTight3"+variable)->Clone();
   TH1D * HyperTight4 = (TH1D*) file->Get(id+"HyperTight4"+variable)->Clone();

   Int_t ngroup = 2;

   All->Rebin(ngroup);
   VeryLoose->Rebin(ngroup);   
   Loose->Rebin(ngroup);
   Medium->Rebin(ngroup);
   Tight->Rebin(ngroup);
   SuperTight->Rebin(ngroup);
   HyperTight1->Rebin(ngroup);
   HyperTight2->Rebin(ngroup);
   HyperTight3->Rebin(ngroup);
   HyperTight4->Rebin(ngroup);

   TCanvas * canvas = new TCanvas();
   //canvas->SetLogy();

   All->GetXaxis()->SetTitle(xtitle);
   All->GetYaxis()->SetTitle("Efficiency respect to synch selection");
   All->GetYaxis()->SetRangeUser(0.5,1.0);
   All->Draw("axis");

   TEfficiency * VeryLooseEff =  new TEfficiency();
   VeryLooseEff->SetTotalHistogram(*All, "f");
   VeryLooseEff->SetPassedHistogram(*VeryLoose, "f");
   VeryLooseEff->SetMarkerStyle(kFullCircle);
   VeryLooseEff->SetMarkerColor(kRed);
   VeryLooseEff->SetLineColor(kRed);
   VeryLooseEff->Draw("samep");
   if (ngroup > 10) cout << "Efficiency for VeryLoose: " << VeryLooseEff->GetEfficiency(1) << std::endl;

   TEfficiency * LooseEff =  new TEfficiency();
   LooseEff->SetTotalHistogram(*All, "f");
   LooseEff->SetPassedHistogram(*Loose, "f");
   LooseEff->SetMarkerStyle(kFullSquare);
   LooseEff->SetMarkerColor(kGreen);
   LooseEff->SetLineColor(kGreen);
   LooseEff->Draw("samep");
   if (ngroup > 10) cout << "Efficiency for Loose: " << LooseEff->GetEfficiency(1) << std::endl;

   TEfficiency * MediumEff =  new TEfficiency();
   MediumEff->SetTotalHistogram(*Loose, "f");
   MediumEff->SetPassedHistogram(*Medium, "f");
   MediumEff->SetMarkerStyle(kFullTriangleUp);
   MediumEff->SetMarkerColor(kBlue);
   MediumEff->SetLineColor(kBlue);
   MediumEff->Draw("samep");
   if (ngroup > 10) cout << "Efficiency for Medium: " << MediumEff->GetEfficiency(1) << std::endl;

   TEfficiency * TightEff =  new TEfficiency();
   TightEff->SetTotalHistogram(*Loose, "f");
   TightEff->SetPassedHistogram(*Tight, "f");
   TightEff->SetMarkerStyle(kFullTriangleDown);
   TightEff->SetMarkerColor(kYellow);
   TightEff->SetLineColor(kYellow);
   TightEff->Draw("samep");
   if (ngroup > 10) cout << "Efficiency for Tight: " << TightEff->GetEfficiency(1) << std::endl;

   TEfficiency * SuperTightEff =  new TEfficiency();
   SuperTightEff->SetTotalHistogram(*Loose, "f");
   SuperTightEff->SetPassedHistogram(*SuperTight, "f");
   SuperTightEff->SetMarkerStyle(kOpenCircle);
   SuperTightEff->SetMarkerColor(kMagenta);
   SuperTightEff->SetLineColor(kMagenta);
   SuperTightEff->Draw("samep");
   if (ngroup > 10) cout << "Efficiency for SuperTight1: " << SuperTightEff->GetEfficiency(1) << std::endl;

   TEfficiency * HyperTight1Eff =  new TEfficiency();
   HyperTight1Eff->SetTotalHistogram(*Loose, "f");
   HyperTight1Eff->SetPassedHistogram(*HyperTight1, "f");
   HyperTight1Eff->SetMarkerStyle(kOpenSquare);
   HyperTight1Eff->SetMarkerColor(kCyan);
   HyperTight1Eff->SetLineColor(kCyan);
   HyperTight1Eff->Draw("samep");
   if (ngroup > 10) cout << "Efficiency for HyperTight1: " << HyperTight1Eff->GetEfficiency(1) << std::endl;

   TEfficiency * HyperTight2Eff =  new TEfficiency();
   HyperTight2Eff->SetTotalHistogram(*Loose, "f");
   HyperTight2Eff->SetPassedHistogram(*HyperTight2, "f");
   HyperTight2Eff->SetMarkerStyle(kOpenTriangleUp);
   HyperTight2Eff->SetMarkerColor(kGreen);
   HyperTight2Eff->SetLineColor(kGreen);
   HyperTight2Eff->Draw("samep");
   if (ngroup > 10) cout << "Efficiency for HyperTight2: " << HyperTight2Eff->GetEfficiency(1) << std::endl;

   TEfficiency * HyperTight3Eff =  new TEfficiency();
   HyperTight3Eff->SetTotalHistogram(*Loose, "f");
   HyperTight3Eff->SetPassedHistogram(*HyperTight3, "f");
   HyperTight3Eff->SetMarkerStyle(kOpenDiamond);
   HyperTight3Eff->SetMarkerColor(kBlue);
   HyperTight3Eff->SetLineColor(kCyan);
   HyperTight3Eff->Draw("samep");
   if (ngroup > 10) cout << "Efficiency for HyperTight3: " << HyperTight3Eff->GetEfficiency(1) << std::endl;

   TEfficiency * HyperTight4Eff =  new TEfficiency();
   HyperTight4Eff->SetTotalHistogram(*Loose, "f");
   HyperTight4Eff->SetPassedHistogram(*HyperTight4, "f");
   HyperTight4Eff->SetMarkerStyle(kOpenCross);
   HyperTight4Eff->SetMarkerColor(kGray);
   HyperTight4Eff->SetLineColor(kGray);
   HyperTight4Eff->Draw("samep");
   if (ngroup > 10) cout << "Efficiency for HyperTight4: " << HyperTight4Eff->GetEfficiency(1) << std::endl;

   TLegend * leg = new TLegend(0.60,0.15,0.86,0.50);
   leg->SetFillColor(kWhite);
   leg->SetLineColor(kWhite);
   leg->AddEntry(VeryLooseEff, "VeryLoose", "p");
   leg->AddEntry(LooseEff, "Loose", "p");
   leg->AddEntry(MediumEff, "Medium", "p");
   leg->AddEntry(TightEff, "Tight", "p");
   leg->AddEntry(SuperTightEff, "SuperTight", "p");
   leg->AddEntry(HyperTight1Eff, "HyperTight1", "p");
   leg->AddEntry(HyperTight2Eff, "HyperTight2", "p");
   leg->AddEntry(HyperTight3Eff, "HyperTight3", "p");
   leg->AddEntry(HyperTight4Eff, "HyperTight4", "p");
   leg->Draw();

   canvas->SaveAs(id+variable+"_"+filename.Remove(filename.Length()-5)+".png");
}

