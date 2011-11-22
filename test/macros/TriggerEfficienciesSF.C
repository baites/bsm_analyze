
#include <iostream>

#include "TBinomialEfficiencyFitter.h"
#include "TCanvas.h"
#include "TEfficiency.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1D.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TVirtualFitter.h"

Double_t sffit(Double_t * xar, Double_t * par)
{
    Double_t x = xar[0];
    Double_t data = par[1] + par[2] * (x-100.0)*(x-100.0);
    Double_t mc   = par[3] + par[4] * (x-100.0)*(x-100.0);

    Double_t dfd1 = 1;
    Double_t dfd2 = (x-100.0)*(x-100.0);

    Double_t * p = par + 5;

    Double_t s1, s2, sigma = 0;
    
    s1 = *p++*dfd1 + *p++*dfd2;
    s2 = *p++*dfd1 + *p++*dfd2;
    
    sigma += dfd1*s1/(data*data);
    sigma += dfd2*s2/(data*data);
    
    s1 = *p++*dfd1 + *p++*dfd2;
    s2 = *p++*dfd1 + *p++*dfd2;
      
    sigma += dfd1*s1/(mc*mc);
    sigma += dfd2*s2/(mc*mc);

    if (sigma < 0.) sigma = 0;

    return (data/mc)*(1+par[0]*sqrt(sigma));
}


Double_t fitSigma(Double_t * xar, Double_t * par)
{
    Double_t x = xar[0];
    Double_t f = par[1] + par[2] * (x-100.0)*(x-100.0);
                
    Double_t dfd1 = 1;
    Double_t dfd2 = (x-100.0)*(x-100.0);
    
    Double_t * p = par + 3;

    Double_t s1, s2;
    
    s1 = *p++*dfd1 + *p++*dfd2;
    s2 = *p++*dfd1 + *p++*dfd2;
        
    Double_t sigma = 0.;
    
    sigma += dfd1*s1;
    sigma += dfd2*s2;

    if (sigma < 0.) sigma = 0;

    return f+par[0]*sqrt(sigma);       
}


void TriggerEfficienciesSF(TString datafile, TString mcfile, TString ref, TString id, TString variable, TString xtitle, Bool_t linear = true)
{
   gROOT->SetStyle("Plain");   
   gStyle->SetOptStat(0);

   TFile * file = new TFile(datafile);

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
   
   TEfficiency * EffData = new TEfficiency();
   EffData->SetTotalHistogram(*Ref, "f");
   EffData->SetPassedHistogram(*Trigger, "f");
   EffData->SetMarkerStyle(kFullCircle);
   EffData->SetMarkerColor(kBlack);
   EffData->SetLineColor(kBlack);
   EffData->Draw("samep");

   TF1 * datafit = new TF1("datafit","[0] + [1]*(x-100)^2", 100, 800);
   
   datafit->SetParameter(0,1.0);
   datafit->SetParameter(1,0.0);
   if (linear) datafit->FixParameter(1,0.0);

   TBinomialEfficiencyFitter * fitter = new TBinomialEfficiencyFitter(Trigger,Ref);
   fitter->Fit(datafit); 
   datafit->Draw("same");

   Double_t * datamatrix = fitter->GetFitter()->GetCovarianceMatrix(); 
   
   Double_t dataparam[7];
   dataparam[1] = datafit->GetParameter(0);
   dataparam[2] = datafit->GetParameter(1);
   for (Int_t i = 0; i < 4; ++i)
   {
   	   if (linear && i)
           dataparam[i+3] = 0;
   	   else 
           dataparam[i+3] = datamatrix[i];
   }

   dataparam[0] = 1.0;
   TF1 * dataplus = new TF1("dataplus", fitSigma, 100.0, 1000, 7);
   dataplus->SetParameters(dataparam);   
   dataplus->SetLineStyle(kDashed);
   dataplus->SetLineWidth(1);   
   dataplus->Draw("same");
     
   dataparam[0] = -1.0;
   TF1 * dataminus = new TF1("dataminus", fitSigma, 100.0, 1000, 7);
   dataminus->SetParameters(dataparam);  
   dataminus->SetLineStyle(kDashed);
   dataminus->SetLineWidth(1);
   dataminus->Draw("same");
   
   canvas->SaveAs(id+variable+"r"+ref+"-data.png");

   file = new TFile(mcfile);

   Ref = (TH1D*) file->Get("Trigger"+ref+variable)->Clone();
   Trigger = (TH1D*) file->Get("Trigger"+id+variable)->Clone();

   Ref->Rebin(ngroup);
   Trigger->Rebin(ngroup);

   TEfficiency * EffMC = new TEfficiency();
   EffMC->SetTotalHistogram(*Ref, "f");
   EffMC->SetPassedHistogram(*Trigger, "f");
   EffMC->SetMarkerStyle(kFullSquare);
   EffMC->SetMarkerColor(kRed);
   EffMC->SetLineColor(kRed);

   All->Draw("axis");
   EffMC->Draw("samep");
   
   TF1 * mcfit = new TF1("mcfit","[0] + [1]*(x-100)^2", 100, 800);

   mcfit->SetLineColor(kRed);   
   mcfit->SetParameter(0,1.0);
   mcfit->SetParameter(1,0.0);
   if (linear) mcfit->FixParameter(1,0.0);

   TBinomialEfficiencyFitter * mcfitter = new TBinomialEfficiencyFitter(Trigger,Ref);
   mcfitter->Fit(mcfit);
   mcfit->Draw("same");

   Double_t * mcmatrix = mcfitter->GetFitter()->GetCovarianceMatrix(); 
   
   Double_t mcparam[7];
   mcparam[1] = mcfit->GetParameter(0);
   mcparam[2] = mcfit->GetParameter(1);
   for (Int_t i = 0; i < 4; ++i)
   {
   	   if (linear && i)
           mcparam[i+3] = 0;
   	   else 
           mcparam[i+3] = mcmatrix[i];
   }

   mcparam[0] = 1.0;
   TF1 * mcplus = new TF1("dataplus", fitSigma, 100.0, 1000, 7);
   mcplus->SetParameters(mcparam);   
   mcplus->SetLineStyle(kDashed);
   mcplus->SetLineColor(kRed);    
   mcplus->SetLineWidth(1); 
   mcplus->Draw("same");
     
   mcparam[0] = -1.0;
   TF1 * mcminus = new TF1("dataminus", fitSigma, 100.0, 1000, 7);
   mcminus->SetParameters(mcparam);  
   mcminus->SetLineStyle(kDashed);
   mcminus->SetLineColor(kRed);
   mcminus->SetLineWidth(1);
   mcminus->Draw("same");

   canvas->SaveAs(id+variable+"r"+ref+"-mc.png");
   
   Double_t sfparam[13];
   sfparam[1] = dataparam[1];
   sfparam[2] = dataparam[2];
   sfparam[3] = mcparam[1];
   sfparam[4] = mcparam[2];   
   for (Int_t i = 0; i < 4; ++i) sfparam[i+5] = dataparam[i+3];
   for (Int_t i = 0; i < 4; ++i) sfparam[i+9] = mcparam[i+3];

   for (Int_t i = 0; i < 13; ++i) 
     cout << sfparam[i] << " ";
   cout << endl;
 
   sfparam[0] = 0.0;
   TF1 * sf = new TF1("sf", sffit, 100.0, 1000, 13);
   sf->SetParameters(sfparam);   

   All->GetYaxis()->SetTitle("Trigger scale factor");
   All->GetYaxis()->SetRangeUser(0.0,1.1);
   All->Draw("axis");
   sf->Draw("same");

   sfparam[0] = 1.0;
   TF1 * sfplus = new TF1("sfplus", sffit, 100.0, 1000, 13);
   sfplus->SetParameters(sfparam);   
   sfplus->SetLineStyle(kDashed);
   sfplus->SetLineWidth(1); 
   sfplus->Draw("same");

   sfparam[0] = -1.0;
   TF1 * sfminus = new TF1("sfminus", sffit, 100.0, 1000, 13);
   sfminus->SetParameters(sfparam);   
   sfminus->SetLineStyle(kDashed);
   sfminus->SetLineWidth(1); 
   sfminus->Draw("same");

   if (linear) cout << "Linear fit for sf: " << sf->Eval(100) << " +- " << (sfplus->Eval(100)-sf->Eval(100)) << endl; 
      
   canvas->SaveAs(id+variable+"r"+ref+"-sf.png");   
}




