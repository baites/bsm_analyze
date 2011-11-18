#ifndef BSM_ROOT_PILEUP_SYSTEMATIC
#define BSM_ROOT_PILEUP_SYSTEMATIC

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>

#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TLegend.h>
#include <TSystem.h>

#include "interface/Input.h"
#include "interface/ROOTUtil.h"

class PileupSystematic
{
    public:
        PileupSystematic(): _plotname("mttbar_after_htlep")
        {
        }

        std::string plotname() const
        {
            return _plotname;
        }

        void load()
        {
            load(_pileup_none, "");
            load(_pileup_plus, "_pileup_plus", 1);
            load(_pileup_minus, "_pileup_minus", -1);
        }

        void draw();

    private:
        typedef std::map<Input::Type, TH1 *> Plots;

        void load(Plots &plots,
                const std::string &suffix,
                const int &systematic = 0);

        void plot(const Input::Type &);

        void style(TH1 *, const int &);

        TLegend *createLegend(const std::string &);

        Plots _pileup_none;
        Plots _pileup_plus;
        Plots _pileup_minus;

        std::string _plotname;

        boost::shared_ptr<TCanvas> _canvas;
};

#endif
