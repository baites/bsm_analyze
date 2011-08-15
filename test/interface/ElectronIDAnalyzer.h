// HistogramProducer: ElectronIDAnalyzer
//
// Produce histograms related to the electronid
//
// Created by Victor E. Bazterra, August 3th 2011
//

#ifndef BSM_ELECTRONID_ANALYZER
#define BSM_ELECTRONID_ANALYZER

#include <boost/shared_ptr.hpp>

#include "interface/Analyzer.h"
#include "interface/HistogramBookkeeper.h"
#include "bsm_input/interface/Event.pb.h"

namespace bsm
{

class ElectronIDAnalyzer : public Analyzer 
{
public:

    typedef boost::shared_ptr<HistogramBookkeeper> HistogramBookkeeperPtr;

    ElectronIDAnalyzer();
    ElectronIDAnalyzer(const ElectronIDAnalyzer &);

    virtual void onFileOpen(const std::string &filename, const Input *) {}
    virtual void process(const Event *);

    virtual void print(std::ostream & os) const
    {
        _bookkeeper->print(os);
    }

    void write(std::string const & filename)
    {
        _bookkeeper->write(filename);
    }

    const HistogramBookkeeperPtr bookkeeper()
    {
        return _bookkeeper;
    }

    virtual uint32_t id() const
    {
        return core::ID<ElectronIDAnalyzer>::get();
    }

    virtual ObjectPtr clone() const
    {
        return ObjectPtr(new ElectronIDAnalyzer(*this));
    }

    using Object::merge;

private:

    HistogramBookkeeperPtr _bookkeeper;

};

}

#endif
