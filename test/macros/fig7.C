TFile *open(const string &filename)
{
    TFile *in = TFile::Open(filename.c_str(), "READ");
    if (!in->IsOpen())
    {
        cerr << "failed to open input: " << filename << endl;

        in = 0;
    }

    return in;
}

TH1 *get(TFile *input, const string &path, const bool &isS2 = false)
{
    TH1 *hist = dynamic_cast<TH1 *>(input->Get(path.c_str()));
    if (!hist)
    {
        cerr << "failed to get: " << path << endl;

        hist = 0;
    }

    if (hist)
    {
        int color = isS2 ? 2 : 4;

        hist->SetLineColor(color);
        hist->SetMarkerColor(color);

        // Normalize histogram
        //
        if (hist->GetEntries())
            hist->Scale(1. / hist->Integral());

        hist->GetXaxis()->SetRangeUser(0, 500);
    }

    return hist;
}

TH1 *get2D(TFile *input, const string &path)
{
    TH1 *hist = dynamic_cast<TH1 *>(input->Get(path.c_str()));
    if (!hist)
    {
        cerr << "failed to get: " << path << endl;

        hist = 0;
    }

    return hist;
}

void plot(const string &histogram,
        TFile *qcd20_s1,
        TFile *qcd20_s2,
        TFile *qcd30_s1,
        TFile *qcd30_s2,
        TFile *qcd80_s1,
        TFile *qcd80_s2)
{
    // Exract historgrams
    //
    TH1 *h20_s1 = get(qcd20_s1, histogram);
    if (!h20_s1)
        return;

    TH1 *h20_s2 = get(qcd20_s2, histogram, true);
    if (!h20_s2)
        return;

    TH1 *h30_s1 = get(qcd30_s1, histogram);
    if (!h30_s1)
        return;

    TH1 *h30_s2 = get(qcd30_s2, histogram, true);
    if (!h30_s2)
        return;

    TH1 *h80_s1 = get(qcd80_s1, histogram);
    if (!h80_s1)
        return;

    TH1 *h80_s2 = get(qcd80_s2, histogram, true);
    if (!h80_s2)
        return;

    TCanvas *canvas = new TCanvas(histogram.c_str());
    canvas->SetWindowSize(1024, 480);
    canvas->Divide(3);

    canvas->cd(1);

    h20_s1->Draw("h");
    h20_s2->Draw("h same");

    canvas->cd(2);

    h30_s1->Draw("h");
    h30_s2->Draw("h same");

    canvas->cd(3);

    h80_s1->Draw("h");
    h80_s2->Draw("h same");
}

void plot(const string &histogram,
        TFile *qcd20_s1,
        TFile *qcd30_s1,
        TFile *qcd80_s1)
{
    // Exract historgrams
    //
    TH1 *h20_s1 = get2D(qcd20_s1, histogram);
    if (!h20_s1)
        return;

    TH1 *h30_s1 = get2D(qcd30_s1, histogram);
    if (!h30_s1)
        return;

    TH1 *h80_s1 = get2D(qcd80_s1, histogram);
    if (!h80_s1)
        return;

    TCanvas *canvas = new TCanvas(histogram.c_str());
    canvas->SetWindowSize(1024, 480);
    canvas->Divide(3);

    canvas->cd(1);

    h20_s1->Draw("colz");

    canvas->cd(2);

    h30_s1->Draw("colz");

    canvas->cd(3);

    h80_s1->Draw("colz");
}

void fig7()
{
    // Open files
    //
    TFile *qcd20_s1 = open("qcd20_s1.root");
    if (!qcd20_s1)
        return;

    TFile *qcd20_s2 = open("qcd20_s2.root");
    if (!qcd20_s2)
        return;

    TFile *qcd30_s1 = open("qcd30_s1.root");
    if (!qcd30_s1)
        return;

    TFile *qcd30_s2 = open("qcd30_s2.root");
    if (!qcd30_s2)
        return;

    TFile *qcd80_s1 = open("qcd80_s1.root");
    if (!qcd80_s1)
        return;

    TFile *qcd80_s2 = open("qcd80_s2.root");
    if (!qcd80_s2)
        return;

    plot("htlep", qcd20_s1, qcd20_s2, qcd30_s1, qcd30_s2, qcd80_s1, qcd80_s2);
    plot("mttbar", qcd20_s1, qcd20_s2, qcd30_s1, qcd30_s2, qcd80_s1, qcd80_s2);
    plot("dr_vs_ptrel", qcd20_s1, qcd30_s1, qcd80_s1);
}
