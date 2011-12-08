#include <iomanip>

float luminosity = 4330;
//float luminosity = 4061.545;
//float luminosity = 3393.157;
//float luminosity = 2039.049;
string plot_name = "mttbar_after_htlep";
string subtitle = "";
int rebin = 100;
bool plot_zprime = true;

const float rebin_x = 5;
const float rebin_y = 2;

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
    ZPRIME1000,
    ZPRIME1500,
    ZPRIME2000,
    ZPRIME3000,
    ZPRIME4000,
    UNKNOWN
};

string getOutputFilename(string filename)
{
    for(;;)
    {
        size_t pos = filename.find("/");
        if (string::npos == pos)
            break;
        else
            filename = filename.substr(0, pos) + "_" + filename.substr(pos + 1);
    }

    return filename;
}

string folder(const InputType &input)
{
    switch(input)
    {
        case QCD_BC_PT20_30: return "qcd_bc_pt20to30";
        case QCD_BC_PT30_80: return "qcd_bc_pt30to80";
        case QCD_BC_PT80_170: return "qcd_bc_pt80to170";
        case QCD_EM_PT20_30: return "qcd_em_pt20to30";
        case QCD_EM_PT30_80: return "qcd_em_pt30to80";
        case QCD_EM_PT80_170: return "qcd_em_pt80to170";
        case TTJETS: return "ttjets";
        case ZJETS: return "zjets";
        case WJETS: return "wjets";
        case STOP_S: return "stop_s";
        case STOP_T: return "stop_t";
        case STOP_TW: return "stop_tw";
        case SATOP_S: return "satop_s";
        case SATOP_T: return "satop_t";
        case SATOP_TW: return "satop_tw";
        case RERECO_2011A_MAY10: return "golden_single_el_2011a_may10_rereco";
        case RERECO_2011A_AUG05: return "golden_single_el_2011a_aug5_rereco_v1";
        case PROMPT_2011A_V4: return "golden_single_el_2011a_prompt_v4";
        case PROMPT_2011A_V6: return "golden_single_el_2011a_prompt_v6";
        case PROMPT_2011B_V1: return "golden_single_el_2011b_prompt_v1";
        case ZPRIME1000: return "zprime_m1000_w10";
        case ZPRIME1500: return "zprime_m1500_w15";
        case ZPRIME2000: return "zprime_m2000_w20";
        case ZPRIME3000: return "zprime_m3000_w30";
        case ZPRIME4000: return "zprime_m4000_w40";
        default:
        {
            cerr << "unsupported input" << endl;

            return "";
        }
    }
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
        case ZPRIME1000: return "Z' m1000 w10";
        case ZPRIME1500: return "Z' m1500 w15";
        case ZPRIME2000: return "Z' m2000 w20";
        case ZPRIME3000: return "Z' m3000 w30";
        case ZPRIME4000: return "Z' m4000 w40";
        default: return "Unknown";
    }
}

void style(TH1 *hist, const InputType &input_type)
{
    int color = 1;
    bool is_fill = true;
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
        case ZPRIME1000:
            {
                color = kBlack;
                hist->SetLineWidth(2);
                hist->SetLineStyle(2);
                is_fill = false;
                break;
            }
        case ZPRIME1500:
            {
                color = kViolet + 2;
                is_fill = false;
                break;
            }
        case ZPRIME2000:
            {
                color = kBlack;
                hist->SetLineWidth(2);
                hist->SetLineStyle(9);
                is_fill = false;
                break;
            }
        case ZPRIME3000:
            {
                color = kBlack;
                hist->SetLineWidth(2);
                is_fill = false;
                break;
            }
        case ZPRIME4000:
            {
                color = kOrange;
                hist->SetLineWidth(2);
                hist->SetLineStyle(9);
                is_fill = false;
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

    if (is_fill)
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
    {
        cout << "skip scale " << toString(input_type) << ": no entries" << endl;

        return;
    }

    float scale = 1;
    switch(input_type)
    {
        case QCD_BC_PT20_30:
            {
                scale = 2.361e8 * 5.9e-4 / 2081560;
                break;
            }

        case QCD_BC_PT30_80:
            {
                scale = 5.944e7 * 2.42e-3 / 2030033;
                break;
            }

        case QCD_BC_PT80_170:
            {
                scale = 8.982e5 * 1.05e-2 / 1082691;
                break;
            }

        case QCD_EM_PT20_30:
            {
                scale = 2.361e8 * 1.06e-2 / 35729669;
                break;
            }

        case QCD_EM_PT30_80:
            {
                scale = 5.944e7 * 6.1e-2 / 70392060;
                break;
            }

        case QCD_EM_PT80_170:
            {
                scale = 8.982e5 * 1.59e-1 / 8150672;
                break;
            }

        case TTJETS:
            {
                // Use NLO x-section: 157.5 instead of LO: 94.76
                //
                scale = 157.5 * 1.0 / 3701947;
                break;
            }

        case ZJETS:
            {
                // Use NLO x-section: 3048 instead of LO: 2475
                //
                scale = 3048 * 1.0 / 36277961;
                break;
            }

        case WJETS:
            {
                // Use NLO x-section: 31314 instead of LO: 27770
                //
                scale = 31314 * 1.0 / 77105816;
                break;
            }

        case STOP_S:
            {
                scale = 3.19 * 1.0 / 259971;
                break;
            }

        case STOP_T:
            {
                scale = 41.92 * 1.0 / 3900171;
                break;
            }

        case STOP_TW:
            {
                scale = 7.87 * 1.0 / 814390;
                break;
            }

        case SATOP_S:
            {
                scale = 1.44 * 1.0 / 137980;
                break;
            }

        case SATOP_T:
            {
                scale = 22.65 * 1.0 / 1944826;
                break;
            }

        case SATOP_TW:
            {
                scale = 7.87 * 1.0 / 809984;
                break;
            }

        case ZPRIME1000:
            {
                scale = 10.0 / 207992;
                break;
            }

        case ZPRIME1500:
            {
                scale = 10.0 / 168383;
                break;
            }

        case ZPRIME2000:
            {
                scale = 10.0 / 179315;
                break;
            }

        case ZPRIME3000:
            {
                scale = 10.0 / 195410;
                break;
            }

        case ZPRIME4000:
            {
                scale = 10.0 / 180381;
                break;
            }

        case RERECO_2011A_MAY10: // Fall through
        case RERECO_2011A_AUG05: // Fall through
        case PROMPT_2011A_V4: // Fall through
        case PROMPT_2011A_V6: // Fall through
        case PROMPT_2011B_V1: // Do nothing
            return;

        default:
            {
                cerr << "unknown type: can not scale the plot" << endl;

                return;
            }
    }

    float scale_with_luminosity = scale * luminosity;

    /*
    cout << "scale " << toString(input_type)
        << " raw: " << scale
        << " with lumi: " << scale_with_luminosity << endl;
        */

    hist->Scale(scale_with_luminosity);
}

TLegend *createLegend(const string &text)
{
    TLegend *legend = new TLegend( .68, .53, .88, .88);
    if (!text.empty())
        legend->SetHeader(text.c_str());

    legend->SetMargin(0.12);
    legend->SetTextSize(0.03);
    legend->SetFillColor(10);
    legend->SetBorderSize(0);

    return legend;
}

void cmsLabel()
{
    TLegend *legend = new TLegend(.16, .92, .81, .97);
    ostringstream title;
    title << "CMS Preliminary 2011, "
        << std::setprecision(2) << fixed << luminosity / 1000
        << " fb-1 at #sqrt{s}=7 TeV/c^{2}, e+jets";
    legend->SetHeader(title.str().c_str());

    legend->SetMargin(0.12);
    legend->SetTextSize(0.04);
    legend->SetFillColor(10);
    legend->SetBorderSize(0);
    
    legend->Draw();
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

    if (1 != rebin)
        hist->Rebin(rebin);

    style(hist, input_type);

    return hist;
}

TH2 *get2D(const TFile *input, const string &path, const InputType &input_type)
{   
    TObject *object = input->Get(path.c_str());
    if (!object)
    {
        cerr << "failed to get: " << path << endl;

        return 0;
    }

    TH2 *hist = dynamic_cast<TH2 *>(object->Clone());
    if (!hist)
    {
        cerr << "object does not seem to be TH2" << endl;

        return 0;
    }

    if (1 != rebin)
        hist->Rebin(rebin);

    style(hist, input_type);

    return hist;
}

TH1 *merge(TFile **input, const string &path, const int &from, const int &to)
{
    TH1 *result = 0;
    //cout << "--- merge from: " << toString(from) << " till " << toString(to) << endl;
    for(int i = from; to > i; ++i)
    {
        TH1 *hist = get(input[i], path, i);
        if (!hist)
        {
            cerr << "failed to extract: " << path << endl;

            continue;
        }

        scale(hist, i);

        if (result)
            result->Add(hist);
        else
            result = dynamic_cast<TH1 *>(hist->Clone());
    }
    //cout << "--- done ---" << endl;

    return result;
}

TH2 *merge2D(TFile **input, const string &path, const int &from, const int &to)
{
    TH2 *result = 0;
    //cout << "--- merge from: " << toString(from) << " till " << toString(to) << endl;
    for(int i = from; to > i; ++i)
    {
        TH2 *hist = get(input[i], path, i);
        if (!hist)
        {
            cerr << "failed to extract: " << path << endl;

            continue;
        }

        scale(hist, i);

        if (result)
            result->Add(hist);
        else
            result = dynamic_cast<TH2 *>(hist->Clone());
    }
    //cout << "--- done ---" << endl;

    return result;
}

const int QCD_CHANNELS = 6;
const int STOP_CHANNELS = 6;
//const int SIGNAL_CHANNELS = 4;
const int SIGNAL_CHANNELS = 5;
const int ZPRIME_CHANNELS = 5;
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

void loadFiles(const string &dir)
{
    for(int i = 0; CHANNELS > i; ++i)
    {
        /*
        TFile *file = open(folder(i) + "/output_signal_p50_hlt.root");
        if (!file)
            return;

        input_s1s2_p50[i] = file;
        */

        TFile *file = open(dir + "/" + folder(i) + ".root");
        if (!file) return;

        input_s1s2_p250[i] = file;
    }
}

void plotComparison(TFile **input, const string &title)
{
    TH1 *ttjets = get(input[TTJETS], plot_name.c_str(), TTJETS);
    scale(ttjets, TTJETS);

    TH1 *zjets = get(input[ZJETS], plot_name.c_str(), ZJETS);
    scale(zjets, ZJETS);

    TH1 *wjets = get(input[WJETS], plot_name.c_str(), WJETS);
    scale(wjets, WJETS);
    
    TH1 *stop = merge(input, plot_name.c_str(), STOP_S, STOP_S + STOP_CHANNELS);
    TH1 *qcd = merge(input, plot_name.c_str(), 0, QCD_CHANNELS);

    TH1 *data = merge(input,
            plot_name.c_str(),
            RERECO_2011A_MAY10,
            RERECO_2011A_MAY10 + SIGNAL_CHANNELS);

    data->SetMarkerStyle(kFullCircle);
    data->SetMarkerSize(0.8);

    TH1 *zprime_m1000 = get(input[ZPRIME1000], plot_name.c_str(), ZPRIME1000);
    scale(zprime_m1000, ZPRIME1000);
    style(zprime_m1000, ZPRIME1000);

    TH1 *zprime_m2000 = get(input[ZPRIME2000], plot_name.c_str(), ZPRIME2000);
    scale(zprime_m2000, ZPRIME2000);
    style(zprime_m2000, ZPRIME2000);

    TH1 *zprime_m3000 = get(input[ZPRIME3000], plot_name.c_str(), ZPRIME3000);
    scale(zprime_m3000, ZPRIME3000);
    style(zprime_m3000, ZPRIME3000);

    TH1 *zprime_m4000 = get(input[ZPRIME4000], plot_name.c_str(), ZPRIME4000);
    scale(zprime_m4000, ZPRIME4000);
    style(zprime_m3000, ZPRIME3000);
    
    THStack *stack = new THStack();
    stack->Add(ttjets);
    stack->Add(wjets);
    stack->Add(zjets);
    stack->Add(stop);
    //stack->Add(qcd);

    if (data->GetMaximum() < stack->GetMaximum())
    {
        stack->Draw("hist");
        stack->GetHistogram()->GetXaxis()->SetTitle(data->GetXaxis()->GetTitle());
        stack->GetHistogram()->GetYaxis()->SetTitle("Events");
        stack->GetHistogram()->GetXaxis()->SetTitleFont(42);
        stack->GetHistogram()->GetXaxis()->SetTitleSize(0.04);
        stack->GetHistogram()->GetXaxis()->SetTitleOffset(1.15);

        if (plot_zprime)
        {
            zprime_m1000->Draw("hist same");
            zprime_m2000->Draw("hist same");
            zprime_m3000->Draw("hist same");
            //zprime_m4000->Draw("hist same");
        }

        data->Draw("same");
    }
    else
    {
        data->Draw("");
        data->GetYaxis()->SetTitle("Events");
        data->GetXaxis()->SetTitleFont(42);
        data->GetXaxis()->SetTitleSize(0.04);
        data->GetXaxis()->SetTitleOffset(1.15);
        stack->Draw("hist same");

        if (plot_zprime)
        {
            zprime_m1000->Draw("hist same");
            zprime_m2000->Draw("hist same");
            zprime_m3000->Draw("hist same");
            //zprime_m4000->Draw("hist same");
        }

        data->Draw("same");
    }

    TLegend *legend = createLegend(title);
    legend->AddEntry(ttjets, "t#bar{t}", "fe");
    legend->AddEntry(wjets, "W#rightarrowl#nu", "fe");
    legend->AddEntry(zjets, "Z/#gamma*#rightarrowl^{+}l^{-}", "fe");
    legend->AddEntry(stop, "Single-Top", "fe");
    //legend->AddEntry(qcd, "QCD", "fe");
    legend->AddEntry(data, "Data 2011", "lpe");

    if (plot_zprime)
    {
        legend->AddEntry(zprime_m1000, "Z' (m = 1 TeV, w 1%)", "l");
        legend->AddEntry(zprime_m2000, "Z' (m = 2 TeV, w 1%)", "l");
        legend->AddEntry(zprime_m3000, "Z' (m = 3 TeV, w 1%)", "l");
        //legend->AddEntry(zprime_m4000, "Z' (m = 4 TeV, w 1%)", "lpe");
    }

    // legend->Draw();
}

void plotDataMcComparison()
{
    string canvas_title = "Data/MC Comparison " + subtitle;
    TCanvas *canvas = new TCanvas();
    canvas->SetTitle(canvas_title.c_str());
    canvas->SetWindowSize(800, 600);

    canvas->cd(1)->SetRightMargin(10);
    canvas->cd(1)->SetTopMargin(10);
    plotComparison(input_s1s2_p250, "p_{T}^{jet} > 250 GeV/c^{2}");

    cmsLabel();

    string output = getOutputFilename(plot_name) + "_data_mc.pdf";

    cout << "save canvas: " << output << endl;
    canvas->SaveAs(output.c_str());
}

void plotData(TFile **input, const string &title)
{
    THStack *stack = new THStack();
    TLegend *legend = createLegend(title);
    TH1 *hist = 0;
    for(int i = RERECO_2011A_MAY10, max = RERECO_2011A_MAY10 + SIGNAL_CHANNELS;
            max > i;
            ++i)
    {
        hist = get(input[i], plot_name.c_str(), i);
        styleData(hist, i);
        stack->Add(hist);

        legend->AddEntry(hist, toString(i).c_str(), "fe");
    }

    stack->Draw("hist");
    stack->GetHistogram()->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());

    legend->Draw();
}

void plotMC(TFile **input, const string &title)
{
    TH1 *ttjets = get(input[TTJETS], plot_name.c_str(), TTJETS);
    scale(ttjets, TTJETS);

    TH1 *zjets = get(input[ZJETS], plot_name.c_str(), ZJETS);
    scale(zjets, ZJETS);

    TH1 *wjets = get(input[WJETS], plot_name.c_str(), WJETS);
    scale(wjets, WJETS);

    TH1 *stop = merge(input, plot_name.c_str(), STOP_S, STOP_S + STOP_CHANNELS);
    //TH1 *qcd = merge(input, plot_name.c_str(), 0, QCD_CHANNELS);
    
    THStack *stack = new THStack();
    stack->Add(ttjets);
    stack->Add(wjets);
    stack->Add(zjets);
    stack->Add(stop);
    //stack->Add(qcd);

    stack->Draw("hist");
    stack->GetHistogram()->GetXaxis()->SetTitle(ttjets->GetXaxis()->GetTitle());

    TLegend *legend = createLegend(title);
    legend->AddEntry(ttjets, "t#bar{t}", "fe");
    legend->AddEntry(wjets, "W#rightarrowl#nu", "fe");
    legend->AddEntry(zjets, "Z/#gamma*#rightarrowl^{+}l^{-}", "fe");
    legend->AddEntry(stop, "Single-Top", "fe");
    //legend->AddEntry(qcd, "QCD", "fe");
    legend->Draw();
}

void plotDataComparison()
{
    string canvas_title = "Data Comparison";
    TCanvas *canvas = new TCanvas();
    canvas->SetTitle(canvas_title.c_str());
    canvas->SetWindowSize(800, 600);
    /*
    canvas->SetWindowSize(800, 480);
    canvas->Divide(2);

    canvas->cd(1);
    */
    plotData(input_s1s2_p250, "p_{T}^{jet} > 250");

    string output = getOutputFilename(plot_name) + "_data.pdf";

    cout << "save canvas: " << output << endl;
    canvas->SaveAs(output.c_str());

    /*
    canvas->cd(2);
    plotData(input_s1s2_p50, "p_{T}^{jet} > 50");
    */
}

void plotMCComparison()
{
    string canvas_title = "MC Comparison";
    TCanvas *canvas = new TCanvas();
    canvas->SetTitle(canvas_title.c_str());
    canvas->SetWindowSize(800, 600);
    /*
    canvas->SetWindowSize(800, 480);
    canvas->Divide(2);

    canvas->cd(1);
    */
    plotMC(input_s1s2_p250, "p_{T}^{jet} > 250");

    string output = getOutputFilename(plot_name) + "_mc.pdf";

    cout << "save canvas: " << output << endl;
    canvas->SaveAs(output.c_str());
    /*

    canvas->cd(2);
    plotMC(input_s1s2_p50, "p_{T}^{jet} > 50");
    */
}

void plotDrvsPtrel(TFile **input, const string &pt)
{
    string canvas_title = "2D Cut: leading jet pT > " + pt;
    TCanvas *canvas = new TCanvas();
    canvas->SetTitle(canvas_title.c_str());
    canvas->SetWindowSize(1200, 600);
    canvas->Divide(5, 2);

    string plot_name = "dr_vs_ptrel";

    canvas->cd(1);
    TH2 *ttjets = get2D(input[TTJETS], plot_name.c_str(), TTJETS);
    scale(ttjets, TTJETS);
    ttjets->Rebin2D(5, 1);
    ttjets->Draw("colz");

    canvas->cd(2);
    TH2 *zjets = get2D(input[ZJETS], plot_name.c_str(), ZJETS);
    scale(zjets, ZJETS);
    zjets->Rebin2D(5, 1);
    zjets->Draw("colz");

    canvas->cd(3);
    TH2 *wjets = get2D(input[WJETS], plot_name.c_str(), WJETS);
    scale(wjets, WJETS);
    wjets->Rebin2D(5, 1);
    wjets->Draw("colz");

    canvas->cd(4);
    TH2 *stop = merge2D(input, plot_name.c_str(), STOP_S, STOP_S + STOP_CHANNELS);
    stop->Rebin2D(5, 1);
    stop->Draw("colz");

    canvas->cd(5);
    TH2 *qcd = merge2D(input, plot_name.c_str(), 0, QCD_CHANNELS);
    qcd->Rebin2D(5, 1);
    qcd->Draw("colz");
    
    canvas->cd(6);
    TH2 *zprime_m1000 = get2D(input[ZPRIME1000], plot_name.c_str(), ZPRIME1000);
    style(zprime_m1000, ZPRIME1000);
    zprime_m1000->Rebin2D(5, 1);
    zprime_m1000->Draw("colz");

    canvas->cd(7);
    TH2 *zprime_m1500 = get2D(input[ZPRIME1500], plot_name.c_str(), ZPRIME1500);
    style(zprime_m1500, ZPRIME1500);
    zprime_m1500->Rebin2D(5, 1);
    zprime_m1500->Draw("colz");

    canvas->cd(8);
    TH2 *zprime_m2000 = get2D(input[ZPRIME2000], plot_name.c_str(), ZPRIME2000);
    style(zprime_m2000, ZPRIME2000);
    zprime_m2000->Rebin2D(5, 1);
    zprime_m2000->Draw("colz");

    canvas->cd(9);
    TH2 *zprime_m3000 = get2D(input[ZPRIME3000], plot_name.c_str(), ZPRIME3000);
    style(zprime_m3000, ZPRIME3000);
    zprime_m3000->Rebin2D(5, 1);
    zprime_m3000->Draw("colz");

    canvas->cd(10);
    TH2 *zprime_m4000 = get2D(input[ZPRIME4000], plot_name.c_str(), ZPRIME4000);
    style(zprime_m4000, ZPRIME4000);
    zprime_m4000->Rebin2D(5, 1);
    zprime_m4000->Draw("colz");

    string output = getOutputFilename(plot_name) + ".pdf";

    cout << "save canvas: " << output << endl;
    canvas->SaveAs(output.c_str());
}

void plot2DCut()
{
    rebin = 1;

    //plotDrvsPtrel(input_s1s2_p50, "50");
    plotDrvsPtrel(input_s1s2_p250, "250");
}

void plotPhivsMet(TFile **input, TCanvas *canvas, const string &plot_name)
{
    canvas->cd(1);
    TH2 *ttjets = get2D(input[TTJETS], plot_name.c_str(), TTJETS);
    scale(ttjets, TTJETS);
    ttjets->Rebin2D(rebin_x, rebin_y);
    ttjets->GetYaxis()->SetTitleOffset(1.7);
    ttjets->GetXaxis()->SetTitleOffset(1.15);
    ttjets->GetXaxis()->SetNdivisions(5);
    ttjets->Draw("colz");

    canvas->cd(2);
    TH2 *zjets = get2D(input[ZJETS], plot_name.c_str(), ZJETS);
    scale(zjets, ZJETS);
    zjets->Rebin2D(rebin_x, rebin_y);
    zjets->GetYaxis()->SetTitleOffset(1.7);
    zjets->GetXaxis()->SetTitleOffset(1.15);
    zjets->GetXaxis()->SetNdivisions(5);
    zjets->Draw("colz");

    canvas->cd(3);
    TH2 *wjets = get2D(input[WJETS], plot_name.c_str(), WJETS);
    scale(wjets, WJETS);
    wjets->Rebin2D(rebin_x, rebin_y);
    wjets->GetYaxis()->SetTitleOffset(1.7);
    wjets->GetXaxis()->SetTitleOffset(1.15);
    wjets->GetXaxis()->SetNdivisions(5);
    wjets->Draw("colz");

    canvas->cd(4);
    TH2 *stop = merge2D(input, plot_name.c_str(), STOP_S, STOP_S + STOP_CHANNELS);
    stop->Rebin2D(rebin_x, rebin_y);
    stop->GetYaxis()->SetTitleOffset(1.7);
    stop->GetXaxis()->SetTitleOffset(1.15);
    stop->GetXaxis()->SetNdivisions(5);
    stop->Draw("colz");

    canvas->cd(5);
    TH2 *qcd = merge2D(input, plot_name.c_str(), 0, QCD_CHANNELS);
    qcd->Rebin2D(rebin_x, rebin_y);
    qcd->GetYaxis()->SetTitleOffset(1.7);
    qcd->GetXaxis()->SetTitleOffset(1.15);
    qcd->GetXaxis()->SetNdivisions(5);
    qcd->Draw("colz");
    
    canvas->cd(6);
    TH2 *zprime_m1000 = get2D(input[ZPRIME1000], plot_name.c_str(), ZPRIME1000);
    style(zprime_m1000, ZPRIME1000);
    zprime_m1000->Rebin2D(rebin_x, rebin_y);
    zprime_m1000->GetYaxis()->SetTitleOffset(1.7);
    zprime_m1000->GetXaxis()->SetTitleOffset(1.15);
    zprime_m1000->GetXaxis()->SetNdivisions(5);
    zprime_m1000->Draw("colz");

    canvas->cd(7);
    TH2 *zprime_m1500 = get2D(input[ZPRIME1500], plot_name.c_str(), ZPRIME1500);
    style(zprime_m1500, ZPRIME1500);
    zprime_m1500->Rebin2D(rebin_x, rebin_y);
    zprime_m1500->GetYaxis()->SetTitleOffset(1.7);
    zprime_m1500->GetXaxis()->SetTitleOffset(1.15);
    zprime_m1500->GetXaxis()->SetNdivisions(5);
    zprime_m1500->Draw("colz");

    canvas->cd(8);
    TH2 *zprime_m2000 = get2D(input[ZPRIME2000], plot_name.c_str(), ZPRIME2000);
    style(zprime_m2000, ZPRIME2000);
    zprime_m2000->Rebin2D(rebin_x, rebin_y);
    zprime_m2000->GetYaxis()->SetTitleOffset(1.7);
    zprime_m2000->GetXaxis()->SetTitleOffset(1.15);
    zprime_m2000->GetXaxis()->SetNdivisions(5);
    zprime_m2000->Draw("colz");

    canvas->cd(9);
    TH2 *zprime_m3000 = get2D(input[ZPRIME3000], plot_name.c_str(), ZPRIME3000);
    style(zprime_m3000, ZPRIME3000);
    zprime_m3000->Rebin2D(rebin_x, rebin_y);
    zprime_m3000->GetYaxis()->SetTitleOffset(1.7);
    zprime_m3000->GetXaxis()->SetTitleOffset(1.15);
    zprime_m3000->GetXaxis()->SetNdivisions(5);
    zprime_m3000->Draw("colz");

    canvas->cd(10);
    TH2 *zprime_m4000 = get2D(input[ZPRIME4000], plot_name.c_str(), ZPRIME4000);
    style(zprime_m4000, ZPRIME4000);
    zprime_m4000->Rebin2D(rebin_x, rebin_y);
    zprime_m4000->GetYaxis()->SetTitleOffset(1.7);
    zprime_m4000->GetXaxis()->SetTitleOffset(1.15);
    zprime_m4000->GetXaxis()->SetNdivisions(5);
    zprime_m4000->Draw("colz");

    string output = getOutputFilename(plot_name) + ".pdf";

    cout << "save canvas: " << output << endl;
    canvas->SaveAs(output.c_str());
}

void plotLjetDphivsMet(TFile **input, const string &pt)
{
    string canvas_title = "Dphi(Leading Jet, MET) vs MET: leading jet pT > " + pt;
    TCanvas *canvas = new TCanvas();
    canvas->SetTitle(canvas_title.c_str());
    canvas->SetWindowSize(1200, 600);
    canvas->Divide(5, 2);

    string plot_name = "ljet_met_dphi_vs_met";

    plotPhivsMet(input, canvas, plot_name);

    canvas_title += " (data)";
    TCanvas *canvas = new TCanvas();
    canvas->SetTitle(canvas_title.c_str());
    canvas->SetWindowSize(640, 400);

    TH2 *data = merge2D(input, plot_name.c_str(), STOP_S, STOP_S + STOP_CHANNELS);
    TH2 *data = merge2D(input,
            plot_name.c_str(),
            RERECO_2011A_MAY10,
            RERECO_2011A_MAY10 + SIGNAL_CHANNELS);
    data->Rebin2D(rebin_x, rebin_y);
    data->SetMarkerSize(0.2);
    data->GetXaxis()->SetTitleOffset(1.15);
    data->GetXaxis()->SetNdivisions(5);
    data->Draw("colz");

    string output = getOutputFilename(plot_name) + ".pdf";

    cout << "save canvas: " << output << endl;
    canvas->SaveAs(output.c_str());
}

void plotLjetMetDphivsMet()
{
    rebin = 1;

    //plotLjetDphivsMet(input_s1s2_p50, "50");
    plotLjetDphivsMet(input_s1s2_p250, "250");
}

void plotLeptonDphivsMet(TFile **input, const string &pt)
{
    string canvas_title = "Dphi(Lepton, MET) vs MET: leading jet pT > " + pt;
    TCanvas *canvas = new TCanvas();
    canvas->SetTitle(canvas_title.c_str());
    canvas->SetWindowSize(1200, 600);
    canvas->Divide(5, 2);

    string plot_name = "lepton_met_dphi_vs_met";

    plotPhivsMet(input, canvas, plot_name);

    canvas_title += " (data)";
    TCanvas *canvas = new TCanvas();
    canvas->SetTitle(canvas_title.c_str());
    canvas->SetWindowSize(640, 400);

    TH2 *data = merge2D(input, plot_name.c_str(), STOP_S, STOP_S + STOP_CHANNELS);
    TH2 *data = merge2D(input,
            plot_name.c_str(),
            RERECO_2011A_MAY10,
            RERECO_2011A_MAY10 + SIGNAL_CHANNELS);
    data->Rebin2D(rebin_x, rebin_y);
    data->SetMarkerSize(0.2);
    data->GetXaxis()->SetTitleOffset(1.15);
    data->GetXaxis()->SetNdivisions(5);
    data->Draw("colz");

    string output = getOutputFilename(plot_name) + ".pdf";

    cout << "save canvas: " << output << endl;
    canvas->SaveAs(output.c_str());
}

void plotLeptonMetDphivsMet()
{
    rebin = 1;

    //plotLeptonDphivsMet(input_s1s2_p50, "50");
    plotLeptonDphivsMet(input_s1s2_p250, "250");
}

void TemplatePlotter(const string & dir = ".")
{
    TGaxis::SetMaxDigits(3);
    gROOT->SetStyle("Plain");
    gStyle->SetOptStat(kFALSE);

    loadFiles(dir);

    string plots[] = {
        "met",
        "mttbar_after_htlep",
        "htlep",
        "htlep_before_htlep",
        "npv",
        "npv_with_pileup",
        "njets",
        "ttbar_pt",
        "wlep_mt",
        "wlep_mass",
        "First_jet/pt",
        "First_jet/eta",
        "Second_jet/pt",
        "Second_jet/eta",
        "Third_jet/pt",
        "Third_jet/eta",
        "Electron/pt",
        "Electron/eta",
        "ltop/pt",
        "ltop/eta",
        "ltop/mass",
        "ltop/mt",
        "htop/pt",
        "htop/eta",
        "htop/mass",
        "htop/mt",
    };

    int rebins[] = {
        10,
        100,    // mttbar
        20,     // htlep
        4,     // htlep_before_htlep
        1,      // npv
        1,      // npv_with_pu
        1,      // njets
        20,     // ttbar_pt
        10,     // wlep_mt
        10,     // wlep_mass
        5,      // First Jet
        2,
        5,      // second jet
        2,
        5,      // third jet
        2,
        5,      // electron
        2,
        20,      // ltop
        2,
        10,
        20,
        20,      // htop
        2,
        10,
        20
    };

    bool data_mc_first[] = {
        false, // met
        true,    // mttbar
        true,     // htlep
        false,      // npv
        true,      // npv_with_pu
        true,      // njets
        true,     // ttbar_pt
        true,     // wlep_mt
        true,     // wlep_mass
        true,      // First Jet
        true,
        true,      // second jet
        true,
        true,      // third jet
        true,
        true,      // electron
        true,
        true,      // ltop
        true,
        true,
        true,
        true,      // htop
        true,
        true,
        true
    };

    string subtitles[] = {
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "First Jet",
        "First Jet",
        "Second Jet",
        "Second Jet",
        "Third Jet",
        "Third Jet",
        "Electron",
        "Electron",
        "Leptonic Top",
        "Leptonic Top",
        "Leptonic Top",
        "Leptonic Top",
        "Hadronic Top",
        "Hadronic Top",
        "Hadronic Top",
        "Hadronic Top"
    };

    int plots_num = 25;
    //int plots_num = 2;

    //for(int i = 0; 2 > i; ++i)
    for(int i = 0; plots_num > i; ++i)
    //for(int i = 1; false && 2 > i; ++i)
    {
        plot_name = plots[i];
        rebin = rebins[i];
        subtitle = subtitles[i];
        plotDataMcComparison();
    }

    //plotLjetMetDphivsMet();
    //plotLeptonMetDphivsMet();
}
