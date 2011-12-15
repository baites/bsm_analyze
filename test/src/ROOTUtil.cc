// utility ROOT functions for plotting
//
// Created by Samvel Khalatyan, Nov 18, 2011
// Copyright 2011, All rights reserved

#include <iostream>

#include <TH1.h>

#include "interface/ROOTUtil.h"

using namespace std;

float getScale(const Input &input)
{
    float scale = 1;
    switch(input.type())
    {
        case Input::QCD_BC_PT20_30:
            {
                scale = 2.361e8 * 5.9e-4 / 2081560;
                break;
            }

        case Input::QCD_BC_PT30_80:
            {
                scale = 5.944e7 * 2.42e-3 / 2030033;
                break;
            }

        case Input::QCD_BC_PT80_170:
            {
                scale = 8.982e5 * 1.05e-2 / 1082691;
                break;
            }

        case Input::QCD_EM_PT20_30:
            {
                scale = 2.361e8 * 1.06e-2 / 35729669;
                break;
            }

        case Input::QCD_EM_PT30_80:
            {
                scale = 5.944e7 * 6.1e-2 / 70392060;
                break;
            }

        case Input::QCD_EM_PT80_170:
            {
                scale = 8.982e5 * 1.59e-1 / 8150672;
                break;
            }

        case Input::TTJETS_SCALE_UP:
            {
                // Use NLO x-section: 157.5 instead of LO: 94.76
                //
                scale = 157.5 * 1.0 / 930483;
                break;
            }

        case Input::TTJETS_SCALE_DOWN:
            {
                // Use NLO x-section: 157.5 instead of LO: 94.76
                //
                scale = 157.5 * 1.0 / 967055;
                break;
            }

        case Input::TTJETS_MATCHING_UP:
            {
                // Use NLO x-section: 157.5 instead of LO: 94.76
                //
                scale = 157.5 * 1.0 / 1057479;
                break;
            }

        case Input::TTJETS_MATCHING_DOWN:
            {
                // Use NLO x-section: 157.5 instead of LO: 94.76
                //
                scale = 157.5 * 1.0 / 1065323;
                break;
            }

        case Input::TTJETS:
            {
                // Use NLO x-section: 157.5 instead of LO: 94.76
                //
                scale = 157.5 * 1.0 / 3701947;
                break;
            }

        case Input::ZJETS:
            {
                // Use NLO x-section: 3048 instead of LO: 2475
                //
                scale = 3048 * 1.0 / 36277961;
                break;
            }

        case Input::WJETS_SCALE_UP: // fall through
            {
                // Use NLO x-section: 31314 instead of LO: 27770
                //
                scale = 31314 * 1.0 / 9784907;
                break;
            }

        case Input::WJETS_SCALE_DOWN: // fall through
            {
                // Use NLO x-section: 31314 instead of LO: 27770
                //
                scale = 31314 * 1.0 / 10022324;
                break;
            }

        case Input::WJETS_MATCHING_UP: // fall through
            {
                // Use NLO x-section: 31314 instead of LO: 27770
                //
                scale = 31314 * 1.0 / 10461655;
                break;
            }

        case Input::WJETS_MATCHING_DOWN: // fall through
            {
                // Use NLO x-section: 31314 instead of LO: 27770
                //
                scale = 31314 * 1.0 / 9956679;
                break;
            }

        case Input::WJETS:
            {
                // Use NLO x-section: 31314 instead of LO: 27770
                //
                scale = 31314 * 1.0 / 77105816;
                break;
            }

        case Input::STOP_S:
            {
                scale = 3.19 * 1.0 / 259971;
                break;
            }

        case Input::STOP_T:
            {
                scale = 41.92 * 1.0 / 3900171;
                break;
            }

        case Input::STOP_TW:
            {
                scale = 7.87 * 1.0 / 814390;
                break;
            }

        case Input::SATOP_S:
            {
                scale = 1.44 * 1.0 / 137980;
                break;
            }

        case Input::SATOP_T:
            {
                scale = 22.65 * 1.0 / 1944826;
                break;
            }

        case Input::SATOP_TW:
            {
                scale = 7.87 * 1.0 / 809984;
                break;
            }

        case Input::ZPRIME1000:
            {
                scale = 1.0 / 207992;
                break;
            }

        case Input::ZPRIME1500:
            {
                scale = 1.0 / 168383;
                break;
            }

        case Input::ZPRIME2000:
            {
                scale = 1.0 / 179315;
                break;
            }

        case Input::ZPRIME3000:
            {
                scale = 1.0 / 195410;
                break;
            }

        case Input::ZPRIME4000:
            {
                scale = 1.0 / 180381;
                break;
            }

        case Input::QCD: // Fall through: QCD is extracted from Data
        case Input::RERECO_2011A_MAY10: // Fall through
        case Input::RERECO_2011A_AUG05: // Fall through
        case Input::PROMPT_2011A_V4: // Fall through
        case Input::PROMPT_2011A_V6: // Fall through
        case Input::PROMPT_2011B_V1: // Do nothing
        case Input::QCD_FROM_DATA:
            break;

        default:
            {
                cerr << "didn't scale: insupported input type" << endl;

                break;
            }
    }

    return scale;
}

void scale(TH1 *h, const Input &input)
{
    using namespace std;

    if (!h->GetEntries())
    {
        cerr << "didn't scale " << input << ": no entries" << endl;

        return;
    }

    float scale = getScale(input);
    if (1 == scale)
        return;

    h->Scale(scale * luminosity());
}

std::string folder(const Input &input)
{
    using namespace std;

    switch(input.type())
    {
        case Input::QCD_BC_PT20_30: return "qcd_bc_pt20to30";
        case Input::QCD_BC_PT30_80: return "qcd_bc_pt30to80";
        case Input::QCD_BC_PT80_170: return "qcd_bc_pt80to170";
        case Input::QCD_EM_PT20_30: return "qcd_em_pt20to30";
        case Input::QCD_EM_PT30_80: return "qcd_em_pt30to80";
        case Input::QCD_EM_PT80_170: return "qcd_em_pt80to170";
        case Input::QCD_FROM_DATA: return "qcd_from_data";
        case Input::TTJETS: return "ttjets";
        case Input::TTJETS_MATCHING_UP: return "ttjets_matchingup";
        case Input::TTJETS_MATCHING_DOWN: return "ttjets_matchingdown";
        case Input::TTJETS_SCALE_UP: return "ttjets_scaleup";
        case Input::TTJETS_SCALE_DOWN: return "ttjets_scaledown";
        case Input::ZJETS: return "zjets";
        case Input::WJETS: return "wjets";
        case Input::WJETS_MATCHING_UP: return "wjets_matchingup";
        case Input::WJETS_MATCHING_DOWN: return "wjets_matchingdown";
        case Input::WJETS_SCALE_UP: return "wjets_scaleup";
        case Input::WJETS_SCALE_DOWN: return "wjets_scaledown";
        case Input::STOP_S: return "stop_s";
        case Input::STOP_T: return "stop_t";
        case Input::STOP_TW: return "stop_tw";
        case Input::SATOP_S: return "satop_s";
        case Input::SATOP_T: return "satop_t";
        case Input::SATOP_TW: return "satop_tw";
        case Input::RERECO_2011A_MAY10: return "golden_single_el_2011a_may10_rereco";
        case Input::RERECO_2011A_AUG05: return "golden_single_el_2011a_aug5_rereco_v1";
        case Input::PROMPT_2011A_V4: return "golden_single_el_2011a_prompt_v4";
        case Input::PROMPT_2011A_V6: return "golden_single_el_2011a_prompt_v6";
        case Input::PROMPT_2011B_V1: return "golden_single_el_2011b_prompt_v1";
        case Input::ZPRIME1000: return "zprime_m1000_w10";
        case Input::ZPRIME1500: return "zprime_m1500_w15";
        case Input::ZPRIME2000: return "zprime_m2000_w20";
        case Input::ZPRIME3000: return "zprime_m3000_w30";
        case Input::ZPRIME4000: return "zprime_m4000_w40";
        case Input::QCD: return "qcd";
        default:
        {
            cerr << "unsupported input: don't know folder" << endl;

            return "";
        }
    }
};

void style(TH1 *hist, const Input &input, const int &systematic)
{
    using namespace std;

    int color = 1;
    switch(input.type())
    {
        case Input::QCD_BC_PT20_30:
            {
                color = kYellow + 1;
                break;
            }
        case Input::QCD_BC_PT30_80:
            {
                color = kYellow + 2;
                break;
            }
        case Input::QCD_BC_PT80_170:
            {
                color = kYellow + 3;
                break;
            }
        case Input::QCD_EM_PT20_30:
            {
                color = kOrange - 3;
                break;
            }
        case Input::QCD_EM_PT30_80:
            {
                color = kOrange - 1;
                break;
            }
        case Input::QCD_EM_PT80_170:
            {
                color = kOrange;
                break;
            }
        case Input::QCD_FROM_DATA:
            {
                color = kYellow + 1;
                break;
            }
        case Input::STOP_S:
            {
                color = kMagenta + 1;
                break;
            }
        case Input::STOP_T:
            {
                color = kMagenta + 2;
                break;
            }
        case Input::STOP_TW:
            {
                color = kMagenta + 3;
                break;
            }
        case Input::SATOP_S:
            {
                color = kMagenta - 4;
                break;
            }
        case Input::SATOP_T:
            {
                color = kMagenta - 3;
                break;
            }
        case Input::SATOP_TW:
            {
                color = kMagenta - 2;
                break;
            }
        case Input::TTJETS:
            {
                color = kRed + 1;
                break;
            }
        case Input::ZJETS:
            {
                color = kBlue + 1;
                break;
            }
        case Input::WJETS:
            {
                color = kGreen + 1;
                break;
            }
        case Input::ZPRIME1000:
            {
                color = kBlack;
                hist->SetLineStyle(2);
                break;
            }
        case Input::ZPRIME1500:
            {
                color = kViolet + 2;
                break;
            }
        case Input::ZPRIME2000:
            {
                color = kBlack;
                hist->SetLineStyle(9);
                break;
            }
        case Input::ZPRIME3000:
            {
                color = kBlack;
                break;
            }
        case Input::ZPRIME4000:
            {
                color = kOrange;
                hist->SetLineStyle(9);
                break;
            }
        case Input::RERECO_2011A_MAY10:
        case Input::RERECO_2011A_AUG05:
        case Input::PROMPT_2011A_V4:
        case Input::PROMPT_2011A_V6:
        case Input::PROMPT_2011B_V1:
            break;
        case Input::QCD:
            {
                color = kYellow + 3;
                break;
            }
        default:
            {
                cerr << "can not stle plot: unsupported type" << endl;

                return;
            }
    }

    switch(systematic)
    {
        case 1:
            color -= 8;
            break;

        case -1:
            color += 1;
            break;

        default:
            break;
    }

    hist->SetLineColor(color);
    hist->SetMarkerColor(color);
    hist->SetMarkerSize(0.5);
    hist->SetLineWidth(2);
}

float luminosity()
{
    return 4328.472;
    //return 4.061545;
}

