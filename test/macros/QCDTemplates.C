float luminosity = 3393.157;

enum InputType
{
    QCD_BC_PT20_30 = 0,
    QCD_BC_PT30_80,
    QCD_BC_PT80_170,
    QCD_EM_PT20_30,
    QCD_EM_PT30_80,
    QCD_EM_PT80_170,
    UNKNOWN
};

string toString(const InputType &input_type)
{
    switch(input_type)
    {
        case QCD_BC_PT20_30: return  "QCD BC pt20to30";
        case QCD_BC_PT30_80: return  "QCD BC pt30to80";
        case QCD_BC_PT80_170: return "QCD BC pt80to170";
        case QCD_EM_PT20_30: return  "QCD EM pt20to30";
        case QCD_EM_PT30_80: return  "QCD EM pt30to80";
        case QCD_EM_PT80_170: return "QCD EM pt80to170";
        default: return "Unknown";
    }
}

void style(TH1 *hist, const InputType &input_type)
{
    int color = 1;
    switch(input_type)
    {
        case QCD_BC_PT20_30:
            {
                color = kYellow + 1;
                break;
            }
        case QCD_BC_PT30_80:
            {
                color = kYellow + 2;
                break;
            }
        case QCD_BC_PT80_170:
            {
                color = kYellow + 3;
                break;
            }
        case QCD_EM_PT20_30:
            {
                color = kOrange - 3;
                break;
            }
        case QCD_EM_PT30_80:
            {
                color = kOrange - 1;
                break;
            }
        case QCD_EM_PT80_170:
            {
                color = kOrange;
                break;
            }

        default:
            {
                cerr << "unknown type: can not style the plot" << endl;

                return;
            }
    }

    hist->SetLineColor(color);
    hist->SetFillColor(color);
    hist->SetMarkerColor(color);
    hist->SetMarkerSize(0.5);
}

void scale(TH1 *hist, const InputType &input_type)
{
    if (!hist->GetEntries())
        return;

    switch(input_type)
    {
        case QCD_BC_PT20_30:
            {
                hist->Scale(2.361e8 * luminosity * 5.9e-4 /  2081560);
                break;
            }

        case QCD_BC_PT30_80:
            {
                hist->Scale(5.944e7 * luminosity * 2.42e-3 /  2030033);
                break;
            }

        case QCD_BC_PT80_170:
            {
                hist->Scale(8.982e5 * luminosity * 1.05e-2 /  1082691);
                break;
            }

        case QCD_EM_PT20_30:
            {
                hist->Scale(2.361e8 * luminosity * 1.06e-2 /  35729669);
                break;
            }

        case QCD_EM_PT30_80:
            {
                hist->Scale(5.944e7 * luminosity * 6.1e-2 /  70392060);
                break;
            }

        case QCD_EM_PT80_170:
            {
                hist->Scale(8.982e5 * luminosity * 1.59e-1 /  8150672);
                break;
            }

        default:
            {
                cerr << "unknown type: can not scale the plot" << endl;
                break;
            }
    }

}

void normalize(TH1 *hist)
{
    if (hist->GetEntries())
        hist->Scale(1. / hist->Integral());
}

TLegend *createLegend(const string &text)
{
    TLegend *legend = new TLegend( .5, .5, .8, .88);
    if (!text.empty())
        legend->SetHeader(text.c_str());

    legend->SetMargin(0.12);
    legend->SetTextSize(0.06);
    legend->SetFillColor(10);
    legend->SetBorderSize(0);

    return legend;
}

//
// Histograms will be scaled by x-section, Nevents, Luminosity,
// filter-efficiency if InputType is supplied
//
TH1 *get(const TFile *input, const string &path, const InputType &input_type)
{   
    TObject *object = input->Get(path.c_str());
    if (!object)
    {
        cerr << "failed to get: " << path << endl;

        return 0;
    }

    TH1 *hist = dynamic_cast<TH1 *>(object->Clone());
    if (!hist)
    {
        cerr << "object does not seem to be TH1" << endl;

        return 0;
    }

    style(hist, input_type);

    return hist;
}

TH1 *merge(TFile **input, const string &path, const int &from, const int &to)
{
    TH1 *result = 0;
    for(int i = from; to > i; ++i)
    {
        TH1 *hist = get(input[i], path, i);
        if (!hist)
        {
            cerr << "failed to extract: " << path << endl;

            continue;
        }

        scale(hist, i);

        if (!result)
            result = dynamic_cast<TH1 *>(hist->Clone());
        else
            result->Add(hist);
    }

    return result;
}

string folder[] =
{
    "qcd_bc_pt20to30",
    "qcd_bc_pt30to80",
    "qcd_bc_pt80to170",
    "qcd_em_pt20to30",
    "qcd_em_pt30to80",
    "qcd_em_pt80to170",
};

const int QCD_CHANNELS = 6;

TFile *input_s1[QCD_CHANNELS];
TFile *input_s2[QCD_CHANNELS];
TFile *input_signal[QCD_CHANNELS];

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

void loadFiles(const string & file1, const string & file2)
{
    for(int i = 0; QCD_CHANNELS > i; ++i)
    {
        TFile *file = open(folder[i] + "/" + file2);
        if (!file)
            return;

        input_s1[i] = file;

        /*file = open(dir2 + "/" + folder[i] + ".root");
        if (!file)
            return;

        input_s2[i] = file;*/

        file = open(folder[i] + "/" + file1);
        if (!file)
            return;

        input_signal[i] = file;
    }
}

void plotQCDTemplates()
{
    TH1 *htlep_s1 = merge(input_s1, "htlep", 0, QCD_CHANNELS);
    TH1 *htlep_signal = merge(input_signal, "htlep", 0, QCD_CHANNELS);

    htlep_s1->Rebin(20);
    htlep_signal->Rebin(20);

    normalize(htlep_s1);
    normalize(htlep_signal);

    string canvas_title = "QCD Templates (jet pT > 50 GeV/c)";
    TCanvas *canvas = new TCanvas();
    canvas->SetTitle(canvas_title.c_str());
    canvas->SetWindowSize(1200, 480);
    canvas->Divide(2);

    canvas->cd(1);
    htlep_s1->SetLineColor(kYellow + 1);
    htlep_s1->SetMarkerColor(kYellow + 1);
    htlep_s1->SetLineWidth(2);

    htlep_signal->SetLineColor(1);
    htlep_signal->SetMarkerColor(1);
    htlep_signal->SetLineWidth(2);

    THStack *stack = new THStack();
    stack->Add(htlep_s1);
    stack->Add(htlep_signal);

    // stack->SetMaximum(0.5);
    stack->Draw("9 nostack");
    stack->GetHistogram()->GetXaxis()->SetTitle("MET [GeV]");

    TLegend *legend = createLegend("Triag. cut");
    legend->AddEntry(htlep_s1, "No pass", "l");
    legend->AddEntry(htlep_signal, "Pass", "l");
    legend->Draw("9");

    TH1 *mttbar_after_htlep_s1 =
        merge(input_s1, "mttbar_after_htlep", 0, QCD_CHANNELS);
    TH1 *mttbar_after_htlep_signal =
        merge(input_signal, "mttbar_after_htlep", 0, QCD_CHANNELS);

    mttbar_after_htlep_s1->Rebin(100);
    mttbar_after_htlep_signal->Rebin(100);

    normalize(mttbar_after_htlep_s1);
    normalize(mttbar_after_htlep_signal);

    canvas->cd(2);
    mttbar_after_htlep_s1->SetLineColor(kYellow + 1);
    mttbar_after_htlep_s1->SetMarkerColor(kYellow + 1);
    mttbar_after_htlep_s1->SetLineWidth(2);    

    mttbar_after_htlep_signal->SetLineColor(1);
    mttbar_after_htlep_signal->SetMarkerColor(1);
    mttbar_after_htlep_signal->SetLineWidth(2);

    stack = new THStack();
    stack->Add(mttbar_after_htlep_signal);
    stack->Add(mttbar_after_htlep_s1);

    // stack->SetMaximum(0.4);
    stack->Draw("9 nostack");
    stack->GetHistogram()->GetXaxis()->SetTitle("M_{t#bar{t}} [GeV/c^{2}]");

    legend = createLegend("Triag cut");
    legend->AddEntry(mttbar_after_htlep_s1, "No pass", "l");
    legend->AddEntry(mttbar_after_htlep_signal, "Pass", "l");
    legend->Draw("9");

    canvas->SaveAs("qcd_template_comparison.png");
    canvas->SaveAs("qcd_template_comparison.pdf");    
}

void plotQCD(TFile **input, const string &hist, const string &axis_title, const string &title, const int rebin = 8)
{
    TH1 *qcd_bc_pt20_30 = get(input[QCD_BC_PT20_30], hist, QCD_BC_PT20_30);
    TH1 *qcd_bc_pt30_80 = get(input[QCD_BC_PT30_80], hist, QCD_BC_PT30_80);
    TH1 *qcd_bc_pt80_170 = get(input[QCD_BC_PT80_170], hist, QCD_BC_PT80_170);
    TH1 *qcd_em_pt20_30 = get(input[QCD_EM_PT20_30], hist, QCD_EM_PT20_30);
    TH1 *qcd_em_pt30_80 = get(input[QCD_EM_PT30_80], hist, QCD_EM_PT30_80);
    TH1 *qcd_em_pt80_170 = get(input[QCD_EM_PT80_170], hist, QCD_EM_PT80_170);

    if (rebin)
    {
        if (qcd_bc_pt20_30->GetEntries()) qcd_bc_pt20_30->Rebin(rebin);
        if (qcd_bc_pt30_80->GetEntries()) qcd_bc_pt30_80->Rebin(rebin);
        if (qcd_bc_pt80_170->GetEntries()) qcd_bc_pt80_170->Rebin(rebin);
        if (qcd_em_pt20_30->GetEntries()) qcd_em_pt20_30->Rebin(rebin);
        if (qcd_em_pt30_80->GetEntries()) qcd_em_pt30_80->Rebin(rebin);
        if (qcd_em_pt80_170->GetEntries()) qcd_em_pt80_170->Rebin(rebin);
    }

    THStack *stack = new THStack();
    if (qcd_bc_pt20_30->GetEntries()) stack->Add(qcd_bc_pt20_30);
    if (qcd_bc_pt30_80->GetEntries()) stack->Add(qcd_bc_pt30_80);
    if (qcd_bc_pt80_170->GetEntries()) stack->Add(qcd_bc_pt80_170);
    if (qcd_em_pt20_30->GetEntries()) stack->Add(qcd_em_pt20_30);
    if (qcd_em_pt30_80->GetEntries()) stack->Add(qcd_em_pt30_80);
    if (qcd_em_pt80_170->GetEntries()) stack->Add(qcd_em_pt80_170);

    stack->Draw("hist");
    if (stack->GetHistogram())
        stack->GetHistogram()->GetXaxis()->SetTitle(axis_title.c_str());

    TLegend *legend = createLegend(title);
    legend->AddEntry(qcd_bc_pt20_30, toString(QCD_BC_PT20_30).c_str(), "fe");
    legend->AddEntry(qcd_bc_pt30_80, toString(QCD_BC_PT30_80).c_str(), "fe");
    legend->AddEntry(qcd_bc_pt80_170, toString(QCD_BC_PT80_170).c_str(), "fe");
    legend->AddEntry(qcd_em_pt20_30, toString(QCD_EM_PT20_30).c_str(), "fe");
    legend->AddEntry(qcd_em_pt30_80, toString(QCD_EM_PT30_80).c_str(), "fe");
    legend->AddEntry(qcd_em_pt80_170, toString(QCD_EM_PT80_170).c_str(), "fe");
    legend->Draw();
}

void plotQCDComparisonInRegion(TFile **input, const string &title)
{
    string canvas_title = "QCD Comparison: " + title;
    TCanvas *canvas = new TCanvas();
    canvas->SetTitle(canvas_title.c_str());
    canvas->SetWindowSize(1200, 680);
    canvas->Divide(3);

    canvas->cd(1)->SetLogy();
    plotQCD(input, "htlep", "H_{T}^{lep} [Gev/c^{2}]", "H_{T}^{lep}");

    canvas->cd(2)->SetLogy();
    plotQCD(input, "mttbar_before_htlep", "M_{t#bar{t}} [GeV/c^{2}]", "m_{t#bar{t}} before H_{T}^{lep}", 20);

    canvas->cd(3)->SetLogy();
    plotQCD(input, "mttbar_after_htlep", "M_{t#bar{t}} [GeV/c^{2}]", "m_{t#bar{t}} after H_{T}^{lep}", 40);
}

void plotQCDComparison()
{
    plotQCDComparisonInRegion(input_s1, "sideband 1");
    // plotQCDComparisonInRegion(input_s2, "sideband 2");
    plotQCDComparisonInRegion(input_signal, "signal");
}

void QCDTemplates(const string & file1, const string & file2)
{
    gROOT->SetStyle("Plain");
    TGaxis::SetMaxDigits(3);

    loadFiles(file1, file2);

    // plotQCDComparison();
    plotQCDTemplates();
}
