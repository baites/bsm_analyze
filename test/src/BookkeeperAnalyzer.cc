// BookkeeperAnalyzer 
//
// Example of how to general purpose histograms
//
// Created by Victor E. Bazterra, UIC, August 16th 2011
//

#include <cmath>
#include <iostream>

#include "interface/BookkeeperAnalyzer.h"
#include "bsm_input/interface/Algebra.h"

namespace bsm
{

BookkeeperAnalyzer::BookkeeperAnalyzer()
{
    // Initializing selector by reseting the pointer
    _synch_selector.reset(new SynchSelector());
    // Monitor the selector 
    monitor(_synch_selector);

    // Initializing bookkeeper
    _bookkeeper.reset(new HistogramBookkeeper());
    // Booking histograms (each histograms has to have a unique name)
    _bookkeeper->book1d("EIDTightPt", 50, 0, 100);
    _bookkeeper->book1d("EIDTightEta", 50, -2.5, 2.5);
    // Monitor the bookkeeper
    monitor(_bookkeeper);
    
    // Note: All the monitored object will be copy and merge (if needed) when using multithreading.
}


BookkeeperAnalyzer::BookkeeperAnalyzer(const BookkeeperAnalyzer & object)
{
	// Initialize the selector by copy the one in object
    _synch_selector.reset(new SynchSelector(*object._synch_selector));
    // Monitor the new selector
    monitor(_synch_selector);
	// Initialize the bookkeeper by copy the one in object
    _bookkeeper.reset(new HistogramBookkeeper(*object._bookkeeper));
    // Monitor the new bookkeeper
    monitor(_bookkeeper);
    
    // Note: the order of to declare monitor object has to be the same as in the default constructor
}


void BookkeeperAnalyzer::process(const Event *event)
{
    // Check for the event pass the selection
    if (!_synch_selector->apply(event)) return;

    // Getting a constant reference to the collection of good electrons
    SynchSelector::GoodElectrons const & electrons = _synch_selector->goodElectrons();

    // Loop over all the electrons in the event (should be only one)
    for (std::size_t i = 0; i < electrons.size(); ++i)
    {
        bsm::Electron const & electron = *electrons[i];

        // Loop over all the electron categories
        // An electron can comply with multiple categories
        for (int j = 0; j < electron.electronid_size(); ++j)
        {
            bsm::Electron::ElectronID const & electronid = electron.electronid(j);

            // Check if the electron has the identification bit for the category tight
            if (electronid.name() == bsm::Electron::Tight && electronid.identification())
            {
            	// Fill the corresponding histograms
                _bookkeeper->get1d("EIDTightPt")->fill(pt(electron.physics_object().p4()));
                _bookkeeper->get1d("EIDTightEta")->fill(eta(electron.physics_object().p4()));
            }
        }
    }
}


}
