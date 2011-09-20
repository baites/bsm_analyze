// HistogramProducer: TriggerEfficiencyAnalyzer
//
// Produce histograms trigger turnon curves.
//
// Created by Victor E. Bazterra, September 19th 2011
//

#ifndef BSM_TRIGGER_EFFICIENCY_ANALYZER
#define BSM_TRIGGER_EFFICIENCY_ANALYZER

#include <boost/shared_ptr.hpp>

#include "interface/Analyzer.h"
#include "interface/Cut2DSelector.h"
#include "interface/HistogramBookkeeper.h"
#include "interface/SynchSelector.h"
#include "JetMETObjects/interface/JetCorrectorParameters.h"

namespace bsm
{

class TriggerEfficiencyAnalyzer : public Analyzer
{
public:

    typedef boost::shared_ptr<SynchSelector> SynchSelectorPtr;
    typedef boost::shared_ptr<HistogramBookkeeper> HistogramBookkeeperPtr;

    // Default constructor
    TriggerEfficiencyAnalyzer();

    // Copy constructor
    TriggerEfficiencyAnalyzer(const TriggerEfficiencyAnalyzer &);

    // Function called when open a file (use to read metadata about the trigger options)
    virtual void onFileOpen(const std::string &filename, const Input *);
    
    // Function that process each event
    virtual void process(const Event *);

    // Print service     
    virtual void print(std::ostream & os) const;

    // Return the bookkeeper
    const HistogramBookkeeperPtr bookkeeper()
    {
        return _bookkeeper;
    }

    // Return the delegate with the options use for JEC
    JetEnergyCorrectionDelegate * getJetEnergyCorrectionDelegate() const
    {
        return _synch_selector->getJetEnergyCorrectionDelegate();
    }

    // Return the delegate with the options use for cut 2d selection
    Cut2DSelectorDelegate * getCut2DSelectorDelegate() const
    {
        return _synch_selector->getCut2DSelectorDelegate();
    }

    // Return the delegate with the options use by the synch selector
    SynchSelectorDelegate * getSynchSelectorDelegate() const
    {
        return _synch_selector.get();
    }

    // Minimal object interface (used by the AppController)
    virtual uint32_t id() const
    {
        return core::ID<TriggerEfficiencyAnalyzer>::get();
    }

    virtual ObjectPtr clone() const
    {
        return ObjectPtr(new TriggerEfficiencyAnalyzer(*this));
    }

    // Redefining the merge function to copy the hlt manu
    virtual void merge(const ObjectPtr &);

private:

    // Pointer to the synch selector
    SynchSelectorPtr _synch_selector;
    // Pointer to the book keepper
    HistogramBookkeeperPtr _bookkeeper;
    
    // HLT map with all the trugger options
    typedef std::map<std::size_t, std::string> HLTMap;
    HLTMap _hlt_map;
};

}

#endif
