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

    // Check if the number of electrons is one and print the event is otherwise.
    if (event->pf_electrons_size() > 1) 
    {
        std::cout << "Run: " << event->extra().run() << " Event: " << event->extra().id();
        std::cout << " number electrons: " << event->pf_electrons_size();
        std::cout << " second electron pt: " << pt(event->pf_electrons(1).physics_object().p4()) << std::endl;
    }

    // Loop over all the electrons in the event (should be only one)
    for (int i = 0; i < event->pf_electrons_size(); ++i)
    {
        const bsm::Electron & electron = event->pf_electrons(i);
        
        // Loop over all the electron categories
        // An electron can comply with multiple categories
        for (int j = 0; j < electron.electronid_size(); ++j)
        {
            const bsm::Electron::ElectronID & electronid = electron.electronid(j);

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
