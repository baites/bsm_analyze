
void TriggerEfficienciesData(TString filename, TString ref, TString id, TString variable, TString xtitle)
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
   
   TEfficiency * TriggerEff = new TEfficiency();
   TriggerEff->SetTotalHistogram(*Ref, "f");
   TriggerEff->SetPassedHistogram(*Trigger, "f");
   TriggerEff->SetMarkerStyle(kFullCircle);
   TriggerEff->SetMarkerColor(kRed);
   TriggerEff->SetLineColor(kRed);

   Int_t nbins = All->GetNbinsX();
   Double_t xmin = All->GetXaxis()->GetBinLowEdge(1);
   Double_t xmax = All->GetXaxis()->GetBinUpEdge(nbins);

   All->Rebin(All->GetNbinsX());
   Ref->Rebin(Ref->GetNbinsX());
   Trigger->Rebin(Trigger->GetNbinsX());
 
   TEfficiency * TotalEff = new TEfficiency();
   TotalEff->SetTotalHistogram(*Ref, "f");
   TotalEff->SetPassedHistogram(*Trigger, "f");

   cout << "Efficiency for Trigger " << id << " using as reference " << ref << " : " << TotalEff->GetEfficiency(1);
   cout << " +" << TotalEff->GetEfficiencyErrorUp(1) << " -" << TotalEff->GetEfficiencyErrorLow(1) << endl;

   Float_t x[200], y[200], exl[200], exh[200], eyl[200], eyh[200];

   for (Int_t i = 0; i < nbins; ++i)
   {
        x[i] = xmin + i*(xmax - xmin)/(nbins-1);
        y[i] = TotalEff->GetEfficiency(1);
        exl[i] = (xmax-xmin)/(2*nbins);
        exh[i] = (xmax-xmin)/(2*nbins);
        eyl[i] = TotalEff->GetEfficiencyErrorLow(1);
        eyh[i] = TotalEff->GetEfficiencyErrorUp(1);
   }

   TGraphAsymmErrors * graph = new TGraphAsymmErrors(nbins, x, y, exl, exh, eyl, eyh);
   graph->SetFillColor(kYellow-6);
   graph->SetLineWidth(3);
   graph->Draw("3");
   graph->Draw("LX");
   TriggerEff->Draw("samep");

   TLegend * leg = new TLegend(0.50,0.15,0.86,0.40);
   leg->SetFillColor(kWhite);
   leg->SetLineColor(kWhite);
   leg->AddEntry(TriggerEff, "Trigger efficiency", "p");
   leg->AddEntry(graph, "Total efficiency", "l");
   leg->Draw();

   canvas->SaveAs(id+variable+"r"+ref+"_"+filename.Remove(filename.Length()-5)+".png");
}

