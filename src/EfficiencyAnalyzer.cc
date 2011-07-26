// Efficiency Plots
//
// Apply lepton removal to jets, Jet Energy Corrections, select events
// based on the corrected P4, leptons and then aaply 2D cut:
//      DeltaR > CUT
//      pT,rel > CUT
//
// between the muon/electron and closest jet
//
// Created by Samvel Khalatyan, Jul 19, 2011
// Copyright 2011, All rights reserved

#include <boost/pointer_cast.hpp>

#include "bsm_core/interface/ID.h"
#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Event.pb.h"
#include "JetMETObjects/interface/FactorizedJetCorrector.h"
#include "interface/Selector.h"
#include "interface/EfficiencyAnalyzer.h"

using namespace std;

using boost::dynamic_pointer_cast;

using bsm::EfficiencyAnalyzer;

// Synch with Jet Energy corrections
//
EfficiencyAnalyzer::EfficiencyAnalyzer(const LeptonMode &mode):
    _lepton_mode(mode)
{
    std::string lepton_mode = (ELECTRON == _lepton_mode)
        ? "Electron"
        : "Muon";

    _cutflow.reset(new MultiplicityCutflow(SELECTIONS));
    _cutflow->cut(PRESELECTION)->setName("pre-selection");
    _cutflow->cut(SCRAPING)->setName("Scraping Veto");
    _cutflow->cut(HBHENOISE)->setName("HBHE Noise");
    _cutflow->cut(PRIMARY_VERTEX)->setName("Good Primary Vertex");
    _cutflow->cut(JET)->setName("2 Good Jets");
    _cutflow->cut(LEPTON)->setName(string("Good ") + lepton_mode);
    _cutflow->cut(VETO_SECOND_LEPTON)->setName(string("Veto Good ") + lepton_mode);
    _cutflow->cut(HTLEP)->setName("Ht,lep");
    _cutflow->cut(ISOLATED_LEPTON)->setName(string("Iso ") + lepton_mode);
    _cutflow->cut(CUT_2D_LEPTON)->setName(string("2D Cutted ") + lepton_mode);
    monitor(_cutflow);

    // Selectrors
    //
    _primary_vertex_selector.reset(new PrimaryVertexSelector());
    _jet_selector.reset(new JetSelector());
    _electron_selector.reset(new ElectronSelector());
    _muon_selector.reset(new MuonSelector());

    _electron_selector->primary_vertex()->disable();
    _muon_selector->primary_vertex()->disable();
    _muon_selector->pt()->setValue(35);

    monitor(_primary_vertex_selector);
    monitor(_jet_selector);
    monitor(_electron_selector);
    monitor(_muon_selector);
}

EfficiencyAnalyzer::EfficiencyAnalyzer(const EfficiencyAnalyzer &object):
    _lepton_mode(object._lepton_mode)
{
    setJetEnergyCorrections(object._corrections);

    _cutflow = 
        dynamic_pointer_cast<MultiplicityCutflow>(object._cutflow->clone());

    monitor(_cutflow);

    // Selectors
    //
    _primary_vertex_selector = 
        dynamic_pointer_cast<PrimaryVertexSelector>(object._primary_vertex_selector->clone());

    _jet_selector = 
        dynamic_pointer_cast<JetSelector>(object._jet_selector->clone());

    _electron_selector = 
        dynamic_pointer_cast<ElectronSelector>(object._electron_selector->clone());

    _muon_selector = 
        dynamic_pointer_cast<MuonSelector>(object._muon_selector->clone());

    monitor(_primary_vertex_selector);
    monitor(_jet_selector);
    monitor(_electron_selector);
    monitor(_muon_selector);
}

void EfficiencyAnalyzer::setJetEnergyCorrections(const Corrections &corrections)
{
    copyCorrections(corrections);

    _jec.reset(new FactorizedJetCorrector(_corrections));
}

void EfficiencyAnalyzer::onFileOpen(const std::string &filename, const Input *)
{
}

void EfficiencyAnalyzer::process(const Event *event)
{
    if (!_jec)
        return;

    _cutflow->apply(PRESELECTION);

    if (event->has_filters()
            && !event->filters().scraping_veto())
        return;

    _cutflow->apply(SCRAPING);

    _cutflow->apply(HBHENOISE);

    if (!event->primary_vertices().size()
            || !_primary_vertex_selector->apply(*event->primary_vertices().begin()))
        return;

    _cutflow->apply(PRIMARY_VERTEX);

    typedef ::google::protobuf::RepeatedPtrField<Electron> Electrons;

    const PrimaryVertex &pv = *event->primary_vertices().begin();
    GoodElectrons good_electrons;

    LockSelectorEventCounterOnUpdate electron_lock(*_electron_selector);
    for(Electrons::const_iterator electron = event->pf_electrons().begin();
            event->pf_electrons().end() != electron;
            ++electron)
    {
        if (_electron_selector->apply(*electron, pv))
            good_electrons.push_back(&*electron);
    }

    typedef ::google::protobuf::RepeatedPtrField<Muon> Muons;

    GoodMuons good_muons;

    LockSelectorEventCounterOnUpdate muon_lock(*_muon_selector);
    for(Muons::const_iterator muon = event->pf_muons().begin();
            event->pf_muons().end() != muon;
            ++muon)
    {
        if (_muon_selector->apply(*muon, pv))
            good_muons.push_back(&*muon);
    }

    GoodJets good_jets = jets(event, good_electrons, good_muons);

    if (2 > good_jets.size())
        return;

    _cutflow->apply(JET);

    const LorentzVector *lepton_p4 = 0;

    if (ELECTRON == _lepton_mode)
    {
        if (1 != good_electrons.size())
            return;

        _cutflow->apply(LEPTON);

        if (good_muons.size())
            return;

        lepton_p4 = &((*good_electrons.begin())->physics_object().p4());
    }
    else if (MUON == _lepton_mode)
    {
        if (1 != good_muons.size())
            return;

        _cutflow->apply(LEPTON);

        if (good_electrons.size())
            return;

        lepton_p4 = &((*good_muons.begin())->physics_object().p4());
    }
    else
        return;

    _cutflow->apply(VETO_SECOND_LEPTON);

    if (150 >= (et(event->missing_energy().p4()) + pt(*lepton_p4)))
        return;

    _cutflow->apply(HTLEP);

    GoodJets::const_iterator closest_jet = good_jets.end();
    float deltar_min = 999999;

    for(GoodJets::const_iterator jet = good_jets.begin();
            good_jets.end() != jet;
            ++jet)
    {
        const float deltar = dr(*lepton_p4, jet->corrected_p4);
        if (deltar < deltar_min)
        {
            deltar_min = deltar;
            closest_jet = jet;
        }
    }

    const float ptrel_value = ptrel(*lepton_p4, closest_jet->corrected_p4);

    if (0.5 < deltar_min)
        _cutflow->apply(ISOLATED_LEPTON);

    if (0.5 < deltar_min
            || 25 < ptrel_value)
        _cutflow->apply(CUT_2D_LEPTON);
}

uint32_t EfficiencyAnalyzer::id() const
{
    return core::ID<EfficiencyAnalyzer>::get();
}

EfficiencyAnalyzer::ObjectPtr EfficiencyAnalyzer::clone() const
{
    return ObjectPtr(new EfficiencyAnalyzer(*this));
}

void EfficiencyAnalyzer::merge(const ObjectPtr &pointer)
{
    if (pointer->id() != id())
        return;

    boost::shared_ptr<EfficiencyAnalyzer> object =
        dynamic_pointer_cast<EfficiencyAnalyzer>(pointer);

    if (!object)
        return;

    Object::merge(pointer);
}

void EfficiencyAnalyzer::print(std::ostream &out) const
{
    out << "Cutflow [" << _lepton_mode << " mode]" << endl;
    out << *_cutflow << endl;
    out << endl;

    out << "Jet Selector" << endl;
    out << *_jet_selector << endl;
    out << endl;

    switch(_lepton_mode)
    {
        case ELECTRON: out << "Electron Selector" << endl;
                       out << *_electron_selector << endl;
                       out << endl;
                       out << "Muon Veto" << endl;
                       out << *_muon_selector << endl;
                       out << endl;
                       break;

        case MUON:     out << "Muon Selector" << endl;
                       out << *_muon_selector << endl;
                       out << endl;
                       out << "Electron Veto" << endl;
                       out << *_electron_selector << endl;
                       out << endl;
                       break;

        default: break;
    }

    //const uint32_t _good_leptons = *(_cutflow->cut(PRESELECTION)->objects());
    const uint32_t _good_leptons = *(_cutflow->cut(HTLEP)->objects());
    out << "Isolation Efficiency: " << (_good_leptons
            ? (1.0 * *(_cutflow->cut(ISOLATED_LEPTON)->objects())) / _good_leptons
            : 0) << endl;
    out << "   2D-cut Efficiency: " << (_good_leptons
            ? (1.0 * *(_cutflow->cut(CUT_2D_LEPTON)->objects())) / _good_leptons
            : 0);
}

// Private
//
void EfficiencyAnalyzer::copyCorrections(const Corrections &corrections)
{
    _corrections.clear();

    if (!corrections.size())
        return;

    _corrections.resize(corrections.size());
    copy(corrections.begin(), corrections.end(), _corrections.begin());
}

EfficiencyAnalyzer::GoodJets
    EfficiencyAnalyzer::jets(const Event *event,
            const GoodElectrons &electrons,
            const GoodMuons &muons)
{
    GoodJets good_jets;

    typedef ::google::protobuf::RepeatedPtrField<Jet> Jets;
    typedef ::google::protobuf::RepeatedPtrField<Jet::Child> Children;

    const float rho = event->extra().rho();

    LockSelectorEventCounterOnUpdate lock(*_jet_selector);
    for(Jets::const_iterator jet = event->jets().begin();
            event->jets().end() != jet;
            ++jet)
    {
        // Skip jet is required information is not available
        //
        if (!jet->has_uncorrected_p4())
            continue;

        // Remove Leptons
        //
        LorentzVector corrected_p4 = jet->uncorrected_p4();
        for(Children::const_iterator child = jet->children().begin();
                jet->children().end() != child;
                ++child)
        {
            const LorentzVector &child_p4 = child->physics_object().p4();

            // Electrons
            //
            for(GoodElectrons::const_iterator electron = electrons.begin();
                    electrons.end() != electron;
                    ++electron)
            {
                const LorentzVector &electron_p4 = (*electron)->physics_object().p4();
                if (electron_p4 == child_p4)
                    corrected_p4 -= electron_p4;
            }

            // Muons
            //
            for(GoodMuons::const_iterator muon = muons.begin();
                    muons.end() != muon;
                    ++muon)
            {
                const LorentzVector &muon_p4 = (*muon)->physics_object().p4();
                if (muon_p4 == child_p4)
                    corrected_p4 -= muon_p4;
            }
        }

        // fix p4
        //
        _jec->setJetEta(eta(corrected_p4));
        _jec->setJetPt(pt(corrected_p4));
        _jec->setJetE(corrected_p4.e());
        _jec->setNPV(event->primary_vertices().size());
        _jec->setJetA(jet->extra().area());
        _jec->setRho(rho);

        float correction = _jec->getCorrection();
        corrected_p4 *= correction;

        // Original jet in the event can not be modified and Jet Selector can
        // only be applied to jet: therefore copy jet, set corrected p4 and
        // apply selector
        //
        Jet corrected_jet = *jet;

        *corrected_jet.mutable_physics_object()->mutable_p4() = corrected_p4;

        if (!_jet_selector->apply(corrected_jet))
            continue;

        // Store original jet and corrected p4
        //
        CorrectedJet tmp;
        tmp.jet = &*jet;
        tmp.corrected_p4 = corrected_p4;

        good_jets.push_back(tmp);
    }

    return good_jets;
}


// Helpers
//
std::ostream &bsm::operator <<(std::ostream &out, const EfficiencyAnalyzer::LeptonMode &mode)
{
    switch(mode)
    {
        case EfficiencyAnalyzer::ELECTRON: out << "electron";
                                           break;

        case EfficiencyAnalyzer::MUON:     out << "muon";
                                           break;

        default:                           out << "unknown";
                                           break;
    }

    return out;
}
