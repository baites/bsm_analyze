
#include <algorithm>
#include <string>

#include <boost/format.hpp>

#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>

#include <bsm_stat/interface/Bin.h>
#include <bsm_stat/interface/Utility.h>
#include <interface/HistogramBookkeeper.h>

namespace bsm
{

using boost::format;
using namespace std;
using namespace stat;

static TH1Ptr convert(string const & name, H1 const & h1)
{
    TH1Ptr h(convert(h1));
    h->SetName(name.c_str());
    return h;
}


static TH2Ptr convert(string const & name, const H2 &h2)
{
    TH2Ptr h(convert(h2));
    h->SetName(name.c_str());
    return h;
}


HistogramBookkeeper::HistogramBookkeeper(HistogramBookkeeper const & object)
{
    for ( H1ProxyPtrContainer::const_iterator h1 = object._cache1d.begin(); h1 != object._cache1d.end(); ++h1 )
        _cache1d[h1->first].reset(new H1Proxy(*h1->second));
    for ( H2ProxyPtrContainer::const_iterator h2 = object._cache2d.begin(); h2 != object._cache2d.end(); ++h2 )
        _cache2d[h2->first].reset(new H2Proxy(*h2->second));
}


void HistogramBookkeeper::merge(const ObjectPtr & pointer)
{
    if (id() != pointer->id())
        return;

    boost::shared_ptr<HistogramBookkeeper> object =
        boost::dynamic_pointer_cast<HistogramBookkeeper>(pointer);

    if (!object)
        return;

    for ( H1ProxyPtrContainer::const_iterator h1 = object->_cache1d.begin(); h1 != object->_cache1d.end(); ++h1 )
        _cache1d[h1->first]->merge(h1->second);
    for ( H2ProxyPtrContainer::const_iterator h2 = object->_cache2d.begin(); h2 != object->_cache2d.end(); ++h2 )
        _cache2d[h2->first]->merge(h2->second);
}


void HistogramBookkeeper::write() const
{
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


void HistogramBookkeeper::print(std::ostream & os) const
{
    vector<string> keys;

    for ( H1ProxyPtrContainer::const_iterator h1 = _cache1d.begin(); h1 != _cache1d.end(); ++h1 )
        keys.push_back(h1->first);

    sort(keys.begin(), keys.end());

    os << format("Number of 1d histograms: %d\n") % _cache1d.size();
    os << "========================================\n";

    for (vector<string>::const_iterator key = keys.begin(); key != keys.end(); ++key)
        os << format ("%-30s : %7d\n") % (*key) % _cache1d.at(*key)->histogram()->entries();

    os << "\n";

    os << format("Number of 2d histograms: %d\n") % _cache2d.size();
    os << "========================================\n";

    keys.clear();

    for ( H2ProxyPtrContainer::const_iterator h2 = _cache2d.begin(); h2 != _cache2d.end(); ++h2 )
        keys.push_back(h2->first);

    sort(keys.begin(), keys.end());

    for (vector<string>::const_iterator key = keys.begin(); key != keys.end(); ++key)
        os << format ("%-30s : %7d\n") % (*key) % _cache1d.at(*key)->histogram()->entries();
}


}

