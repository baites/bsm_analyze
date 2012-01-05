
Int_t ninputs = 3;

char *inputs[] = {
    "ttjets",
    "wjets",
    "zprime_m1000_w10"
};

char * names[] = {
"t#bar{t}",
"W#rightarrowl#nu",
"Z' 1.0 TeV/c^{2}"
};

Int_t colors[] = {
    kRed + 1,
    kGreen + 1,
    kBlack
};

Int_t markers[] = {
    kOpenTriangleUp,
    kOpenSquare,
    kOpenCircle
};

void TriggerThreshold(TString input)
{
   gROOT->SetStyle("Plain");   
   gStyle->SetOptStat(0);

   TCanvas * canvas = new TCanvas();

   TLegend * leg = new TLegend(0.55,0.15,0.89,0.40);
   leg->SetFillColor(kWhite);
   leg->SetLineColor(kWhite);
   leg->SetTextSize(0.06);

   for (Int_t i=0; i<ninputs; ++i)
   {
       TFile * file = new TFile(TString(inputs[i])+"/"+input);

       TH1D * All = (TH1D*) file->Get("AllTriggerThreshold")->Clone();
       TH1D * Trigger = (TH1D*) file->Get("PassTriggerThreshold")->Clone();

       Int_t ngroup = 1;

       All->Rebin(ngroup);
       Trigger->Rebin(ngroup);

       if (!i)
       {
           All->GetXaxis()->SetTitle("p^{e}_{T} [GeV/c]");
           All->GetYaxis()->SetTitle("Trigger efficiency");
           All->GetYaxis()->SetRangeUser(0.0,1.1);
           All->Draw("axis");
       }
   
       TEfficiency * TriggerEff = new TEfficiency();
       TriggerEff->SetTotalHistogram(*All, "f");
       TriggerEff->SetPassedHistogram(*Trigger, "f");
       TriggerEff->SetMarkerStyle(markers[i]);
       TriggerEff->SetMarkerColor(colors[i]);
       TriggerEff->SetLineColor(colors[i]);
       TriggerEff->Draw("samep");

       leg->AddEntry(TriggerEff, names[i], "p");
   }

   leg->Draw();

   TLatex * label = new TLatex();
   label->SetNDC();
   label->SetTextColor(kBlack);
   label->DrawLatex(.55, .92, "CMS MC Preliminary");

   canvas->SaveAs("TriggerThreshold.png");
   canvas->SaveAs("TriggerThreshold.pdf");
}

