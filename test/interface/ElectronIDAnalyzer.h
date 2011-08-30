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
#include "interface/Cut2DSelector.h"
#include "interface/HistogramBookkeeper.h"
#include "interface/SynchSelector.h"

#include "bsm_input/interface/Event.pb.h"
#include "JetMETObjects/interface/JetCorrectorParameters.h"

namespace bsm
{

class ElectronIDAnalyzer : public Analyzer
{
public:

    typedef boost::shared_ptr<SynchSelector> SynchSelectorPtr;
    typedef boost::shared_ptr<HistogramBookkeeper> HistogramBookkeeperPtr;

    ElectronIDAnalyzer();
    ElectronIDAnalyzer(const ElectronIDAnalyzer &);

    virtual void onFileOpen(const std::string &filename, const Input *) {}
    virtual void process(const Event *);

    virtual void print(std::ostream & os) const
    {
        _synch_selector->print(os);
        _bookkeeper->print(os);
    }

    const HistogramBookkeeperPtr bookkeeper()
    {
        return _bookkeeper;
    }

    JetEnergyCorrectionDelegate * getJetEnergyCorrectionDelegate() const
    {
        return _synch_selector->getJetEnergyCorrectionDelegate();
    }

    Cut2DSelectorDelegate * getCut2DSelectorDelegate() const
    {
        return _synch_selector->getCut2DSelectorDelegate();
    }

    SynchSelectorDelegate * getSynchSelectorDelegate() const
    {
        return _synch_selector.get();
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

    SynchSelectorPtr _synch_selector;
    HistogramBookkeeperPtr _bookkeeper;
};

}

#endif
