// BookkeeperAnalyzer 
//
// Example of how to general purpose histograms
//
// Created by Victor E. Bazterra, UIC, August 16th 2011
//

#ifndef BSM_BOOKEEPER_ANALYZER
#define BSM_BOOKEEPER_ANALYZER

#include <boost/shared_ptr.hpp>

#include "interface/Analyzer.h"
#include "interface/HistogramBookkeeper.h"
#include "interface/SynchSelector.h"

#include "bsm_input/interface/Event.pb.h"
#include "JetMETObjects/interface/JetCorrectorParameters.h"

namespace bsm
{

// All analyzer needs to inherence from Analyzer class
class BookkeeperAnalyzer : public Analyzer
{
public:

    // Pointer type to the synchronization selectior
    typedef boost::shared_ptr<SynchSelector> SynchSelectorPtr;
    // Pointer type to the histogram bookkeeper
    typedef boost::shared_ptr<HistogramBookkeeper> HistogramBookkeeperPtr;

    // Defaul constructor
    BookkeeperAnalyzer();
    // Copy constructor (all analyzer needs to define a copy constructor)
    BookkeeperAnalyzer(const BookkeeperAnalyzer &);

    // Defining a empty onFileOpen function (called when opening a file)
    virtual void onFileOpen(const std::string &filename, const Input *) {}

    // Main function that process events (use to fill histograms)
    virtual void process(const Event *);

    // Defining print output (called at the end by the AppController)
    virtual void print(std::ostream & os) const
    {
        _synch_selector->print(os);
        _bookkeeper->print(os);
    }

    // Return a pointer to the bookkeeper
    const HistogramBookkeeperPtr bookkeeper()
    {
        return _bookkeeper;
    }

    // Return the JES delegate with the options used by the synch selector
    bsm::JetEnergyCorrectionDelegate * getJetEnergyCorrectionDelegate() const
    {
        return _synch_selector.get();
    }

    // What follows needs to be defined by all the analyzers

    virtual uint32_t id() const
    {
        return core::ID<BookkeeperAnalyzer>::get();
    }

    virtual ObjectPtr clone() const
    {
        return ObjectPtr(new BookkeeperAnalyzer(*this));
    }

    using Object::merge;

private:

    // Pointer to synch selection (implements the selection after PAT)
    SynchSelectorPtr _synch_selector;
    // Pointer to the bookkeeper (allow the fast creating of histograms)
    HistogramBookkeeperPtr _bookkeeper;
};

}

#endif
