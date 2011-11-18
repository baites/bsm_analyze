#ifndef BSM_ROOT_JES_SYSTEMATIC
#define BSM_ROOT_JES_SYSTEMATIC

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

class JESSystematic
{
    public:
        JESSystematic(): _plotname("mttbar_after_htlep")
        {
        }

        std::string plotname() const
        {
            return _plotname;
        }

        void load()
        {
            load(_jes_none, "");
            load(_jes_plus, "_jes_plus", 1);
            load(_jes_minus, "_jes_minus", -1);
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

        Plots _jes_none;
        Plots _jes_plus;
        Plots _jes_minus;

        std::string _plotname;

        boost::shared_ptr<TCanvas> _canvas;
};

#endif
