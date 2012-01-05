// HistogramProducer: TriggerEfficiencyAnalyzer
//
// Produce histograms trigger turnon curves.
//
// Created by Victor E. Bazterra, September 19th 2011
//

#ifndef BSM_TRIGGER_EFFICIENCY_ANALYZER
#define BSM_TRIGGER_EFFICIENCY_ANALYZER

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include "interface/Analyzer.h"
#include "interface/Cut2DSelector.h"
#include "interface/HistogramBookkeeper.h"
#include "interface/Pileup.h"
#include "interface/SynchSelector.h"
#include "JetMETObjects/interface/JetCorrectorParameters.h"

namespace bsm
{

class TriggerEfficiencyDelegate
{
public:

    virtual ~TriggerEfficiencyDelegate() {}

    virtual void setLooseSelection(bool) {}
};


class TriggerEfficiencyOptions : public Options
{
public:

    TriggerEfficiencyOptions();

    virtual ~TriggerEfficiencyOptions() {}

    void setDelegate(TriggerEfficiencyDelegate *delegate)
    {
        if (_delegate != delegate)
            _delegate = delegate;
    }

    TriggerEfficiencyDelegate *delegate() const
    {
        return _delegate;
    }

    virtual DescriptionPtr description() const
    {
        return _description;
    }

private:

    void setLooseSelection(bool);

    TriggerEfficiencyDelegate *_delegate;

    DescriptionPtr _description;
};


class TriggerEfficiencyAnalyzer : 
    public Analyzer, public TriggerEfficiencyDelegate
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

    // Set the loose selection for trigger studies
    virtual void setLooseSelection(bool);

    // Return the bookkeeper
    const HistogramBookkeeperPtr bookkeeper()
    {
        return _bookkeeper;
    }

    // Return the JES delegate with the options used by the synch selector
    bsm::JetEnergyCorrectionDelegate * getJetEnergyCorrectionDelegate() const
    {
        return _synch_selector.get();
    }

    // Return the synch delegate with the options used by the synch selector
    bsm::SynchSelectorDelegate * getSynchSelectorDelegate() const
    {
        return _synch_selector.get();
    }

    // Return the pileup selegate with the options used by the pileup reweighting
    bsm::PileupDelegate * getPileupDelegate() const
    {
        return _pileup.get();
    }

    // Return the 2dCut delegate with the options used by the synch selector
    bsm::Cut2DSelectorDelegate * getCut2DSelectorDelegate() const
    {
        return _synch_selector->getCut2DSelectorDelegate();
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

    // Use of pileup reweighting
    boost::shared_ptr<Pileup> _pileup;
    bool _use_pileup;

    // HLT map with all the trugger options
    typedef std::map<std::size_t, std::string> HLTMap;
    HLTMap _hlt_map;
};

}

#endif
