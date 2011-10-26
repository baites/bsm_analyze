enum InputType
{
    RERECO_2011A_MAY10 = 0,
    RERECO_2011A_AUG05,
    PROMPT_2011A_V4,
    PROMPT_2011A_V6,
    PROMPT_2011B_V1,
    UNKNOWN
};

string toString(const InputType &input_type)
{
    switch(input_type)
    {
        case RERECO_2011A_MAY10: return "Re-reco 2011A May10";
        case RERECO_2011A_AUG05: return "Re-reco 2011A Aug05";
        case PROMPT_2011A_V4: return "Prompt 2011A v4";
        case PROMPT_2011A_V6: return "Prompt 2011A v6";
        case PROMPT_2011B_V1: return "Prompt 2011B v1";
        default: return "Unknown";
    }
}

void style(TH1 *hist, const InputType &input_type)
{
    int color = 1;
    switch(input_type)
    {
        case RERECO_2011A_MAY10:
            {
                color = kGray;
                break;
            }
        case RERECO_2011A_AUG05:
            {
                color = kGray + 1;
                break;
            }
        case PROMPT_2011A_V4:
            {
                color = kGray + 2;
                break;
            }
        case PROMPT_2011A_V6:
            {
                color = kGray + 3;
                break;
            }
        case PROMPT_2011B_V1:
            {
                color = kBlack;
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

        if (!result)
            result = dynamic_cast<TH1 *>(hist->Clone());
        else
            result->Add(hist);
    }

    return result;
}

string folder[] =
{
    "golden_single_el_2011a_aug5_rereco_v1",
    "golden_single_el_2011a_may10_rereco",
    "golden_single_el_2011a_prompt_v4",
    "golden_single_el_2011a_prompt_v6",
    "golden_single_el_2011b_prompt_v1"
};

const int DATA_CHANNELS = 5;

TFile *input_s1_p50[DATA_CHANNELS];
TFile *input_s2_p50[DATA_CHANNELS];
TFile *input_s1_p250[DATA_CHANNELS];
TFile *input_s2_p250[DATA_CHANNELS];

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
    for(int i = 0; DATA_CHANNELS > i; ++i)
    {
        TFile *file = open(folder[i] + "/output_s1_p50.root");
        if (!file)
            return;

        input_s1_p50[i] = file;

        file = open(folder[i] + "/output_s2_p50.root");
        if (!file)
            return;

        input_s2_p50[i] = file;

        file = open(folder[i] + "/output_s1_p250.root");
        if (!file)
            return;

        input_s1_p250[i] = file;

        file = open(folder[i] + "/output_s2_p250.root");
        if (!file)
            return;

        input_s2_p250[i] = file;
    }
}

void plotDataComparison()
{
    TH1 *htlep_s1 = merge(input_s1_p250, "htlep", 0, DATA_CHANNELS);
    TH1 *htlep_s2 = merge(input_s2_p250, "htlep", 0, DATA_CHANNELS);

    normalize(htlep_s1);
    normalize(htlep_s2);

    string canvas_title = "Comparison of Data sidebands";

    TCanvas *canvas = new TCanvas();
    canvas->SetTitle(canvas_title.c_str());
    canvas->SetWindowSize(800, 640);
    canvas->Divide(2, 2);

    canvas->cd(1);
    htlep_s1->SetLineColor(4);
    htlep_s1->SetMarkerColor(4);
    
    htlep_s2->SetLineColor(2);
    htlep_s2->SetMarkerColor(2);

    THStack *stack = new THStack();
    stack->Add(htlep_s1);
    stack->Add(htlep_s2);

    stack->Draw("nostack");
    stack->GetHistogram()->GetXaxis()->SetTitle("H_{T}^{lep} [GeV]");

    TLegend *legend = createLegend("p_{T}^{jet} > 250 [GeV/c]");
    legend->AddEntry(htlep_s1, "s1", "lpe");
    legend->AddEntry(htlep_s2, "s2", "lpe");
    legend->Draw();

    TH1 *mttbar_after_htlep_s1 =
        merge(input_s1_p250, "mttbar_after_htlep", 0, DATA_CHANNELS);
    TH1 *mttbar_after_htlep_s2 =
        merge(input_s2_p250, "mttbar_after_htlep", 0, DATA_CHANNELS);

    mttbar_after_htlep_s1->Rebin(20);
    mttbar_after_htlep_s2->Rebin(20);

    normalize(mttbar_after_htlep_s1);
    normalize(mttbar_after_htlep_s2);

    canvas->cd(2);
    mttbar_after_htlep_s1->SetLineColor(4);
    mttbar_after_htlep_s1->SetMarkerColor(4);
    
    mttbar_after_htlep_s2->SetLineColor(2);
    mttbar_after_htlep_s2->SetMarkerColor(2);

    stack = new THStack();
    stack->Add(mttbar_after_htlep_s1);
    stack->Add(mttbar_after_htlep_s2);

    stack->Draw("nostack");
    stack->GetHistogram()->GetXaxis()->SetTitle("M_{t#bar{t}} [GeV/c^{2}]");

    legend = createLegend("p_{T}^{jet} > 250 [Gev/c]");
    legend->AddEntry(mttbar_after_htlep_s1, "s1", "lpe");
    legend->AddEntry(mttbar_after_htlep_s2, "s2", "lpe");
    legend->Draw();

    htlep_s1 = merge(input_s1_p50, "htlep", 0, DATA_CHANNELS);
    htlep_s2 = merge(input_s2_p50, "htlep", 0, DATA_CHANNELS);

    normalize(htlep_s1);
    normalize(htlep_s2);

    canvas->cd(3);
    htlep_s1->SetLineColor(4);
    htlep_s1->SetMarkerColor(4);
    
    htlep_s2->SetLineColor(2);
    htlep_s2->SetMarkerColor(2);

    THStack *stack = new THStack();
    stack->Add(htlep_s1);
    stack->Add(htlep_s2);

    stack->Draw("nostack");
    stack->GetHistogram()->GetXaxis()->SetTitle("H_{T}^{lep} [GeV]");

    TLegend *legend = createLegend("p_{T}^{jet} > 50 [GeV/c]");
    legend->AddEntry(htlep_s1, "s1", "lpe");
    legend->AddEntry(htlep_s2, "s2", "lpe");
    legend->Draw();

    mttbar_after_htlep_s1 =
        merge(input_s1_p50, "mttbar_after_htlep", 0, DATA_CHANNELS);
    mttbar_after_htlep_s2 =
        merge(input_s2_p50, "mttbar_after_htlep", 0, DATA_CHANNELS);

    mttbar_after_htlep_s1->Rebin(20);
    mttbar_after_htlep_s2->Rebin(20);

    normalize(mttbar_after_htlep_s1);
    normalize(mttbar_after_htlep_s2);

    canvas->cd(4);
    mttbar_after_htlep_s1->SetLineColor(4);
    mttbar_after_htlep_s1->SetMarkerColor(4);
    
    mttbar_after_htlep_s2->SetLineColor(2);
    mttbar_after_htlep_s2->SetMarkerColor(2);

    stack = new THStack();
    stack->Add(mttbar_after_htlep_s1);
    stack->Add(mttbar_after_htlep_s2);

    stack->Draw("nostack");
    stack->GetHistogram()->GetXaxis()->SetTitle("M_{t#bar{t}} [GeV/c^{2}]");

    legend = createLegend("p_{T}^{jet} > 50 [Gev/c]");
    legend->AddEntry(mttbar_after_htlep_s1, "s1", "lpe");
    legend->AddEntry(mttbar_after_htlep_s2, "s2", "lpe");
    legend->Draw();
}

void data_sidebands()
{
    TGaxis::SetMaxDigits(3);

    loadFiles();

    plotDataComparison();
}
