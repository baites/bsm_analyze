#ifndef BSM_ROOT_SYSTEMATIC
#define BSM_ROOT_SYSTEMATIC

#include <vector>

class TCanvas;
class TH1;
class TLegend;
class TObject;
class Input;

class Systematic
{
    public:
        Systematic() {}
        virtual ~Systematic();

        TCanvas *draw(const Input &input,
                TH1 *central,
                TH1 *plus,
                TH1 *minus);

    private:
        TLegend *createLegend(const std::string &);

        typedef std::vector<TObject *> Heap;

        Heap _heap;
};

#endif
