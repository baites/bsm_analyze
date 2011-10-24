float luminosity = 3393.157;
//float luminosity = 216.200;

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
    STOP_S,
    STOP_T,
    STOP_TW,
    SATOP_S,
    SATOP_T,
    SATOP_TW,
    RERECO_2011A_MAY10,
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
        case QCD_BC_PT20_30: return  "QCD BC pt20to30";
        case QCD_BC_PT30_80: return  "QCD BC pt30to80";
        case QCD_BC_PT80_170: return "QCD BC pt80to170";
        case QCD_EM_PT20_30: return  "QCD EM pt20to30";
        case QCD_EM_PT30_80: return  "QCD EM pt30to80";
        case QCD_EM_PT80_170: return "QCD EM pt80to170";
        case TTJETS: return "TTjets";
        case ZJETS: return "Zjets";
        case WJETS: return "Wjets";
        case STOP_S: return "Single-Top S";
        case STOP_T: return "Single-Top T";
        case STOP_TW: return "Single-Top TW";
        case SATOP_S: return "Single-aTop S";
        case SATOP_T: return "Single-aTop T";
        case SATOP_TW: return "Single-aTop TW";
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
        case STOP_S:
            {
                color = kMagenta + 1;
                break;
            }
        case STOP_T:
            {
                color = kMagenta + 2;
                break;
            }
        case STOP_TW:
            {
                color = kMagenta + 3;
                break;
            }
        case SATOP_S:
            {
                color = kMagenta - 4;
                break;
            }
        case SATOP_T:
            {
                color = kMagenta - 3;
                break;
            }
        case SATOP_TW:
            {
                color = kMagenta - 2;
                break;
            }
        case TTJETS:
            {
                color = kRed + 1;
                break;
            }
        case ZJETS:
            {
                color = kBlue - 4;
                break;
            }
        case WJETS:
            {
                color = kGreen + 1;
                break;
            }
        case RERECO_2011A_MAY10:
        case RERECO_2011A_AUG05:
        case PROMPT_2011A_V4:
        case PROMPT_2011A_V6:
        case PROMPT_2011B_V1:
            break;
        default:
            {
                cerr << "unknown type: can not style the plot" << endl;

                return;
            }
    }

    hist->SetLineColor(color);
    hist->SetMarkerColor(color);
    hist->SetMarkerSize(0.5);

    if (1 != color)
        hist->SetFillColor(color);
}

void styleData(TH1 *hist, const InputType &input_type)
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
    hist->SetFillColor(color);
    hist->SetMarkerColor(color);
    hist->SetMarkerSize(0.5);
}

void scale(TH1 *hist, const InputType &input_type)
{
    if (!hist->GetEntries())
        return;

    float scale = 1;
    switch(input_type)
    {
        case QCD_BC_PT20_30:
            {
                scale = 2.361e8 * luminosity * 5.9e-4 / 2081560;
                break;
            }

        case QCD_BC_PT30_80:
            {
                scale = 5.944e7 * luminosity * 2.42e-3 / 2030033;
                break;
            }

        case QCD_BC_PT80_170:
            {
                scale = 8.982e5 * luminosity * 1.05e-2 / 1082691;
                break;
            }

        case QCD_EM_PT20_30:
            {
                scale = 2.361e8 * luminosity * 1.06e-2 / 35729669;
                break;
            }

        case QCD_EM_PT30_80:
            {
                scale = 5.944e7 * luminosity * 6.1e-2 / 70392060;
                break;
            }

        case QCD_EM_PT80_170:
            {
                scale = 8.982e5 * luminosity * 1.59e-1 / 8150672;
                break;
            }

        case TTJETS:
            {
                // Use NLO x-section: 157.5 instead of LO: 94.76
                //
                scale = 157.5 * luminosity * 1.0 / 3701947;
                break;
            }

        case ZJETS:
            {
                // Use NLO x-section: 3048 instead of LO: 2475
                //
                scale = 3048 * luminosity * 1.0 / 36277961;
                break;
            }

        case WJETS:
            {
                // Use NLO x-section: 31314 instead of LO: 27770
                //
                scale = 31314 * luminosity * 1.0 / 77105816;
                break;
            }

        case STOP_S:
            {
                scale = 3.19 * luminosity * 1.0 / 259971;
                break;
            }

        case STOP_T:
            {
                scale = 41.92 * luminosity * 1.0 / 3900171;
                break;
            }

        case STOP_TW:
            {
                scale = 7.87 * luminosity * 1.0 / 814390;
                break;
            }

        case SATOP_S:
            {
                scale = 1.44 * luminosity * 1.0 / 137980;
                break;
            }

        case SATOP_T:
            {
                scale = 22.65 * luminosity * 1.0 / 1944826;
                break;
            }

        case SATOP_TW:
            {
                scale = 7.87 * luminosity * 1.0 / 809984;
                break;
            }

        case RERECO_2011A_MAY10: // Fall through
        case RERECO_2011A_AUG05: // Fall through
        case PROMPT_2011A_V4: // Fall through
        case PROMPT_2011A_V6: // Fall through
        case PROMPT_2011B_V1: // Do nothing
            break;

        default:
            {
                cerr << "unknown type: can not scale the plot" << endl;
                break;
            }
    }

    if (1 != scale)
    {
        cout << "scale " << toString(input_type) << " by " << scale << endl;
    
        hist->Scale(scale);
    }
}

TLegend *createLegend(const string &text)
{
    TLegend *legend = new TLegend( .6, .85, .85, .5);
    if (!text.empty())
        legend->SetHeader(text.c_str());

    legend->SetMargin(0.12);
    legend->SetTextSize(0.04);
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

    hist->Rebin(10);

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
    "ttjets",
    "zjets",
    "wjets",
    "stop_s",
    "stop_t",
    "stop_tw",
    "satop_s",
    "satop_t",
    "satop_tw",
    "golden_single_el_2011a_may10_rereco",
    "golden_single_el_2011a_aug5_rereco_v1",
    "golden_single_el_2011a_prompt_v4",
    "golden_single_el_2011a_prompt_v6",
    "golden_single_el_2011b_prompt_v1"
};

const int SIGNAL_CHANELS = 4;
const int QCD_CHANNELS = 6;
const int STOP_CHANNELS = 6;
const int BG_CHANNELS = UNKNOWN - SIGNAL_CHANELS;
const int CHANNELS = UNKNOWN;

TFile *input_s1s2_p50[CHANNELS];
TFile *input_s1s2_p250[CHANNELS];

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
    for(int i = 0; CHANNELS > i; ++i)
    {
        TFile *file = open(folder[i] + "/output_signal_p50_hlt.root");
        if (!file)
            return;

        input_s1s2_p50[i] = file;

        file = open(folder[i] + "/output_signal_p250_hlt.root");
        if (!file)
            return;

        input_s1s2_p250[i] = file;
    }
}

void plotComparison(TFile **input, const string &title, const bool &draw_mc_first = false)
{
    TH1 *ttjets = get(input[TTJETS], "mttbar_after_htlep", TTJETS);
    scale(ttjets, TTJETS);

    TH1 *zjets = get(input[ZJETS], "mttbar_after_htlep", ZJETS);
    scale(zjets, ZJETS);

    TH1 *wjets = get(input[WJETS], "mttbar_after_htlep", WJETS);
    scale(wjets, WJETS);
    
    TH1 *stop = merge(input, "mttbar_after_htlep", STOP_S, STOP_S + STOP_CHANNELS);
    TH1 *qcd = merge(input, "mttbar_after_htlep", 0, QCD_CHANNELS);

    TH1 *data = merge(input, "mttbar_after_htlep", RERECO_2011A_MAY10, RERECO_2011A_MAY10 + SIGNAL_CHANELS);
    //TH1 *data = merge(input, "mttbar_after_htlep", RERECO_2011A_MAY10, RERECO_2011A_MAY10 + 1);
    
    THStack *stack = new THStack();
    stack->Add(ttjets);
    stack->Add(wjets);
    stack->Add(zjets);
    stack->Add(stop);
    stack->Add(qcd);

    if (draw_mc_first)
    {
        stack->Draw("hist");
        data->Draw("same");
    }
    else
    {
        data->Draw("");
        stack->Draw("hist same");
        data->Draw("same");
    }

    stack->GetHistogram()->GetXaxis()->SetTitle("H_{T}^{lep} [GeV]");

    TLegend *legend = createLegend(title);
    legend->AddEntry(ttjets, "t#bar{t}", "fe");
    legend->AddEntry(wjets, "W#rightarrowl#nu", "fe");
    legend->AddEntry(zjets, "Z/#gamma*#rightarrowl^{+}l^{-}", "fe");
    legend->AddEntry(stop, "Single-Top", "fe");
    legend->AddEntry(qcd, "QCD", "fe");
    legend->AddEntry(data, "CMS data 2011", "lpe");
    legend->Draw();
}

void plotDataMcComparison()
{
    string canvas_title = "Data/MC Comparison";
    TCanvas *canvas = new TCanvas(canvas_title.c_str(), canvas_title.c_str());
    canvas->SetWindowSize(800, 480);
    canvas->Divide(2);

    canvas->cd(1);
    plotComparison(input_s1s2_p250, "p_{T}^{jet} > 250");

    canvas->cd(2);
    plotComparison(input_s1s2_p50, "p_{T}^{jet} > 50");
}

void plotData(TFile **input, const string &title)
{
    TH1 *rereco_2011a_may10 = get(input[RERECO_2011A_MAY10], "mttbar_after_htlep", RERECO_2011A_MAY10);
    TH1 *rereco_2011a_aug05 = get(input[RERECO_2011A_AUG05], "mttbar_after_htlep", RERECO_2011A_AUG05);
    TH1 *prompt_2011a_v4 = get(input[PROMPT_2011A_V4], "mttbar_after_htlep", PROMPT_2011A_V4);
    TH1 *prompt_2011a_v6 = get(input[PROMPT_2011A_V6], "mttbar_after_htlep", PROMPT_2011A_V6);
    TH1 *prompt_2011b_v1 = get(input[PROMPT_2011B_V1], "mttbar_after_htlep", PROMPT_2011B_V1);

    styleData(rereco_2011a_may10, RERECO_2011A_MAY10);
    styleData(rereco_2011a_aug05, RERECO_2011A_AUG05);
    styleData(prompt_2011a_v4, PROMPT_2011A_V4);
    styleData(prompt_2011a_v6, PROMPT_2011A_V6);
    styleData(prompt_2011b_v1, PROMPT_2011B_V1);
    
    THStack *stack = new THStack();
    stack->Add(rereco_2011a_may10);
    stack->Add(rereco_2011a_aug05);
    stack->Add(prompt_2011a_v4);
    stack->Add(prompt_2011a_v6);
    stack->Add(prompt_2011b_v1);

    stack->Draw("hist");
    stack->GetHistogram()->GetXaxis()->SetTitle("H_{T}^{lep} [GeV/c^{2}]");

    TLegend *legend = createLegend(title);
    legend->AddEntry(rereco_2011a_may10, toString(RERECO_2011A_MAY10).c_str(), "fe");
    legend->AddEntry(rereco_2011a_aug05, toString(RERECO_2011A_AUG05).c_str(), "fe");
    legend->AddEntry(prompt_2011a_v4, toString(PROMPT_2011A_V4).c_str(), "fe");
    legend->AddEntry(prompt_2011a_v6, toString(PROMPT_2011A_V6).c_str(), "fe");
    legend->AddEntry(prompt_2011b_v1, toString(PROMPT_2011B_V1).c_str(), "fe");
    legend->Draw();
}

void plotMC(TFile **input, const string &title)
{
    TH1 *ttjets = get(input[TTJETS], "mttbar_after_htlep", TTJETS);
    scale(ttjets, TTJETS);

    TH1 *zjets = get(input[ZJETS], "mttbar_after_htlep", ZJETS);
    scale(zjets, ZJETS);

    TH1 *wjets = get(input[WJETS], "mttbar_after_htlep", WJETS);
    scale(wjets, WJETS);

    TH1 *stop = merge(input, "mttbar_after_htlep", STOP_S, STOP_S + STOP_CHANNELS);
    TH1 *qcd = merge(input, "mttbar_after_htlep", 0, QCD_CHANNELS);
    
    THStack *stack = new THStack();
    stack->Add(ttjets);
    stack->Add(wjets);
    stack->Add(zjets);
    stack->Add(stop);
    stack->Add(qcd);

    stack->Draw("hist");
    stack->GetHistogram()->GetXaxis()->SetTitle("H_{T}^{lep} [GeV/c^{2}]");

    TLegend *legend = createLegend(title);
    legend->AddEntry(ttjets, "t#bar{t}", "fe");
    legend->AddEntry(wjets, "W#rightarrowl#nu", "fe");
    legend->AddEntry(zjets, "Z/#gamma*#rightarrowl^{+}l^{-}", "fe");
    legend->AddEntry(stop, "Single-Top", "fe");
    legend->AddEntry(qcd, "QCD", "fe");
    legend->Draw();
}

void plotDataComparison()
{
    string canvas_title = "Data Comparison";
    TCanvas *canvas = new TCanvas();
    canvas->SetTitle(canvas_title.c_str());
    canvas->SetWindowSize(800, 480);
    canvas->Divide(2);

    canvas->cd(1);
    plotData(input_s1s2_p250, "p_{T}^{jet} > 250");

    canvas->cd(2);
    plotData(input_s1s2_p50, "p_{T}^{jet} > 50");
}

void plotMCComparison()
{
    string canvas_title = "MC Comparison";
    TCanvas *canvas = new TCanvas();
    canvas->SetTitle(canvas_title.c_str());
    canvas->SetWindowSize(800, 480);
    canvas->Divide(2);

    canvas->cd(1);
    plotMC(input_s1s2_p250, "p_{T}^{jet} > 250");

    canvas->cd(2);
    plotMC(input_s1s2_p50, "p_{T}^{jet} > 50");
}

void data_mc_comparison()
{
    TGaxis::SetMaxDigits(3);
    gStyle->SetOptStat(kFALSE);

    loadFiles();
    plotDataComparison();
    plotMCComparison();
    plotDataMcComparison();
}
