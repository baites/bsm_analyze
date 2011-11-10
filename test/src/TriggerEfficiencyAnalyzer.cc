
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
    //_synch_selector->htlep()->disable();
    monitor(_synch_selector);

    // Initializing bookkeeper (booking histograms)

    _bookkeeper.reset(new HistogramBookkeeper());
    _bookkeeper->book1d("AllEventsHT", 50, 400, 1000);
    _bookkeeper->book1d("AllEventsElectronPT", 50, 100, 400);
    _bookkeeper->book1d("TriggerHT200HT", 50, 400, 1000);
    _bookkeeper->book1d("TriggerEle8HT", 50, 400, 1000);
    _bookkeeper->book1d("TriggerCaloIsoHT", 50, 400, 1000);
    _bookkeeper->book1d("TriggerEle25TriCentralJet30HT", 50, 400, 1000);
    _bookkeeper->book1d("TriggerEleXHT", 50, 400, 1000);
    _bookkeeper->book1d("TriggerEleXElectronPT", 50, 100, 400);
    _bookkeeper->book1d("TriggerEle90HT", 50, 400, 1000);
    _bookkeeper->book1d("TriggerEle90ElectronPT", 50, 100, 400);
    _bookkeeper->book1d("TriggerFancyHT", 50, 400, 1000);
    _bookkeeper->book1d("TriggerFancyElectronPT", 50, 100, 400);

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

    if (!input->info().trigger().path().size())
    {
        clog << "Trigger menu is not available" << endl;
        return;
    }

    typedef ::google::protobuf::RepeatedPtrField<TriggerItem> TriggerItems;
    for(
        TriggerItems::const_iterator hlt = input->info().trigger().path().begin();
        input->info().trigger().path().end() != hlt;
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

    // Get the collection of good electron from the synch selection
    SynchSelector::GoodElectrons const & electrons = _synch_selector->goodElectrons();

    // Electron id selection
    bool passeid = false;
    double electronPt = 0.0;

    // Loop over the collection (it should be one electron)
    for (std::size_t i = 0; i < electrons.size(); ++i)
    {
        bsm::Electron const & electron = *electrons[i];

        // Loop over the possible electron ids
        for (int j = 0; j < electron.id_size(); ++j)
        {
            const bsm::Electron::ElectronID & electronid = electron.id(j);

            // Keep looping if the electron id is not HyperTight1
            if (electronid.name() != bsm::Electron::HyperTight1) continue;

            // Check if HyperTight1 minimal condition
            if (
                electronid.identification() &&
                electronid.conversion_rejection()
            )
                passeid = true;

            break;
        }
        electronPt = pt(electron.physics_object().p4());
        break;
    }

    // Reject the event if do not pass the electronid
    if (!passeid) return;

    typedef ::google::protobuf::RepeatedPtrField<Trigger> Triggers;

    // Check if the event contains htl information
    if (!event->hlt().trigger().size())
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
    // Adding also the pt of the electron
    ht += electronPt;

    // Cuting in ht
    if (electronPt < 100) return;

    // Fill the histogram with all the events
    _bookkeeper->get1d("AllEventsHT")->fill(ht);
    _bookkeeper->get1d("AllEventsElectronPT")->fill(electronPt);

    bool elexflag = false;
    bool ele90flag = false;

    // Loop over the trigger menu to see which trigger is fire
    for(Triggers::const_iterator hlt = event->hlt().trigger().begin();
            event->hlt().trigger().end() != hlt;
            ++hlt)
    {
        if (_hlt_map[hlt->hash()] == "hlt_ht200" && hlt->pass())
            _bookkeeper->get1d("TriggerHT200HT")->fill(ht);
        if (_hlt_map[hlt->hash()] == "hlt_ele8" && hlt->pass())
            _bookkeeper->get1d("TriggerEle8HT")->fill(ht);
        if (_hlt_map[hlt->hash()] == "hlt_ele8_caloidl_caloisovl" && hlt->pass())
            _bookkeeper->get1d("TriggerCaloIsoHT")->fill(ht);
        if (_hlt_map[hlt->hash()] == "hlt_ele25_caloidvt_trkidt_centraltrijet30" && hlt->pass())
            _bookkeeper->get1d("TriggerEle25TriCentralJet30HT")->fill(ht);
        if (_hlt_map[hlt->hash()] == "hlt_ele10_caloidt_caloisovl_trkidt_trkisovl_ht200" && hlt->pass())
        {
            _bookkeeper->get1d("TriggerFancyHT")->fill(ht);
            _bookkeeper->get1d("TriggerFancyElectronPT")->fill(electronPt);
        }
        if (_hlt_map[hlt->hash()] == "hlt_ele90_nospikefilter" && hlt->pass())
        {
            _bookkeeper->get1d("TriggerEle90HT")->fill(ht);
            _bookkeeper->get1d("TriggerEle90ElectronPT")->fill(electronPt);
            ele90flag = true;
        }        
        if (
            (_hlt_map[hlt->hash()] == "hlt_ele45_caloidvt_trkidt" && hlt->pass()) ||
            (_hlt_map[hlt->hash()] == "hlt_ele52_caloidvt_trkidt" && hlt->pass()) ||
            (_hlt_map[hlt->hash()] == "hlt_ele65_caloidvt_trkidt" && hlt->pass())
        )
            elexflag = true;
    }

    if (elexflag && ele90flag)
    {
        _bookkeeper->get1d("TriggerEleXHT")->fill(ht);
        _bookkeeper->get1d("TriggerEleXElectronPT")->fill(electronPt);
    }

    return;
}


}
