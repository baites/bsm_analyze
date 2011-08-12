// Analyzer: HistogramProducer
//
// Generate a series of histograms based on PB.
//
// Created by Victor E. Bazterra, August 3th 2011 UIC
//

#ifndef BSM_HISTOGRAM_BOOKKEEPER
#define BSM_HISTOGRAM_BOOKKEEPER

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_core/interface/Object.h"
#include "bsm_stat/interface/H1.h"
#include "bsm_stat/interface/H2.h"
#include "interface/StatProxy.h"

namespace bsm
{


class HistogramBookkeeper : public core::Object
{
public:

    typedef std::vector<std::string> KeyContainer;
    typedef boost::shared_ptr<stat::H1> H1Ptr;
    typedef boost::shared_ptr<stat::H2> H2Ptr;
    typedef boost::shared_ptr<H1Proxy> H1ProxyPtr;
    typedef boost::shared_ptr<H2Proxy> H2ProxyPtr;
    typedef boost::unordered_map<std::string, H1ProxyPtr> H1ProxyPtrContainer;
    typedef boost::unordered_map<std::string, H2ProxyPtr> H2ProxyPtrContainer;

    HistogramBookkeeper() {};
    HistogramBookkeeper(HistogramBookkeeper const & object);

    // Booking 1d histograms
    void book1d(std::string const & key,
                uint32_t const & bins, float const & min, float const & max
               )
    {
        _cache1d[key].reset(new H1Proxy(bins, min, max));
    }

    // Booking 2d histograms
    void book2d(std::string const & key,
                uint32_t const & xbins, float const & xmin, float const & xmax,
                uint32_t const & ybins, float const & ymin, float const & ymax
               )
    {
        _cache2d[key].reset(new H2Proxy(xbins, xmin, xmax, ybins, ymin, ymax));
    }

    // Get a pointer 1d histograms
    const H1Ptr get1d(std::string const & key)
    {
        return _cache1d.at(key)->histogram();
    }

    // Get a pointer 1d histograms
    const H2Ptr get2d(std::string const & key)
    {
        return _cache2d.at(key)->histogram();
    }

    virtual void print(std::ostream &) const;

    void write(std::string const &);

    virtual uint32_t id() const
    {
        return core::ID<HistogramBookkeeper>::get();
    }

    virtual ObjectPtr clone() const
    {
        return ObjectPtr(new HistogramBookkeeper(*this));
    }

    void merge(const ObjectPtr &);

private:

    H1ProxyPtrContainer _cache1d;
    H2ProxyPtrContainer _cache2d;

};

}

#endif
