float luminosity = 1010.922;

string folder[] =
{
    "qcd_bc_pt20to30",
    "qcd_bc_pt30to80",
    "qcd_bc_pt80to170",
    "qcd_em_pt20to30",
    "qcd_em_pt30to80",
    "qcd_em_pt80to170",
    "ttjets",
    "wjets",
    "zjets",
    "zprime_1000",
    "zprime_1500",
    "zprime_2000",
    "zprime_3000",
    "zprime_4000",
    "rereco",
    "prompt"
};

const int QCD_CHANNELS = 6;
const int BACKGROUND_CHANNELS = QCD_CHANNELS + 3;
const int SIGNAL_CHANNELS = 5;
const int DATA_CHANNELS = 2;
const int MC_CHANNELS = BACKGROUND_CHANNELS + SIGNAL_CHANNELS;
const int CHANNELS = MC_CHANNELS + DATA_CHANNELS;

TFile *input_s1[CHANNELS];
TFile *input_s2[CHANNELS];
TFile *input_signal[MC_CHANNELS];
TFile *input_s1s2_pt50[CHANNELS];
TFile *input_s1s2_pt250[CHANNELS];

enum InputType
{
    QCD_BC_PT20_30 = 0,
    QCD_BC_PT30_80,
    QCD_BC_PT80_170,
    QCD_EM_PT20_30,
    QCD_EM_PT30_80,
    QCD_EM_PT80_170,
    TTJETS,
    ZJETS,
    WJETS,
    ZPRIME_1000,
    ZPRIME_1500,
    ZPRIME_2000,
    ZPRIME_3000,
    ZPRIME_4000,
    RERECO,
    PROMPT,
    UNKNOWN
};

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
    // Sideband 1
    //
    string filename = "output_s1.root";
    for(int i = 0; MC_CHANNELS > i; ++i)
    {
        TFile *file = open(folder[i] + "/" + filename);
        if (!file)
            return;

        input_s1[i] = file;
    }

    // Sideband 2
    //
    filename = "output_s2.root";
    for(int i = 0; MC_CHANNELS > i; ++i)
    {
        TFile *file = open(folder[i] + "/" + filename);
        if (!file)
            return;

        input_s2[i] = file;
    }

    // Signal Region
    //
    filename = "output_signal.root";
    for(int i = 0; MC_CHANNELS > i; ++i)
    {
        TFile *file = open(folder[i] + "/" + filename);
        if (!file)
            return;

        input_signal[i] = file;
    }

    // Sideband 1+2 pt50
    //
    filename = "output_s1s2_p50.root";
    for(int i = 0; CHANNELS > i; ++i)
    {
        if (ZPRIME_1000 == i
                || ZPRIME_1500 == i
                || ZPRIME_2000 == i
                || ZPRIME_3000 == i
                || ZPRIME_4000 == i)
            continue;

        TFile *file = open(folder[i] + "/" + filename);
        if (!file)
            return;

        input_s1s2_pt50[i] = file;
    }

    // Sideband 1+2 pt250
    //
    filename = "output_s1s2_p250.root";
    for(int i = 0; CHANNELS > i; ++i)
    {
        if (ZPRIME_1000 == i
                || ZPRIME_1500 == i
                || ZPRIME_2000 == i
                || ZPRIME_3000 == i
                || ZPRIME_4000 == i)
            continue;

        TFile *file = open(folder[i] + "/" + filename);
        if (!file)
            return;

        input_s1s2_pt250[i] = file;
    }
}

TLegend *createLegend(const string &text)
{
    TLegend *legend = new TLegend( .2, .7, .3, .8);
    if (!text.empty())
        legend->SetHeader(text.c_str());

    legend->SetMargin(0.12);
    legend->SetTextSize(0.06);
    legend->SetFillColor(10);
    legend->SetBorderSize(0);

    return legend;
}

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
        case TTJETS: return "TTjets";
        case ZJETS: return "Zjets";
        case WJETS: return "Wjets";
        case ZPRIME_1000: return "Z' 1 TeV";
        case ZPRIME_1500: return "Z' 1.5 TeV";
        case ZPRIME_2000: return "Z' 2 TeV";
        case ZPRIME_3000: return "Z' 3 TeV";
        case ZPRIME_4000: return "Z' 4 TeV";
        case RERECO: return "Rereco";
        case PROMPT: return "Prompt";
        default: return "Unknown";
    }
}

void style(TH1 *hist, const InputType &input_type, const bool &set_fill = true)
{
    int color = 1;
    switch(input_type)
    {
        case QCD_BC_PT20_30: // Fall through
        case QCD_BC_PT30_80: // Fall through
        case QCD_BC_PT80_170: // Fall through
        case QCD_EM_PT20_30: // Fall through
        case QCD_EM_PT30_80: // Fall through
        case QCD_EM_PT80_170:
            {
                color = kYellow + 1;
                break;
            }

        case TTJETS:
            {
                color = 2;
                break;
            }

        case ZJETS:
            {
                color = 4;
                break;
            }

        case WJETS:
            {
                color = 3;
                break;
            }

        case ZPRIME_1000: // Fall through
        case ZPRIME_1500: // Fall through 
        case ZPRIME_2000: // Fall through 
        case ZPRIME_3000: // Fall through 
        case ZPRIME_4000: 
            {
                color = kAzure - 2 * (input_type - ZPRIME_1000);
                break;
            }

        case RERECO: // Fall through
        case PROMPT: // Do nothing
            break;

        default:
            {
                cerr << "unknown type: can not scale the plot" << endl;
                break;
            }
    }

    hist->SetLineColor(color);
    hist->SetMarkerColor(color);

    if (set_fill)
        hist->SetFillColor(color);
}

// Histograms will be scaled by x-section, Nevents, Luminosity,
// filter-efficiency if InputType is supplied
//
TH1 *get(const string &path, TFile *input, const InputType &input_type = UNKNOWN)
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

        case TTJETS:
            {
                // Use NLO x-section: 157.5 instead of LO: 94.76
                //
                hist->Scale(157.5 * luminosity * 1.0 /  2748612);
                break;
            }

        case ZJETS:
            {
                // Use NLO x-section: 3048 instead of LO: 2475
                //
                hist->Scale(3048 * luminosity * 1.0 /  29414670);
                break;
            }

        case WJETS:
            {
                // Use NLO x-section: 31314 instead of LO: 27770
                //
                hist->Scale(31314 * luminosity * 1.0 /  39705660);
                break;
            }

        case RERECO: // Fall through
        case PROMPT: // Do nothing
            break;

        default:
            {
                cerr << "unknown type: can not scale the plot" << endl;
                break;
            }
    }

    if (UNKNOWN != input_type)
        style(hist, input_type);

    return hist;
}

// Extract and merge histograms in range [from, to) (to is not included)
//
TH1 *merge(const string &path, TFile **input, const int &from, const int &to,
        const bool &do_normalize = false)
{
    TH1 *result = 0;
    for(int i = from; to > i; ++i)
    {
        TH1 *hist = get(path, input[i], i);
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

    if (do_normalize
            && result
            && result->GetEntries())
    {
        result->Scale(1. / result->Integral());
    }

    return result;
}

void plotCut2DBackground()
{
    string canvas_title = "Cut 2D Background";
    TCanvas *canvas = new TCanvas(canvas_title.c_str(), canvas_title.c_str());
    canvas->SetWindowSize(1200, 800);
    canvas->Divide(3, 3);

    canvas_title = "DeltaR Background";
    TCanvas *canvas_dr = new TCanvas(canvas_title.c_str(), canvas_title.c_str());
    canvas_dr->SetWindowSize(1200, 800);
    canvas_dr->Divide(3, 3);

    canvas_title = "pTrel Background";
    TCanvas *canvas_ptrel = new TCanvas(canvas_title.c_str(), canvas_title.c_str());
    canvas_ptrel->SetWindowSize(1200, 800);
    canvas_ptrel->Divide(3, 3);
    for(int i = 0; BACKGROUND_CHANNELS > i; ++i)
    {
        TH1 *hist = get("dr_vs_ptrel", input_s1[i]);
        if (!hist)
            continue;

        canvas->cd(i + 1);

        style(hist, i);
        hist->SetMarkerSize(0.1);
        hist->Draw("scat");

        TLegend *legend = createLegend(toString(i));
        legend->Draw();

        canvas_ptrel->cd(i + 1);
        TH2 *hist2d = dynamic_cast<TH2 *>(hist);
        TH1 *ptrel = dynamic_cast<TH1 *>(hist2d->ProjectionX()->Clone());
        style(ptrel, i);

        ptrel->Draw();
        legend->Draw();

        canvas_dr->cd(i + 1);
        TH2 *hist2d = dynamic_cast<TH2 *>(hist);
        TH1 *dr = dynamic_cast<TH1 *>(hist2d->ProjectionY()->Clone());
        style(dr, i);

        dr->Draw();
        legend->Draw();
    }
}

void plotCut2DSignal()
{
    string canvas_title = "Cut 2D Signal";
    TCanvas *canvas = new TCanvas(canvas_title.c_str(), canvas_title.c_str());
    canvas->SetWindowSize(1200, 640);
    canvas->Divide(3, 2);

    canvas_title = "DeltaR Signal";
    TCanvas *canvas_dr = new TCanvas(canvas_title.c_str(), canvas_title.c_str());
    canvas_dr->SetWindowSize(1200, 640);
    canvas_dr->Divide(3, 2);

    canvas_title = "pTrel Signal";
    TCanvas *canvas_ptrel = new TCanvas(canvas_title.c_str(), canvas_title.c_str());
    canvas_ptrel->SetWindowSize(1200, 640);
    canvas_ptrel->Divide(3, 2);
    for(int i = 0; SIGNAL_CHANNELS > i; ++i)
    {
        int id = BACKGROUND_CHANNELS + i;
        TH1 *hist = get("dr_vs_ptrel", input_s1[id], id);
        if (!hist)
            continue;

        canvas->cd(i + 1);

        style(hist, id);
        hist->SetMarkerSize(0.1);
        hist->Draw("scat");

        TLegend *legend = createLegend(toString(id));
        legend->Draw();

        canvas_ptrel->cd(i + 1);
        TH2 *hist2d = dynamic_cast<TH2 *>(hist);
        TH1 *ptrel = dynamic_cast<TH1 *>(hist2d->ProjectionX()->Clone());
        style(ptrel, id);

        ptrel->Draw();
        legend->Draw();

        canvas_dr->cd(i + 1);
        TH2 *hist2d = dynamic_cast<TH2 *>(hist);
        TH1 *dr = dynamic_cast<TH1 *>(hist2d->ProjectionY()->Clone());
        style(dr, id);

        dr->Draw();
        legend->Draw();
    }
}

void plotQCDTemplates()
{
    TH1 *htlep_s1 = merge("htlep", input_s1, 0, QCD_CHANNELS, true);
    TH1 *htlep_s2 = merge("htlep", input_s2, 0, QCD_CHANNELS, true);
    TH1 *htlep_signal = merge("htlep", input_signal, 0, QCD_CHANNELS, true);

    TH1 *mttbar_before_htlep_s1 =
        merge("mttbar_before_htlep", input_s1, 0, QCD_CHANNELS, true);
    TH1 *mttbar_before_htlep_s2 =
        merge("mttbar_before_htlep", input_s2, 0, QCD_CHANNELS, true);
    TH1 *mttbar_before_htlep_signal =
        merge("mttbar_before_htlep", input_signal, 0, QCD_CHANNELS, true);

    TH1 *mttbar_after_htlep_s1 =
        merge("mttbar_after_htlep", input_s1, 0, QCD_CHANNELS, true);
    TH1 *mttbar_after_htlep_s2 = 
        merge("mttbar_after_htlep", input_s2, 0, QCD_CHANNELS, true);
    TH1 *mttbar_after_htlep_signal =
        merge("mttbar_after_htlep", input_signal, 0, QCD_CHANNELS, true);
 
    string canvas_title = "QCD Templates";
    TCanvas *canvas = new TCanvas(canvas_title.c_str(), canvas_title.c_str());
    canvas->SetWindowSize(1200, 480);
    canvas->Divide(3);

    canvas->cd(1);
    htlep_s1->SetLineColor(4);
    htlep_s1->SetMarkerColor(4);
    
    htlep_s2->SetLineColor(2);
    htlep_s2->SetMarkerColor(2);

    htlep_signal->SetLineColor(1);
    htlep_signal->SetMarkerColor(1);

    THStack *stack = new THStack();
    stack->Add(htlep_s1);
    stack->Add(htlep_s2);
    stack->Add(htlep_signal);

    stack->Draw("nostack h");

    TLegend *legend = createLegend("H_{T}^{lep}");
    legend->Draw();

    canvas->cd(2);
    mttbar_before_htlep_s1->SetLineColor(4);
    mttbar_before_htlep_s1->SetMarkerColor(4);
    
    mttbar_before_htlep_s2->SetLineColor(2);
    mttbar_before_htlep_s2->SetMarkerColor(2);

    mttbar_before_htlep_signal->SetLineColor(1);
    mttbar_before_htlep_signal->SetMarkerColor(1);

    stack = new THStack();
    stack->Add(mttbar_before_htlep_s1);
    stack->Add(mttbar_before_htlep_s2);
    stack->Add(mttbar_before_htlep_signal);

    stack->Draw("nostack h");

    legend = createLegend("M_{t#bar{t}} before H_{T}^{lep}");
    legend->Draw();

    canvas->cd(3);
    mttbar_after_htlep_s1->SetLineColor(4);
    mttbar_after_htlep_s1->SetMarkerColor(4);
    
    mttbar_after_htlep_s2->SetLineColor(2);
    mttbar_after_htlep_s2->SetMarkerColor(2);

    mttbar_after_htlep_signal->SetLineColor(1);
    mttbar_after_htlep_signal->SetMarkerColor(1);

    stack = new THStack();
    stack->Add(mttbar_after_htlep_s1);
    stack->Add(mttbar_after_htlep_s2);
    stack->Add(mttbar_after_htlep_signal);

    stack->Draw("nostack h");

    legend = createLegend("M_{t#bar{t}} after H_{T}^{lep}");
    legend->Draw();
}

void plotComparison(TFile **input, const string &title, const bool &reverse_order = false)
{
    TH1 *qcd = merge("htlep", input, 0, QCD_CHANNELS);
    TH1 *ttjets = get("htlep", input[TTJETS], TTJETS);
    TH1 *zjets = get("htlep", input[ZJETS], ZJETS);
    TH1 *wjets = get("htlep", input[WJETS], WJETS);
    TH1 *data = merge("htlep", input, RERECO, RERECO + DATA_CHANNELS);
    data->SetFillColor(0);
    
    THStack *stack = new THStack();
    stack->Add(qcd);
    stack->Add(ttjets);
    stack->Add(zjets);
    stack->Add(wjets);

    if (reverse_order)
    {
        stack->Draw("h");
        data->Draw("h same");
    }
    else
    {
        data->Draw("h");
        stack->Draw("h same");
    }

    TLegend *legend = createLegend(title.c_str());
    legend->Draw();
}

void plotDataMCComparison()
{
    string canvas_title = "Data/MC Comparison";
    TCanvas *canvas = new TCanvas(canvas_title.c_str(), canvas_title.c_str());
    canvas->SetWindowSize(800, 480);
    canvas->Divide(2);

    canvas->cd(1);
    plotComparison(input_s1s2_pt50, "H_{T}^{lep} p_{T}^{jet} > 50", true);

    canvas->cd(2);
    plotComparison(input_s1s2_pt250, "H_{T}^{lep}, p_{T}^{jet} > 250");
}

void plotQCDHtlep(TFile **input, const string &title)
{
    string canvas_title = string("QCD S1+S2 Separate ") + title;
    TCanvas *canvas = new TCanvas(canvas_title.c_str(), canvas_title.c_str());
    canvas->SetWindowSize(1200, 640);
    canvas->Divide(3, 2);

    for(int i = 0; QCD_CHANNELS > i; ++i)
    {
        canvas->cd(i + 1)->SetLeftMargin(10);
        get("htlep", input[i], i)->Draw();
        createLegend(toString(i))->Draw();
    }

    canvas->Update();

    canvas_title = string("QCD S1+S2 Merged ") + title;
    canvas = new TCanvas(canvas_title.c_str(), canvas_title.c_str());
    canvas->SetWindowSize(800, 480);
    canvas->Divide(2);

    for(int i = 0; 2 > i; ++i)
    {
        canvas->cd(i + 1)->SetLeftMargin(10);
        merge("htlep", input, i * 3, i * 3 + 3)->Draw();
        createLegend(i ? "QCD EMEnriched" : "QCD BCtoE")->Draw();
    }

    canvas->Update();
}

void plotBGHtlep(TFile **input, const string &title)
{
    plotQCDHtlep(input, title);

    string canvas_title = string("BG S1+S2 Separate ") + title;
    TCanvas *canvas = new TCanvas(canvas_title.c_str(), canvas_title.c_str());
    canvas->SetWindowSize(1200, 480);
    canvas->Divide(3);

    for(int i = QCD_CHANNELS, j = 1; BACKGROUND_CHANNELS > i; ++i, ++j)
    {
        canvas->cd(j)->SetLeftMargin(10);
        get("htlep", input[i], i)->Draw();
        createLegend(toString(i))->Draw();
    }

    canvas->Update();
}

void plotHtlep50()
{
    plotBGHtlep(input_s1s2_pt50, "(leading jet pt > 50)");
}

void plotHtlep250()
{
    plotBGHtlep(input_s1s2_pt250, "(leading jet pt > 250)");
}

void fig()
{
    TGaxis::SetMaxDigits(3);

    loadFiles();

    plotCut2DBackground();
    plotCut2DSignal();

    //plotQCDTemplates();
    //plotDataMCComparison();

    //plotHtlep50();
    //plotHtlep250();
}
