#ifndef BSM_ROOT_INPUT
#define BSM_ROOT_INPUT

#include <ostream>
#include <string>

class Input
{
    public:
        enum Type
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
            UNKNOWN
        };

        Input(const Type &type): _type(type) {}

        Type type() const
        {
            return _type;
        }

        operator std::string() const
        {
            switch(type())
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
                default: return "Unknown";
            }
        }

        std::string repr() const
        {
            switch(type())
            {
                case QCD_BC_PT20_30: return  "qcd_bc_pt20to30";
                case QCD_BC_PT30_80: return  "qcd_bc_pt30to80";
                case QCD_BC_PT80_170: return "qcd_bc_pt80to170";
                case QCD_EM_PT20_30: return  "qcd_em_pt20to30";
                case QCD_EM_PT30_80: return  "qcd_em_pt30to80";
                case QCD_EM_PT80_170: return "qcd_em_pt80to170";
                case TTJETS: return "ttjets";
                case TTJETS_MATCHING_UP: return "ttjets_matching_up";
                case TTJETS_MATCHING_DOWN: return "ttjets_matching_down";
                case TTJETS_SCALE_UP: return "ttjets_scaling_up";
                case TTJETS_SCALE_DOWN: return "ttjets_scaling_down";
                case ZJETS: return "zjets";
                case WJETS: return "wjets";
                case WJETS_MATCHING_UP: return "wjets_matching_up";
                case WJETS_MATCHING_DOWN: return "wjets_matching_down";
                case WJETS_SCALE_UP: return "wjets_scaling_up";
                case WJETS_SCALE_DOWN: return "wjets_scaling_down";
                case STOP_S: return "stop_s";
                case STOP_T: return "stop_t";
                case STOP_TW: return "stop_tw";
                case SATOP_S: return "satop_s";
                case SATOP_T: return "satop_t";
                case SATOP_TW: return "satop_tw";
                case RERECO_2011A_MAY10: return "re-reco_2011a_may10";
                case RERECO_2011A_AUG05: return "re-reco_2011a_aug05";
                case PROMPT_2011A_V4: return "prompt_2011a_v4";
                case PROMPT_2011A_V6: return "prompt_2011a_v6";
                case PROMPT_2011B_V1: return "prompt_2011b_v1";
                case ZPRIME1000: return "z_m1000_w10";
                case ZPRIME1500: return "z_m1500_w15";
                case ZPRIME2000: return "z_m2000_w20";
                case ZPRIME3000: return "z_m3000_w30";
                case ZPRIME4000: return "z_m4000_w40";
                default: return "unknown";
            }
        }

        Input &operator ++()
        {
            _type = Type(static_cast<int>(_type) + 1);

            return *this;
        }

    private:
        Type _type;
};

bool operator <(const Input &l, const Input &r);

bool operator >(const Input &l, const Input &r);

std::ostream &operator <<(std::ostream &out, const Input &input);

#endif
