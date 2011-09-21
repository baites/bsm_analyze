
// HistogramProducer: TriggerEfficiencyAnalyzer
//
// Produce histograms trigger turnon curves.
//
// Created by Victor E. Bazterra, September 19th 2011
//


#include <cmath>

#include "interface/Cut.h"
#include "interface/TriggerEfficiencyAnalyzer.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Input.pb.h"
#include "bsm_input/interface/Trigger.pb.h"

namespace bsm
{

using namespace std;

TriggerEfficiencyAnalyzer::TriggerEfficiencyAnalyzer()
{	
    // Initializing selector (desabling htlep)

    _synch_selector.reset(new SynchSelector());
    _synch_selector->htlep()->disable();
    monitor(_synch_selector);

    // Initializing bookkeeper (booking histograms)

    _bookkeeper.reset(new HistogramBookkeeper());
    _bookkeeper->book1d("AllEventsHT", 50, 0, 300);
    _bookkeeper->book1d("TriggerOption1HT", 50, 0, 300);    
    monitor(_bookkeeper);
}


TriggerEfficiencyAnalyzer::TriggerEfficiencyAnalyzer(const TriggerEfficiencyAnalyzer & object)
{
    _synch_selector.reset(new SynchSelector(*object._synch_selector));
    monitor(_synch_selector);
    _bookkeeper.reset(new HistogramBookkeeper(*object._bookkeeper));
    monitor(_bookkeeper);
}


void TriggerEfficiencyAnalyzer::onFileOpen(const std::string &filename, const Input *input)
{
    if (!input->has_info())
    {
        clog << "event info is not available" << endl;
        return;
    }

    if (!input->info().triggers().size())
    {
        clog << "Trigger menu is not available" << endl;
        return;
    }

    typedef ::google::protobuf::RepeatedPtrField<TriggerItem> TriggerItems;
    for(
        TriggerItems::const_iterator hlt = input->info().triggers().begin();
        input->info().triggers().end() != hlt;
        ++hlt
    )
    {
        if (_hlt_map.end() != _hlt_map.find(hlt->hash())) continue;
        _hlt_map[hlt->hash()] = hlt->name();
    }
}


void TriggerEfficiencyAnalyzer::merge(const ObjectPtr & pointer)
{
    if (id() != pointer->id())
        return;

    boost::shared_ptr<TriggerEfficiencyAnalyzer> object =
        boost::dynamic_pointer_cast<TriggerEfficiencyAnalyzer>(pointer);

    if (!object)
        return;

    for (HLTMap::const_iterator hlt = object->_hlt_map.begin(); hlt != object->_hlt_map.end(); ++hlt)
        _hlt_map[hlt->first] = hlt->second;
        
    Object::merge(pointer);
}


void TriggerEfficiencyAnalyzer::print(std::ostream & os) const
{
    _synch_selector->print(os);

    os << "List of triggers in the menu" << endl;
    for (HLTMap::const_iterator hlt = _hlt_map.begin(); hlt != _hlt_map.end(); ++hlt)
        os << "  " << hlt->second << endl;
    os << endl;

    _bookkeeper->print(os);
}


void TriggerEfficiencyAnalyzer::process(const Event *event)
{
	// Check if the event pass the selection
    if (!_synch_selector->apply(event)) return;

    typedef ::google::protobuf::RepeatedPtrField<Trigger> Triggers;

    // Check if the event contains htl information
    if (!event->hlts().size())
    {
        clog << "HLT is not available" << endl;
        return;
    }

    // Computting the HT of the event
    double ht = 0;
    for(
        SynchSelector::GoodJets::const_iterator jet =
        _synch_selector->niceJets().begin();
        _synch_selector->niceJets().end() != jet;
        ++jet
    )   
        ht += pt(*jet->corrected_p4);

    // Fill the histogram with all the events
    _bookkeeper->get1d("AllEventsHT")->fill(ht);

    // Loop over the trigger menu to see which trigger is fire
    for(Triggers::const_iterator hlt = event->hlts().begin(); event->hlts().end() != hlt; ++hlt)
    {
        if (_hlt_map[hlt->hash()] == "hlt_ele10_caloidt_caloisovl_trkidt_trkisovl_ht200" && hlt->pass())
            _bookkeeper->get1d("TriggerOption1HT")->fill(ht);    	
    }

    return;
}


}
