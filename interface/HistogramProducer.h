// Analyzer: HistogramProducer
//
// Generate a series of histograms based on PB.
//
// Created by Victor E. Bazterra, August 3th 2011 UIC
//

#ifndef BSM_HISTOGRAM_PRODUCER
#define BSM_HISTOGRAM_PRODUCER

#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_stat/interface/H1.h"
#include "bsm_stat/interface/H2.h"
#include "bsm_stat/interface/bsm_stat_fwd.h"
#include "interface/Analyzer.h"
#include "interface/bsm_fwd.h"
#include "interface/StatProxy.h"

namespace bsm
{

typedef boost::shared_ptr<H1Proxy> H1ProxyPtr;
typedef boost::shared_ptr<H2Proxy> H2ProxyPtr;
typedef boost::unordered_map<std::string, H1ProxyPtr> H1ProxyPtrContainer;
typedef boost::unordered_map<std::string, H2ProxyPtr> H2ProxyPtrContainer;

class HistogramProducer : public Analyzer
{
public:

    HistogramProducer() {}
    HistogramProducer(HistogramProducer const & object)
    {
        for ( H1ProxyPtrContainer::const_iterator h1 = object._cache1d.begin(); h1 != object._cache1d.end(); ++h1 )
        {
            _cache1d[h1->first].reset(new H1Proxy(*h1->second));
            monitor(_cache1d[h1->first]);
        }
        for ( H2ProxyPtrContainer::const_iterator h2 = object._cache2d.begin(); h2 != object._cache2d.end(); ++h2 )
        {
            _cache2d[h2->first].reset(new H2Proxy(*h2->second));
            monitor(_cache2d[h2->first]);
        }
    }

    // Booking 1d histograms
    void book1d(std::string const & key, 
        uint32_t const & bins, float const & min, float const & max
    )
    {
        _cache1d[key].reset(new H1Proxy(bins, min, max));
        monitor(_cache1d[key]);
    }

    // Booking 1d histograms
    void book2d(std::string const & key, 
        uint32_t const & xbins, float const & xmin, float const & xmax,
        uint32_t const & ybins, float const & ymin, float const & ymax
    )
    {
        _cache2d[key].reset(new H2Proxy(xbins, xmin, xmax, ybins, ymin, ymax));
        monitor(_cache2d[key]);
    }

    // Filling 1d histograms
    void fill1d(std::string const & key, double value, double weight = 1)
    {
        _cache1d.at(key)->histogram()->fill(value, weight);
    }
   
    // Filling 1d histograms
    void fill2d(std::string const & key, double xvalue, double yvalue, double weight = 1)
    {
        _cache2d.at(key)->histogram()->fill(xvalue, yvalue, weight);
    }
    
    virtual void print(std::ostream &) const {}

private:

    H1ProxyPtrContainer _cache1d;
    H2ProxyPtrContainer _cache2d;
};
 
#define ObjectInterface(class)\
virtual uint32_t id() const\
{\
    return core::ID<class>::get();\
}\
virtual ObjectPtr clone() const\
{\
    return ObjectPtr(new class(*this));\
}\
using Object::merge

}

#endif
