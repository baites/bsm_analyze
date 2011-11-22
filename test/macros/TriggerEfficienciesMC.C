
void TriggerEfficienciesMC(TString filename, TString ref, TString id, TString variable, TString xtitle)
{
   gROOT->SetStyle("Plain");   
   gStyle->SetOptStat(0);

   TFile * file = new TFile(filename);

   TH1D * All = (TH1D*) file->Get(TString("AllEvents")+variable)->Clone();
   TH1D * Ref = (TH1D*) file->Get("Trigger"+ref+variable)->Clone();
   TH1D * Trigger = (TH1D*) file->Get("Trigger"+id+variable)->Clone();

   Int_t ngroup = 2;

   All->Rebin(ngroup);
   Ref->Rebin(ngroup);
   Trigger->Rebin(ngroup);

   TCanvas * canvas = new TCanvas();

   // canvas->SetLogy();

   All->GetXaxis()->SetTitle(xtitle);
   All->GetYaxis()->SetTitle("Trigger efficiency");
   All->GetYaxis()->SetRangeUser(0.0,1.1);
   All->Draw("axis");

   TEfficiency * RefEff = new TEfficiency();
   RefEff->SetTotalHistogram(*All, "f");
   RefEff->SetPassedHistogram(*Trigger, "f");
   RefEff->SetMarkerStyle(kFullSquare);
   RefEff->SetMarkerColor(kBlack);
   RefEff->SetLineColor(kBlack);
   RefEff->Draw("samep");
   if (ngroup > 10) cout << "Efficiency for Reference " << id << ": " << RefEff->GetEfficiency(1) << std::endl;
   
   TEfficiency * TriggerEff = new TEfficiency();
   TriggerEff->SetTotalHistogram(*Ref, "f");
   TriggerEff->SetPassedHistogram(*Trigger, "f");
   TriggerEff->SetMarkerStyle(kFullCircle);
   TriggerEff->SetMarkerColor(kRed);
   TriggerEff->SetLineColor(kRed);
   TriggerEff->Draw("samep");
   if (ngroup > 10) cout << "Efficiency for Trigger " << id << ": " << TriggerEff->GetEfficiency(1) << std::endl;

   TLegend * leg = new TLegend(0.50,0.15,0.86,0.40);
   leg->SetFillColor(kWhite);
   leg->SetLineColor(kWhite);
   leg->AddEntry(RefEff, "MC Trigger efficiency", "p");
   leg->AddEntry(TriggerEff, "Method", "p");
   leg->Draw();

   canvas->SaveAs(id+variable+"r"+ref+"_"+filename.Remove(filename.Length()-5)+"_mc.png");
}

