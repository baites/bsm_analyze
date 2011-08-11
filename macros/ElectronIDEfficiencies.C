
void ElectronIDEfficiencies(TString id, TString variable, TString xtitle)
{
   gROOT->SetStyle("Plain");   
   gStyle->SetOptStat(0);

   TFile * file = new TFile("histograms.root");

   TH1D * Loose = (TH1D*) file->Get(id+"Loose"+variable)->Clone();
   TH1D * Medium = (TH1D*) file->Get(id+"Medium"+variable)->Clone();
   TH1D * Tight = (TH1D*) file->Get(id+"Tight"+variable)->Clone(); 
   TH1D * SuperTight = (TH1D*) file->Get(id+"SuperTight"+variable)->Clone();
   TH1D * HyperTight1 = (TH1D*) file->Get(id+"HyperTight1"+variable)->Clone();
   TH1D * HyperTight2 = (TH1D*) file->Get(id+"HyperTight2"+variable)->Clone();
   TH1D * HyperTight3 = (TH1D*) file->Get(id+"HyperTight3"+variable)->Clone();
   TH1D * HyperTight4 = (TH1D*) file->Get(id+"HyperTight4"+variable)->Clone();

   Medium->GetXaxis()->SetTitle(xtitle);
   Medium->GetYaxis()->SetTitle("Efficiency respect to Loose");

   Medium->SetMarkerStyle(kFullTriangleUp);
   Medium->SetMarkerColor(kBlue);
   Medium->SetLineColor(kBlue);

   Tight->SetMarkerStyle(kFullTriangleDown);
   Tight->SetMarkerColor(kYellow);
   Tight->SetLineColor(kYellow);

   SuperTight->SetMarkerStyle(kOpenCircle);
   SuperTight->SetMarkerColor(kMagenta);
   SuperTight->SetLineColor(kMagenta);

   HyperTight1->SetMarkerStyle(kOpenSquare);
   HyperTight1->SetMarkerColor(kCyan);
   HyperTight1->SetLineColor(kCyan);

   HyperTight2->SetMarkerStyle(kOpenTriangleUp);
   HyperTight2->SetMarkerColor(kGreen);
   HyperTight2->SetLineColor(kGreen);

   HyperTight3->SetMarkerStyle(kOpenDiamond);
   HyperTight3->SetMarkerColor(kBlue);
   HyperTight3->SetLineColor(kCyan);

   HyperTight4->SetMarkerStyle(kOpenCross);
   HyperTight4->SetMarkerColor(kGray);
   HyperTight4->SetLineColor(kGray);
   
   Medium->Divide(Medium, Loose, 1., 1., "B");
   Tight->Divide(Tight, Loose, 1., 1., "B");
   SuperTight->Divide(SuperTight, Loose, 1., 1., "B");   
   HyperTight1->Divide(HyperTight1, Loose, 1., 1., "B");
   HyperTight2->Divide(HyperTight2, Loose, 1., 1., "B");
   HyperTight3->Divide(HyperTight3, Loose, 1., 1., "B");
   HyperTight4->Divide(HyperTight4, Loose, 1., 1., "B");

   TCanvas * canvas = new TCanvas();

   Medium->GetYaxis()->SetRangeUser(0.0,1.0);

   Medium->Draw();
   Tight->Draw("SAME");
   SuperTight->Draw("SAME");
   HyperTight1->Draw("SAME");
   HyperTight2->Draw("SAME");
   HyperTight3->Draw("SAME");
   HyperTight4->Draw("SAME");

   TLegend * leg = new TLegend(0.60,0.15,0.86,0.45);
   leg->SetFillColor(kWhite);
   leg->SetLineColor(kWhite);
   leg->AddEntry(Medium, "Medium", "p");
   leg->AddEntry(Tight, "Tight", "p");
   leg->AddEntry(SuperTight, "SuperTight", "p");
   leg->AddEntry(HyperTight1, "HyperTight1", "p");
   leg->AddEntry(HyperTight2, "HyperTight2", "p");
   leg->AddEntry(HyperTight3, "HyperTight3", "p");
   leg->AddEntry(HyperTight4, "HyperTight4", "p");

   leg->Draw();

   canvas->SaveAs(id+variable+"Efficiencies.png");
}

