//float luminosity = 1010.922;
float luminosity = 973.171;

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
        case QCD_BC_PT20_30: return "QCD_BC_pt20to30";
        case QCD_BC_PT30_80: return "QCD_BC_pt30to80";
        case QCD_BC_PT80_170: return "QCD_BC_pt80to170";
        case QCD_EM_PT20_30: return "QCD_EM_pt20to30";
        case QCD_EM_PT30_80: return "QCD_EM_pt30to80";
        case QCD_EM_PT80_170: return "QCD_EM_pt80to170";
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
                color = kOrange - 4;
                break;
            }
        case QCD_EM_PT30_80:
            {
                color = kOrange - 2;
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
    hist->SetMarkerColor(color);
    hist->SetMarkerSize(0.5);
}

void scale(TH1 *hist, const InputType &input_type)
{
    switch(input_type)
    {
        case QCD_BC_PT20_30:
            {
                hist->Scale(2.361e8 * luminosity * 5.9e-4 /  2071515);
                break;
            }

        case QCD_BC_PT30_80:
            {
                hist->Scale(5.944e7 * luminosity * 2.42e-3 /  2009881);
                break;
            }

        case QCD_BC_PT80_170:
            {
                hist->Scale(8.982e5 * luminosity * 1.05e-2 /  1071954);
                break;
            }

        case QCD_EM_PT20_30:
            {
                hist->Scale(2.361e8 * luminosity * 1.06e-2 /  35577687);
                break;
            }

        case QCD_EM_PT30_80:
            {
                hist->Scale(5.944e7 * luminosity * 6.1e-2 /  55173330);
                break;
            }

        case QCD_EM_PT80_170:
            {
                hist->Scale(8.982e5 * luminosity * 1.59e-1 /  7852884);
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
    TLegend *legend = new TLegend( .5, .8, .8, .6);
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

TFile *input_s1_p50[QCD_CHANNELS];
TFile *input_s2_p50[QCD_CHANNELS];
TFile *input_signal_p50[QCD_CHANNELS];

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

void loadFiles()
{
    for(int i = 0; QCD_CHANNELS > i; ++i)
    {
        TFile *file = open(folder[i] + "/output_s1_p50.root");
        if (!file)
            return;

        input_s1_p50[i] = file;

        file = open(folder[i] + "/output_s2_p50.root");
        if (!file)
            return;

        input_s2_p50[i] = file;

        file = open(folder[i] + "/output_signal_p50.root");
        if (!file)
            return;

        input_signal_p50[i] = file;
    }
}

void plotQCDTemplates()
{
    TH1 *htlep_s1 = merge(input_s1_p50, "htlep", 0, QCD_CHANNELS);
    TH1 *htlep_s2 = merge(input_s2_p50, "htlep", 0, QCD_CHANNELS);
    TH1 *htlep_signal = merge(input_signal_p50, "htlep", 0, QCD_CHANNELS);

    normalize(htlep_s1);
    normalize(htlep_s2);
    normalize(htlep_signal);

    string canvas_title = "QCD Templates (jet pT > 50 GeV/c)";
    TCanvas *canvas = new TCanvas();
    canvas->SetTitle(canvas_title.c_str());
    canvas->SetWindowSize(1200, 480);
    canvas->Divide(3);

    canvas->cd(1);
    htlep_s1->SetLineColor(kYellow + 1);
    htlep_s1->SetMarkerColor(kYellow + 1);
    
    htlep_s2->SetLineColor(2);
    htlep_s2->SetMarkerColor(2);

    htlep_signal->SetLineColor(1);
    htlep_signal->SetMarkerColor(1);

    THStack *stack = new THStack();
    stack->Add(htlep_s1);
    stack->Add(htlep_s2);
    stack->Add(htlep_signal);

    stack->Draw("nostack");
    stack->GetHistogram()->GetXaxis()->SetTitle("H_{T}^{lep} [GeV]");

    TLegend *legend = createLegend(" ");
    legend->AddEntry(htlep_s1, "s1", "lpe");
    legend->AddEntry(htlep_s2, "s2", "lpe");
    legend->AddEntry(htlep_signal, "signal", "lpe");
    legend->Draw();

    TH1 *mttbar_before_htlep_s1 =
        merge(input_s1_p50, "mttbar_before_htlep", 0, QCD_CHANNELS);
    TH1 *mttbar_before_htlep_s2 =
        merge(input_s2_p50, "mttbar_before_htlep", 0, QCD_CHANNELS);
    TH1 *mttbar_before_htlep_signal =
        merge(input_signal_p50, "mttbar_before_htlep", 0, QCD_CHANNELS);

    mttbar_before_htlep_s1->Rebin(20);
    mttbar_before_htlep_s2->Rebin(20);
    mttbar_before_htlep_signal->Rebin(20);

    normalize(mttbar_before_htlep_s1);
    normalize(mttbar_before_htlep_s2);
    normalize(mttbar_before_htlep_signal);

    canvas->cd(2);
    mttbar_before_htlep_s1->SetLineColor(kYellow + 1);
    mttbar_before_htlep_s1->SetMarkerColor(kYellow + 1);
    
    mttbar_before_htlep_s2->SetLineColor(2);
    mttbar_before_htlep_s2->SetMarkerColor(2);

    mttbar_before_htlep_signal->SetLineColor(1);
    mttbar_before_htlep_signal->SetMarkerColor(1);

    stack = new THStack();
    stack->Add(mttbar_before_htlep_s1);
    stack->Add(mttbar_before_htlep_s2);
    stack->Add(mttbar_before_htlep_signal);

    stack->Draw("nostack");
    stack->GetHistogram()->GetXaxis()->SetTitle("M_{t#bar{t}} [GeV/c^{2}]");

    legend = createLegend("before H_{T}^{lep}");
    legend->AddEntry(mttbar_before_htlep_s1, "s1", "lpe");
    legend->AddEntry(mttbar_before_htlep_s2, "s2", "lpe");
    legend->AddEntry(mttbar_before_htlep_signal, "signal", "lpe");
    legend->Draw();

    TH1 *mttbar_after_htlep_s1 =
        merge(input_s1_p50, "mttbar_after_htlep", 0, QCD_CHANNELS);
    TH1 *mttbar_after_htlep_s2 =
        merge(input_s2_p50, "mttbar_after_htlep", 0, QCD_CHANNELS);
    TH1 *mttbar_after_htlep_signal =
        merge(input_signal_p50, "mttbar_after_htlep", 0, QCD_CHANNELS);

    mttbar_after_htlep_s1->Rebin(40);
    mttbar_after_htlep_s2->Rebin(40);
    mttbar_after_htlep_signal->Rebin(40);

    normalize(mttbar_after_htlep_s1);
    normalize(mttbar_after_htlep_s2);
    normalize(mttbar_after_htlep_signal);

    canvas->cd(3);
    mttbar_after_htlep_s1->SetLineColor(kYellow + 1);
    mttbar_after_htlep_s1->SetMarkerColor(kYellow + 1);
    
    mttbar_after_htlep_s2->SetLineColor(2);
    mttbar_after_htlep_s2->SetMarkerColor(2);

    mttbar_after_htlep_signal->SetLineColor(1);
    mttbar_after_htlep_signal->SetMarkerColor(1);

    stack = new THStack();
    stack->Add(mttbar_after_htlep_s1);
    stack->Add(mttbar_after_htlep_s2);
    stack->Add(mttbar_after_htlep_signal);

    stack->Draw("nostack");
    stack->GetHistogram()->GetXaxis()->SetTitle("M_{t#bar{t}} [GeV/c^{2}]");

    legend = createLegend("after H_{T}^{lep}");
    legend->AddEntry(mttbar_after_htlep_s1, "s1", "lpe");
    legend->AddEntry(mttbar_after_htlep_s2, "s2", "lpe");
    legend->AddEntry(mttbar_after_htlep_signal, "signal", "lpe");
    legend->Draw();
}

void qcd_templates()
{
    TGaxis::SetMaxDigits(3);

    loadFiles();
    plotQCDTemplates();
}
