
#include <algorithm>
#include <string>

#include <boost/format.hpp>

#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>

#include <bsm_stat/interface/Bin.h>
#include <bsm_stat/interface/Utility.h>
#include <interface/HistogramProducer.h>

namespace bsm
{

using boost::format;
using namespace std;
using namespace stat;

static TH1Ptr convert(string const & name, H1 const & h1)
{
    TH1Ptr h(new TH1D());

    h->SetName(name.c_str());
    h->Sumw2();

    if (h1)
    {
        h->SetBins(h1.axis()->bins(), h1.axis()->min(), h1.axis()->max());

        for(int bin = 0, bins = 1 + h1.axis()->bins(); bins >= bin; ++bin)
        {
            const Bin & b(h1[bin]);
            h->SetBinContent(bin, b.data());
            h->SetBinError(bin, b.error());
        }

        h->SetEntries(h1.entries());
    }

    return h;
}


static TH2Ptr convert(string const & name, const H2 &h2)
{
    TH2Ptr h(new TH2D());

    h->SetName(name.c_str());
    h->Sumw2();

    if (!h2)
        return h;

    h->SetBins(
        h2.xAxis()->bins(), h2.xAxis()->min(),h2.xAxis()->max(),
        h2.yAxis()->bins(), h2.yAxis()->min(),h2.yAxis()->max()
    );

    for(
        uint32_t x_bin = 0,
        x_bins = 1 + h2.xAxis()->bins(),
        y_bins = 1 + h2.yAxis()->bins();
        x_bins >= x_bin;
        ++x_bin
    )
        for(uint32_t y_bin = 0; y_bins >= y_bin; ++y_bin)
        {
            const Bin & b(h2[x_bin][y_bin]);
            h->SetBinContent(x_bin, y_bin, b.data());
            h->SetBinError(x_bin, y_bin, b.error());
        }

    h->SetEntries(h2.entries());
    return h;
}


HistogramProducer::HistogramProducer(HistogramProducer const & object)
{
    for ( KeyContainer::const_iterator key = object._keys1d.begin(); key != object._keys1d.end(); ++key )
    {
        _cache1d[*key].reset(new H1Proxy(*object._cache1d.at(*key)));
        monitor(_cache1d[*key]);
    }
    for ( KeyContainer::const_iterator key = object._keys2d.begin(); key != object._keys2d.end(); ++key )
    {
        _cache2d[*key].reset(new H2Proxy(*object._cache2d.at(*key)));
        monitor(_cache2d[*key]);
    }
}


void HistogramProducer::write(std::string const & filename)
{
    boost::shared_ptr<TFile> file = boost::shared_ptr<TFile>(new TFile(filename.c_str(),"recreate"));

    for ( H1ProxyPtrContainer::const_iterator h1 = _cache1d.begin(); h1 != _cache1d.end(); ++h1 )
    {
        TH1Ptr th1 = convert(h1->first, (*h1->second->histogram()));
        th1->Write();
    }
    for ( H2ProxyPtrContainer::const_iterator h2 = _cache2d.begin(); h2 != _cache2d.end(); ++h2 )
    {
        TH2Ptr th2 = convert(h2->first, (*h2->second->histogram()));
        th2->Write();
    }
}


void HistogramProducer::print(std::ostream & os) const
{

    KeyContainer keys(_keys1d);
    sort(keys.begin(), keys.end());

    for (vector<string>::const_iterator key = keys.begin(); key != keys.end(); ++key)
        os << format ("%-30s : %7d\n") % (*key) % _cache1d.at(*key)->histogram()->entries();

    os << "\n";

    os << format("Number of 2d histograms: %d\n") % _cache2d.size();
    os << "========================================\n";

    keys = _keys2d;
    sort(keys.begin(), keys.end());

    for (vector<string>::const_iterator key = keys.begin(); key != keys.end(); ++key)
        os << format ("%-30s : %7d\n") % (*key) % _cache1d.at(*key)->histogram()->entries();
}


}


