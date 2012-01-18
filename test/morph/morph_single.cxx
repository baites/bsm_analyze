#include <TFile.h>
#include <TH1F.h>
#include <TROOT.h>

#include <RooMomentMorph.h>
#include <RooBinning.h>
#include <RooRealVar.h>
#include <RooDataHist.h>
#include <RooHistPdf.h>

#include <boost/lexical_cast.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace RooFit;

TH1* createInterpolatedHist(TH1* hlower, TH1* hupper, double evaluateMass, double lowerMass, double upperMass, const TString& newName) {
    cout << "interpolating " << lowerMass << ", " << upperMass << "   to   " << evaluateMass << endl;
    double binning[hlower->GetNbinsX()+1];
    for (int i = 1; i <= hlower->GetNbinsX() + 1; ++i) {
        binning[i-1] = hlower->GetBinLowEdge(i); // should also work for overflow
    }
    RooBinning rooBinning(hlower->GetNbinsX(), binning);
    RooRealVar mttbar("mttbar", "mttbar", evaluateMass, "GeV");
    mttbar.setBinning(rooBinning);
    
    //IMPORTANT: give the following four object globally unique names
    // if you interpolate more than one histogram per program run
    // (otherwise, temporary caches of integrals can mess up the result)
    RooDataHist h_1("h_signal_1", "h_signal_1", mttbar, Import(*hlower));
    RooHistPdf pdf_1("pdf_signal_1", "pdf_signal_1", mttbar, h_1);
    RooDataHist h_2("h_signal_2", "h_signal_2", mttbar, Import(*hupper));
    RooHistPdf pdf_2("pdf_signal_2", "pdf_signal_2", mttbar, h_2);

    RooArgList massPdfs;
    massPdfs.add(pdf_1);
    massPdfs.add(pdf_2);
   
    RooRealVar mzprime("mzprime", "mzprime", evaluateMass, lowerMass, upperMass, "GeV");
    
    TVectorD massPoints;
    massPoints.ResizeTo(2);
    massPoints[0] = lowerMass;
    massPoints[1] = upperMass;
    RooMomentMorph morph("morph", "morph", mzprime, mttbar, massPdfs, massPoints);
    
    TH1F* interpolHist = new TH1F(newName, newName, hlower->GetNbinsX(), hlower->GetXaxis()->GetXmin(), hlower->GetXaxis()->GetXmax());
    
    // The RooFit binning convention differs from the ROOT binning convention
    for (Int_t i = 0; i < rooBinning.numBins(); ++i) {
        Double_t binCenter = rooBinning.binCenter(i);
        mttbar.setVal(binCenter);
        interpolHist->SetBinContent(i+1, morph.getVal());
    }
    interpolHist->SetDirectory(0);
    double delta_x = upperMass - lowerMass;
    double delta_y = hupper->Integral() - hlower->Integral();
    double new_norm = (delta_y / delta_x) * (evaluateMass - lowerMass) + hlower->Integral();
    interpolHist->Scale(new_norm / interpolHist->Integral());
    return interpolHist;
}

TH1* load(const string & filename, const string & hname){
    TFile f(filename.c_str(), "read");
    if(f.IsZombie()){
       cerr << "could not open file " << filename << endl;
       exit(1);
    }
    TH1* result = (TH1*)f.Get(hname.c_str());
    if(!result){
       cerr << "Did not find histo " << hname << " in file " << filename << endl;
       exit(1);
    }
    result = (TH1*)result->Clone();
    result->SetDirectory(0);
    return result;
}

int main(int argc, char** argv){
   if(argc!=9){
       cerr << "Usage: morph_single input_rootfile input_hname1 input_hname2 mass1 mass2 target_mass target_hname target_file" << endl;
       exit(1);
   }
   string input_rootfile = argv[1];
   string input_hname1 = argv[2];
   string input_hname2 = argv[3];
   using boost::lexical_cast;
   double mass1 = lexical_cast<double>(argv[4]);
   double mass2 = lexical_cast<double>(argv[5]);
   double target_mass = lexical_cast<double>(argv[6]);
   string target_hname = argv[7];
   string target_file = argv[8];
 
   TH1 * h1 = load(input_rootfile, input_hname1);
   TH1 * h2 = load(input_rootfile, input_hname2);
   TH1 * h = createInterpolatedHist(h1, h2, target_mass, mass1, mass2, target_hname.c_str());
   struct stat buf;
   int res = stat(target_file.c_str(), &buf);
   bool file_exists = res == 0 && (S_ISREG(buf.st_mode));
   TFile f(target_file.c_str(), file_exists ? "update": "recreate");
   h->SetDirectory(&f);
   f.Write();
   f.Close();
}
