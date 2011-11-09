#include <iomanip>

float luminosity = 3393.157;
//float luminosity = 2039.049;

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
                scale = 0.1 / 207992;
                break;
            }

        case ZPRIME1500:
            {
                scale = 0.1 / 168383;
                break;
            }

        case ZPRIME2000:
            {
                scale = 0.1 / 179315;
                break;
            }

        case ZPRIME3000:
            {
                scale = 0.1 / 195410;
                break;
            }

        case ZPRIME4000:
            {
                scale = 0.1 / 180381;
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

    hist->Scale(scale_with_luminosity);
}

TLegend *createLegend(const string &text)
{
    TLegend *legend = new TLegend( .6, .4, .8, .75);
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
    TLegend *legend = new TLegend(.35, .78, .85, .88);
    ostringstream title;
    title << "#splitline{CMS Preliminary 2011}{"
        << std::setprecision(2) << fixed << luminosity / 1000
        << " fb-1 at #sqrt{s}=7 TeV/c^{2}, e+jets}";
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

    style(hist, input_type);

    return hist;
}

TH1 *merge(TFile **input, const string &path, const int &from, const int &to)
{
    TH1 *result = 0;
    cout << "--- merge from: " << toString(from) << " till " << toString(to) << endl;
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
    cout << "--- done ---" << endl;

    return result;
}

const int QCD_CHANNELS = 6;
const int STOP_CHANNELS = 6;
const int SIGNAL_CHANNELS = 5;
const int ZPRIME_CHANNELS = 5;
const int CHANNELS = UNKNOWN;

TFile *input[CHANNELS];

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
        TFile *file = open(folder(i) + "/output_signal_p250_hlt.root");
        if (!file)
            return;

        input[i] = file;
    }
}

void save(const string &plot_name, const string &destination)
{
    TH1 *hist = get(input[TTJETS], plot_name.c_str(), TTJETS);
    scale(hist, TTJETS);
    hist->Write(("el_" + destination + "__ttjets").c_str());

    hist = get(input[ZJETS], plot_name.c_str(), ZJETS);
    scale(hist, ZJETS);
    hist->Write(("el_" + destination + "__zjets").c_str());

    hist = get(input[WJETS], plot_name.c_str(), WJETS);
    scale(hist, WJETS);
    hist->Write(("el_" + destination + "__wjets").c_str());
    
    hist = merge(input, plot_name.c_str(), STOP_S, STOP_S + STOP_CHANNELS);
    hist->Write(("el_" + destination + "__singletop").c_str());

    hist = merge(input, plot_name.c_str(), 0, QCD_CHANNELS);
    hist->Write(("el_" + destination + "__eleqcd").c_str());

    hist = merge(input,
            plot_name.c_str(),
            RERECO_2011A_MAY10,
            RERECO_2011A_MAY10 + SIGNAL_CHANNELS);
    hist->Write(("el_" + destination + "__DATA").c_str());

    hist = get(input[ZPRIME1000], plot_name.c_str(), ZPRIME1000);
    scale(hist, ZPRIME1000);
    hist->Write(("el_" + destination + "__zp1000").c_str());

    hist = get(input[ZPRIME1500], plot_name.c_str(), ZPRIME1500);
    scale(hist, ZPRIME1500);
    hist->Write(("el_" + destination + "__zp1500").c_str());

    hist = get(input[ZPRIME2000], plot_name.c_str(), ZPRIME2000);
    scale(hist, ZPRIME2000);
    hist->Write(("el_" + destination + "__zp2000").c_str());

    hist = get(input[ZPRIME3000], plot_name.c_str(), ZPRIME3000);
    scale(hist, ZPRIME3000);
    hist->Write(("el_" + destination + "__zp3000").c_str());

    hist = get(input[ZPRIME4000], plot_name.c_str(), ZPRIME4000);
    scale(hist, ZPRIME4000);
    hist->Write(("el_" + destination + "__zp4000").c_str());
}

void theta_input()
{
    TGaxis::SetMaxDigits(3);
    gStyle->SetOptStat(kFALSE);

    loadFiles();

    TFile *output = TFile::Open("theta_input.root", "recreate");
    if (!output
            || !output->IsOpen())
    {
        cerr << "failed to open output file" << endl;

        return;
    }

    save("htlep_before_htlep", "htlep");
    save("mttbar_after_htlep", "mttbar");

    output->Close();
}
