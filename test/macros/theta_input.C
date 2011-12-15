//float luminosity = 4061.545;
float luminosity = 4328.472;
string file_mask = "output_signal_p250_hlt";
float mc_scale = 0.96;
float qcd_scale = 1;

enum Systematic
{
    PILEUP_PLUS = 0,
    PILEUP_MINUS,
    JES_PLUS,
    JES_MINUS,
    MATCHING_PLUS,
    MATCHING_MINUS,
    SCALING_PLUS,
    SCALING_MINUS,
    NONE
};

Systematic systematic = NONE;

enum InputType
{
    QCD_BC_PT20_30 = 0,
    QCD_BC_PT30_80,
    QCD_BC_PT80_170,
    QCD_EM_PT20_30,
    QCD_EM_PT30_80,
    QCD_EM_PT80_170,
    TTJETS,
    TTJETS_SCALE_UP,
    TTJETS_SCALE_DOWN,
    TTJETS_MATCHING_UP,
    TTJETS_MATCHING_DOWN,
    ZJETS,
    WJETS,
    WJETS_SCALE_UP,
    WJETS_SCALE_DOWN,
    WJETS_MATCHING_UP,
    WJETS_MATCHING_DOWN,
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
    QCD_FROM_DATA,
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
        case TTJETS_MATCHING_UP: return "ttjets_matchingup";
        case TTJETS_MATCHING_DOWN: return "ttjets_matchingdown";
        case TTJETS_SCALE_UP: return "ttjets_scaleup";
        case TTJETS_SCALE_DOWN: return "ttjets_scaledown";
        case ZJETS: return "zjets";
        case WJETS: return "wjets";
        case WJETS_MATCHING_UP: return "wjets_matchingup";
        case WJETS_MATCHING_DOWN: return "wjets_matchingdown";
        case WJETS_SCALE_UP: return "wjets_scaleup";
        case WJETS_SCALE_DOWN: return "wjets_scaledown";
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
        case QCD_FROM_DATA: return "qcd_from_data";
        default:
        {
            cerr << "unsupported input" << endl;

            return "";
        }
    }
};

string filename()
{
    string name = "/" + file_mask;
    switch(systematic)
    {
        case PILEUP_PLUS:
            name += "_pileup_plus";
            break;
        case PILEUP_MINUS:
            name += "_pileup_minus";
            break;
        case JES_PLUS:
            name += "_jes_plus";
            break;
        case JES_MINUS:
            name += "_jes_minus";
            break;
        case SCALING_PLUS:
            name += "_scaling_plus";
            break;
        case SCALING_MINUS:
            name += "_scaling_minus";
            break;
        case MATCHING_PLUS:
            name += "_matching_plus";
            break;
        case MATCHING_MINUS:
            name += "_matching_minus";
            break;
        case NONE: // fall through
        default:
            break;
    };
    name += ".root";

    return name;
}

string systematicSuffix()
{
    switch(systematic)
    {
        case PILEUP_PLUS: return "__pileup__plus";
        case PILEUP_MINUS: return "__pileup__minus";
        case JES_PLUS: return "__jes__plus";
        case JES_MINUS: return "__jes__minus";
        case MATCHING_PLUS: return "__matching__plus";
        case MATCHING_MINUS: return "__matching__minus";
        case SCALING_PLUS: return "__scaling__plus";
        case SCALING_MINUS: return "__scaling__minus";
        case NONE: // fall through
        default:
            return "";
    };
}

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
        case TTJETS_MATCHING_UP: return "TTjets matching up";
        case TTJETS_MATCHING_DOWN: return "TTjets matching down";
        case TTJETS_SCALE_UP: return "TTjets scale up";
        case TTJETS_SCALE_DOWN: return "TTjets scale down";
        case ZJETS: return "Zjets";
        case WJETS: return "Wjets";
        case WJETS_MATCHING_UP: return "Wjets matching up";
        case WJETS_MATCHING_DOWN: return "Wjets matching down";
        case WJETS_SCALE_UP: return "Wjets scale up";
        case WJETS_SCALE_DOWN: return "Wjets scale down";
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
        case QCD_FROM_DATA: return "QCD from Data";
        default: return "Unknown";
    }
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
                scale = 2.361e8 * 5.9e-4 / 2081560 * mc_scale;
                break;
            }

        case QCD_BC_PT30_80:
            {
                scale = 5.944e7 * 2.42e-3 / 2030033 * mc_scale;
                break;
            }

        case QCD_BC_PT80_170:
            {
                scale = 8.982e5 * 1.05e-2 / 1082691 * mc_scale;
                break;
            }

        case QCD_EM_PT20_30:
            {
                scale = 2.361e8 * 1.06e-2 / 35729669 * mc_scale;
                break;
            }

        case QCD_EM_PT30_80:
            {
                scale = 5.944e7 * 6.1e-2 / 70392060 * mc_scale;
                break;
            }

        case QCD_EM_PT80_170:
            {
                scale = 8.982e5 * 1.59e-1 / 8150672 * mc_scale;
                break;
            }

        case TTJETS_SCALE_UP:
            {
                // Use NLO x-section: 157.5 instead of LO: 94.76
                //
                scale = 163 * 1.0 / 930483 * mc_scale;
                break;
            }
        case TTJETS_SCALE_DOWN:
            {
                // Use NLO x-section: 157.5 instead of LO: 94.76
                //
                scale = 163 * 1.0 / 967055 * mc_scale;
                break;
            }
        case TTJETS_MATCHING_UP:
            {
                // Use NLO x-section: 157.5 instead of LO: 94.76
                //
                scale = 163 * 1.0 / 1057479 * mc_scale;
                break;
            }
        case TTJETS_MATCHING_DOWN:
            {
                // Use NLO x-section: 157.5 instead of LO: 94.76
                //
                scale = 163 * 1.0 / 1065323 * mc_scale;
                break;
            }
        case TTJETS:
            {
                // Use NLO x-section: 157.5 instead of LO: 94.76
                //
                scale = 163 * 1.0 / 3701947 * mc_scale;
                break;
            }

        case ZJETS:
            {
                // Use NLO x-section: 3048 instead of LO: 2475
                //
                scale = 3048 * 1.0 / 36277961 * mc_scale;
                break;
            }

        case WJETS_SCALE_UP: // fall through
            {
                // Use NLO x-section: 31314 instead of LO: 27770
                //
                scale = 31314 * 1.0 / 9784907 * mc_scale;
                break;
            }
        case WJETS_SCALE_DOWN: // fall through
            {
                // Use NLO x-section: 31314 instead of LO: 27770
                //
                scale = 31314 * 1.0 / 10022324 * mc_scale;
                break;
            }
        case WJETS_MATCHING_UP: // fall through
            {
                // Use NLO x-section: 31314 instead of LO: 27770
                //
                scale = 31314 * 1.0 / 10461655 * mc_scale;
                break;
            }
        case WJETS_MATCHING_DOWN: // fall through
            {
                // Use NLO x-section: 31314 instead of LO: 27770
                //
                scale = 31314 * 1.0 / 9956679 * mc_scale;
                break;
            }
        case WJETS:
            {
                // Use NLO x-section: 31314 instead of LO: 27770
                //
                scale = 31314 * 1.0 / 77105816 * mc_scale;
                break;
            }

        case STOP_S:
            {
                scale = 3.19 * 1.0 / 259971 * mc_scale;
                break;
            }

        case STOP_T:
            {
                scale = 41.92 * 1.0 / 3900171 * mc_scale;
                break;
            }

        case STOP_TW:
            {
                scale = 7.87 * 1.0 / 814390 * mc_scale;
                break;
            }

        case SATOP_S:
            {
                scale = 1.44 * 1.0 / 137980 * mc_scale;
                break;
            }

        case SATOP_T:
            {
                scale = 22.65 * 1.0 / 1944826 * mc_scale;
                break;
            }

        case SATOP_TW:
            {
                scale = 7.87 * 1.0 / 809984 * mc_scale;
                break;
            }

        case ZPRIME1000:
            {
                scale = 1.0 / 207992 * mc_scale;
                break;
            }

        case ZPRIME1500:
            {
                scale = 1.0 / 168383 * mc_scale;
                break;
            }

        case ZPRIME2000:
            {
                scale = 1.0 / 179315 * mc_scale;
                break;
            }

        case ZPRIME3000:
            {
                scale = 1.0 / 195410 * mc_scale;
                break;
            }

        case ZPRIME4000:
            {
                scale = 1.0 / 180381 * mc_scale;
                break;
            }

        case RERECO_2011A_MAY10: // Fall through
        case RERECO_2011A_AUG05: // Fall through
        case PROMPT_2011A_V4: // Fall through
        case PROMPT_2011A_V6: // Fall through
        case PROMPT_2011B_V1:  // Do nothing
            return;

        case QCD_FROM_DATA: // do nothing
            hist->Scale(qcd_scale);
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
    FileStat_t buf;

    for(int i = 0; CHANNELS > i; ++i)
    {
        // SKIP QCD
        //
        if (QCD_EM_PT80_170 >= i)
            continue;

        string file_name = folder(i) + filename();
        if (gSystem->GetPathInfo(file_name.c_str(), buf))
        {
            cout << "file : " << file_name << " is not available" << endl;
            continue;
        }

        TFile *file = open(file_name);
        if (!file)
            return;

        input[i] = file;
    }
}

void save(const string &plot_name, const string &destination)
{
    TH1 *hist = get(input[TTJETS], plot_name.c_str(), TTJETS);
    scale(hist, TTJETS);
    hist->Write(("el_" + destination + "__ttbar").c_str());

    TH1 *mc = (TH1 *) hist->Clone();

    hist = get(input[ZJETS], plot_name.c_str(), ZJETS);
    scale(hist, ZJETS);
    hist->Write(("el_" + destination + "__zjets").c_str());
    mc->Add(hist);

    hist = get(input[WJETS], plot_name.c_str(), WJETS);
    scale(hist, WJETS);
    hist->Write(("el_" + destination + "__wjets").c_str());
    mc->Add(hist);
    
    hist = merge(input, plot_name.c_str(), STOP_S, STOP_S + STOP_CHANNELS);
    hist->Write(("el_" + destination + "__singletop").c_str());
    mc->Add(hist);

    /*
    hist = merge(input, plot_name.c_str(), 0, QCD_CHANNELS);
    hist->Write(("el_" + destination + "__eleqcd").c_str());
    */

    hist = merge(input,
            plot_name.c_str(),
            RERECO_2011A_MAY10,
            RERECO_2011A_MAY10 + SIGNAL_CHANNELS);
    hist->Write(("el_" + destination + "__DATA").c_str());

    TH1 *data = (TH1 *) hist->Clone();
    data->Add(mc, -1);
    cout << "Integral(Data - MC): " << data->Integral() << endl;

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

    hist = get(input[QCD_FROM_DATA], plot_name.c_str(), QCD_FROM_DATA);
    scale(hist, QCD_FROM_DATA);
    hist->Write(("el_" + destination + "__eleqcd").c_str());
}

void saveSystematics(const string &plot_name, const string &destination)
{
    string suffix = systematicSuffix();

    if (MATCHING_PLUS == systematic)
    {
        TH1 *hist = get(input[TTJETS_MATCHING_UP], plot_name.c_str(), TTJETS_SCALE_UP);
        scale(hist, TTJETS_MATCHING_UP);
        hist->Write(("el_" + destination + "__ttbar"+ suffix).c_str());

        hist = get(input[WJETS_MATCHING_UP], plot_name.c_str(), WJETS_SCALE_UP);
        scale(hist, WJETS_MATCHING_UP);
        hist->Write(("el_" + destination + "__wjets"+ suffix).c_str());
    }
    else if (MATCHING_MINUS == systematic)
    {
        TH1 *hist = get(input[TTJETS_MATCHING_DOWN], plot_name.c_str(), TTJETS_SCALE_DOWN);
        scale(hist, TTJETS_MATCHING_DOWN);
        hist->Write(("el_" + destination + "__ttbar"+ suffix).c_str());

        hist = get(input[WJETS_MATCHING_DOWN], plot_name.c_str(), WJETS_SCALE_DOWN);
        scale(hist, WJETS_MATCHING_DOWN);
        hist->Write(("el_" + destination + "__wjets"+ suffix).c_str());
    }
    else if (SCALING_PLUS == systematic)
    {
        TH1 *hist = get(input[TTJETS_SCALE_UP], plot_name.c_str(), TTJETS_SCALE_UP);
        scale(hist, TTJETS_SCALE_UP);
        hist->Write(("el_" + destination + "__ttbar"+ suffix).c_str());

        hist = get(input[WJETS_SCALE_UP], plot_name.c_str(), WJETS_SCALE_UP);
        scale(hist, WJETS_SCALE_UP);
        hist->Write(("el_" + destination + "__wjets"+ suffix).c_str());
    }
    else if (SCALING_MINUS == systematic)
    {
        TH1 *hist = get(input[TTJETS_SCALE_DOWN], plot_name.c_str(), TTJETS_SCALE_DOWN);
        scale(hist, TTJETS_SCALE_DOWN);
        hist->Write(("el_" + destination + "__ttbar"+ suffix).c_str());

        hist = get(input[WJETS_SCALE_DOWN], plot_name.c_str(), WJETS_SCALE_DOWN);
        scale(hist, WJETS_SCALE_DOWN);
        hist->Write(("el_" + destination + "__wjets"+ suffix).c_str());
    }
    else
    {
        TH1 *hist = get(input[TTJETS], plot_name.c_str(), TTJETS);
        scale(hist, TTJETS);
        hist->Write(("el_" + destination + "__ttbar"+ suffix).c_str());

        hist = get(input[ZJETS], plot_name.c_str(), ZJETS);
        scale(hist, ZJETS);
        hist->Write(("el_" + destination + "__zjets"+ suffix).c_str());

        hist = get(input[WJETS], plot_name.c_str(), WJETS);
        scale(hist, WJETS);
        hist->Write(("el_" + destination + "__wjets"+ suffix).c_str());
        
        hist = merge(input, plot_name.c_str(), STOP_S, STOP_S + STOP_CHANNELS);
        hist->Write(("el_" + destination + "__singletop"+ suffix).c_str());

        /*
        hist = merge(input, plot_name.c_str(), 0, QCD_CHANNELS);
        hist->Write(("el_" + destination + "__eleqcd"+ suffix).c_str());
        */

        hist = get(input[ZPRIME1000], plot_name.c_str(), ZPRIME1000);
        scale(hist, ZPRIME1000);
        hist->Write(("el_" + destination + "__zp1000"+ suffix).c_str());

        hist = get(input[ZPRIME1500], plot_name.c_str(), ZPRIME1500);
        scale(hist, ZPRIME1500);
        hist->Write(("el_" + destination + "__zp1500"+ suffix).c_str());

        hist = get(input[ZPRIME2000], plot_name.c_str(), ZPRIME2000);
        scale(hist, ZPRIME2000);
        hist->Write(("el_" + destination + "__zp2000"+ suffix).c_str());

        hist = get(input[ZPRIME3000], plot_name.c_str(), ZPRIME3000);
        scale(hist, ZPRIME3000);
        hist->Write(("el_" + destination + "__zp3000"+ suffix).c_str());

        hist = get(input[ZPRIME4000], plot_name.c_str(), ZPRIME4000);
        scale(hist, ZPRIME4000);
        hist->Write(("el_" + destination + "__zp4000"+ suffix).c_str());
    }
}

void theta_input(const string &mask = "",
        const float &scale_qcd)
{
    if (!mask.empty())
        file_mask = mask;

    qcd_scale = scale_qcd;

    cout << "mc_scale: " << mc_scale << endl;
    cout << "qcd_scale: " << qcd_scale << endl;

    TGaxis::SetMaxDigits(3);
    gStyle->SetOptStat(kFALSE);

    loadFiles();

    TFile *output = TFile::Open("theta_input.root", "update");
    if (!output
            || !output->IsOpen())
    {
        cerr << "failed to open output file" << endl;

        return;
    }

    if (NONE == systematic)
    {
        save("htlep_before_htlep", "htlep");
        save("mttbar_after_htlep", "mttbar");
    }
    else
    {
        saveSystematics("mttbar_after_htlep", "mttbar");
    }

    output->Close();
}
