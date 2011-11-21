// Synchronization exercise selector that is used to select "good"
// events with muons, electrons, jetc, etc.
//
// Created by Samvel Khalatyan, Aug 01, 2011
// Copyright 2011, All rights reserved

#include <functional>

#include <boost/algorithm/string.hpp>
#include <boost/pointer_cast.hpp>

#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Electron.pb.h"
#include "bsm_input/interface/Event.pb.h"
#include "bsm_input/interface/Isolation.pb.h"
#include "bsm_input/interface/Muon.pb.h"
#include "bsm_input/interface/PrimaryVertex.pb.h"
#include "bsm_input/interface/Physics.pb.h"
#include "interface/Cut.h"
#include "interface/SynchSelector.h"
#include "interface/Cut2DSelector.h"
#include "interface/Utility.h"

using namespace std;
using namespace boost;
using namespace bsm;

// Synch Selector Options
//
SynchSelectorOptions::SynchSelectorOptions()
{
    _delegate = 0;

    _description.reset(new po::options_description("Synchronization Selector Options"));
    _description->add_options()
        ("lepton-mode",
         po::value<string>()->notifier(
             boost::bind(&SynchSelectorOptions::setLeptonMode, this, _1)),
         "synchronization selector lepton mode: muon, electron")

        ("cut-mode",
         po::value<string>()->notifier(
             boost::bind(&SynchSelectorOptions::setCutMode, this, _1)),
         "synchroniation selector cut mode: 2dcut, isolation")

        ("leading-jet",
         po::value<float>()->notifier(
             boost::bind(&SynchSelectorOptions::setLeadingJetPt, this, _1)),
         "leading jet pT cut")
    ;
}

SynchSelectorOptions::~SynchSelectorOptions()
{
}

void SynchSelectorOptions::setDelegate(SynchSelectorDelegate *delegate)
{
    if (_delegate != delegate)
        _delegate = delegate;
}

SynchSelectorDelegate *SynchSelectorOptions::delegate() const
{
    return _delegate;
}

// Options interface
//
SynchSelectorOptions::DescriptionPtr SynchSelectorOptions::description() const
{
    return _description;
}

// Privates
//
void SynchSelectorOptions::setLeptonMode(std::string mode)
{
    if (!delegate())
        return;

    to_lower(mode);

    if ("electron" == mode)
        delegate()->setLeptonMode(SynchSelectorDelegate::ELECTRON);
    else if ("muon" == mode)
        delegate()->setLeptonMode(SynchSelectorDelegate::MUON);
    else
        cerr << "unsupported synchronization selector lepton mode" << endl;
}

void SynchSelectorOptions::setCutMode(std::string mode)
{
    if (!delegate())
        return;

    to_lower(mode);

    if ("2dcut" == mode)
        delegate()->setCutMode(SynchSelectorDelegate::CUT_2D);
    else if ("isolation" == mode)
        delegate()->setCutMode(SynchSelectorDelegate::ISOLATION);
    else
        cerr << "unsupported synchronization selector cut mode" << endl;
}

void SynchSelectorOptions::setLeadingJetPt(const float &value)
{
    if (!delegate())
        return;

    if (0 > value)
    {
        cerr << "only positive values of leading jet pT are accepted" << endl;

        return;
    }

    delegate()->setLeadingJetPt(value);
}



// Synchronization Exercise Selector
//
SynchSelector::SynchSelector():
    _lepton_mode(ELECTRON),
    _cut_mode(CUT_2D)
{
    // Cutflow table
    //
    _cutflow.reset(new MultiplicityCutflow(SELECTIONS - 1));
    monitor(_cutflow);

    // Selectors
    //
    _primary_vertex_selector.reset(new PrimaryVertexSelector());
    _primary_vertex_selector->cut(PrimaryVertexSelector::RHO)->setValue(2.0);
    monitor(_primary_vertex_selector);

    _electron_selector.reset(new ElectronSelector());
    _electron_selector->cut(ElectronSelector::PT)->setValue(70);
    _electron_selector->cut(ElectronSelector::PRIMARY_VERTEX)->disable();
    monitor(_electron_selector);

    _muon_selector.reset(new MuonSelector());
    _muon_selector->cut(MuonSelector::PT)->setValue(35);
    monitor(_muon_selector);

    // Nice jets have pT > 25
    //
    _nice_jet_selector.reset(new JetSelector());
    _nice_jet_selector->cut(JetSelector::PT)->setValue(25);
    monitor(_nice_jet_selector);

    // Good jets have pT > 50
    //
    _good_jet_selector.reset(new JetSelector());
    monitor(_good_jet_selector);

    // 2D-Cut
    //
    _cut2d_selector.reset(new Cut2DSelector());
    monitor(_cut2d_selector);

    // Jet Energy Corrections
    //
    _jec.reset(new DeltaRJetEnergyCorrections());
    monitor(_jec);

    // Cuts
    //
    _cut.reset(new Comparator<logical_and<bool> >(true));
    monitor(_cut);

    _leading_jet.reset(new Comparator<>(250));
    monitor(_leading_jet);

    _htlep.reset(new Comparator<>(150));
    monitor(_htlep);

    // The cut value does not matter as it is not used
    //
    _tricut.reset(new Comparator<>(0));
    monitor(_tricut);

    _met.reset(new Comparator<>(50));
    monitor(_met);
}

SynchSelector::SynchSelector(const SynchSelector &object):
    _lepton_mode(object._lepton_mode),
    _cut_mode(object._cut_mode)
{
    // Cutflow Table
    //
    _cutflow = 
        dynamic_pointer_cast<MultiplicityCutflow>(object._cutflow->clone());
    monitor(_cutflow);

    // Selectors
    //
    _primary_vertex_selector = 
        dynamic_pointer_cast<PrimaryVertexSelector>(object._primary_vertex_selector->clone());
    monitor(_primary_vertex_selector);

    _electron_selector = 
        dynamic_pointer_cast<ElectronSelector>(object._electron_selector->clone());
    monitor(_electron_selector);

    _muon_selector = 
        dynamic_pointer_cast<MuonSelector>(object._muon_selector->clone());
    monitor(_muon_selector);

    _nice_jet_selector = 
        dynamic_pointer_cast<JetSelector>(object._nice_jet_selector->clone());
    monitor(_nice_jet_selector);

    _good_jet_selector = 
        dynamic_pointer_cast<JetSelector>(object._good_jet_selector->clone());
    monitor(_good_jet_selector);

    // 2D-Cut
    //
    _cut2d_selector = 
        dynamic_pointer_cast<Cut2DSelector>(object._cut2d_selector->clone());
    monitor(_cut2d_selector);

    // Jet Energy Corrections
    //
    _jec = dynamic_pointer_cast<JetEnergyCorrections>(object._jec->clone());
    monitor(_jec);

    // cuts
    //
    _cut = dynamic_pointer_cast<Cut>(object.cut()->clone());
    monitor(_cut);

    _leading_jet = dynamic_pointer_cast<Cut>(object.leadingJet()->clone());
    monitor(_leading_jet);

    _htlep = dynamic_pointer_cast<Cut>(object.htlep()->clone());
    monitor(_htlep);

    _tricut = dynamic_pointer_cast<Cut>(object.tricut()->clone());
    monitor(_tricut);

    _met = dynamic_pointer_cast<Cut>(object.met()->clone());
    monitor(_met);
}

SynchSelector::~SynchSelector()
{
}

SynchSelector::CutPtr SynchSelector::cut() const
{
    return _cut;
}

SynchSelector::CutPtr SynchSelector::leadingJet() const
{
    return _leading_jet;
}

SynchSelector::CutPtr SynchSelector::htlep() const
{
    return _htlep;
}

SynchSelector::CutPtr SynchSelector::tricut() const
{
    return _tricut;
}

SynchSelector::CutPtr SynchSelector::met() const
{
    return _met;
}

bool SynchSelector::apply(const Event *event)
{
    _cutflow->apply(PRESELECTION);

    _good_primary_vertices.clear();
    _good_electrons.clear();
    _good_muons.clear();
    _nice_jets.clear();
    _good_jets.clear();
    _closest_jet = _nice_jets.end();

    return primaryVertices(event)
        && jets(event)
        && lepton()
        && secondaryLeptonVeto()
        && isolationAnd2DCut()
        && leadingJetCut()
        && htlepCut(event)
        && triangularCut(event)
        && missingEnergy(event);
}

SynchSelector::CutflowPtr SynchSelector::cutflow() const
{
    return _cutflow;
}

const SynchSelector::GoodPrimaryVertices
    &SynchSelector::goodPrimaryVertices() const
{
    return _good_primary_vertices;
}

const SynchSelector::GoodElectrons &SynchSelector::goodElectrons() const
{
    return _good_electrons;
}

const SynchSelector::GoodMuons &SynchSelector::goodMuons() const
{
    return _good_muons;
}

const SynchSelector::GoodJets &SynchSelector::niceJets() const
{
    return _nice_jets;
}

const SynchSelector::GoodJets &SynchSelector::goodJets() const
{
    return _good_jets;
}

SynchSelector::GoodJets::const_iterator SynchSelector::closestJet() const
{
    return _closest_jet;
}

SynchSelector::LeptonMode SynchSelector::leptonMode() const
{
    return _lepton_mode;
}

SynchSelector::CutMode SynchSelector::cutMode() const
{
    return _cut_mode;
}

bsm::Cut2DSelectorDelegate *SynchSelector::getCut2DSelectorDelegate() const
{
    return _cut2d_selector.get();
}

// Synch Selector Delegate interface
//
void SynchSelector::setLeptonMode(const LeptonMode &lepton_mode)
{
    _lepton_mode = lepton_mode;
}

void SynchSelector::setCutMode(const CutMode &cut_mode)
{
    _cut_mode = cut_mode;
}

void SynchSelector::setLeadingJetPt(const float &value)
{
    _leading_jet->setValue(value);
}

// Jet Energy Correction Delegate interface
//
void SynchSelector::setCorrection(const Level &level,
        const string &file_name)
{
    _jec->setCorrection(level, file_name);
}

void SynchSelector::setSystematic(const Systematic &systematic,
        const std::string &filename)
{
    _jec->setSystematic(systematic, filename);
}

void SynchSelector::setChildCorrection()
{
    // there is not guarantee that --child-corrrection argument is used before
    // any level of the jet energy corrections file is specified. Therefore,
    // files should be reloaded with ne object.
    //
    shared_ptr<JetEnergyCorrections> jec(new ChildJetEnergyCorrections());
    jec->setCorrectionFiles(_jec->correctionFiles());

    // Activate new Jet Energy Corrections
    //
    _jec = jec;
}

// Selector interface
//
void SynchSelector::enable()
{
}

void SynchSelector::disable()
{
}

// Object inteface
//
uint32_t SynchSelector::id() const
{
    return core::ID<SynchSelector>::get();
}

SynchSelector::ObjectPtr SynchSelector::clone() const
{
    return ObjectPtr(new SynchSelector(*this));
}

void SynchSelector::print(std::ostream &out) const
{
    _cutflow->cut(PRESELECTION)->setName("pre-selection");
    _cutflow->cut(SCRAPING)->setName("Scraping Veto");
    _cutflow->cut(HBHENOISE)->setName("HBHE Noise");
    _cutflow->cut(PRIMARY_VERTEX)->setName("Good Primary Vertex");
    _cutflow->cut(JET)->setName("2 Good Jets");

    ostringstream lepton;
    lepton << _lepton_mode;

    _cutflow->cut(LEPTON)->setName(string("Good ") + lepton.str());
    _cutflow->cut(VETO_SECOND_LEPTON)->setName("Veto 2nd lepton");

    lepton << " " << _cut_mode;
    _cutflow->cut(CUT_LEPTON)->setName(lepton.str());

    _cutflow->cut(LEADING_JET)->setName("Leading Jet");
    _cutflow->cut(HTLEP)->setName("hTlep");
    _cutflow->cut(TRICUT)->setName("tri-cut");
    _cutflow->cut(MET)->setName("MET");

    out << "Cutflow [" << _lepton_mode << ": " << _cut_mode << "]" << endl;
    out << *_cutflow << endl;
    out << endl;
}

// Private
//
bool SynchSelector::primaryVertices(const Event *event)
{
    selectGoodPrimaryVertices(event);

    return !goodPrimaryVertices().empty()
        && (_cutflow->apply(PRIMARY_VERTEX), true);
}

bool SynchSelector::jets(const Event *event)
{
    selectGoodElectrons(event);
    selectGoodMuons(event);

    typedef ::google::protobuf::RepeatedPtrField<Jet> Jets;

    LockSelectorEventCounterOnUpdate lock_nice_jets(*_nice_jet_selector);
    LockSelectorEventCounterOnUpdate lock_good_jets(*_good_jet_selector);
    for(Jets::const_iterator jet = event->jet().begin();
            event->jet().end() != jet;
            ++jet)
    {
        CorrectedJet correction = _jec->correctJet(&*jet,
                event,
                _good_electrons,
                _good_muons);

        // Skip jet if energy corrections failed
        //
        if (!correction.corrected_p4)
            continue;

        // Original jet in the event can not be modified and Jet Selector can
        // only be applied to jet: therefore copy jet, set corrected p4 and
        // apply selector
        //
        Jet corrected_jet;
        corrected_jet.CopyFrom(*jet);
        corrected_jet.mutable_physics_object()->mutable_p4()->CopyFrom(
                *correction.corrected_p4);

        if (!_nice_jet_selector->apply(corrected_jet))
            continue;

        // Store original jet and corrected p4
        //
        _nice_jets.push_back(correction);

        if (!_good_jet_selector->apply(corrected_jet))
            continue;

        _good_jets.push_back(correction);
    }

    // Sort jets by pT
    //
    sort(_nice_jets.begin(), _nice_jets.end(), CorrectedPtGreater());
    sort(_good_jets.begin(), _good_jets.end(), CorrectedPtGreater());

    return 1 < _good_jets.size()
        && (_cutflow->apply(JET), true);
}

bool SynchSelector::lepton()
{
    return (ELECTRON == _lepton_mode
        ? !_good_electrons.empty()
        : !_good_muons.empty())

        && (_cutflow->apply(LEPTON), true);
}

bool SynchSelector::secondaryLeptonVeto()
{
    return (ELECTRON == _lepton_mode
        ? (1 == _good_electrons.size()
            && _good_muons.empty())

        : (1 == _good_muons.size()
            && _good_electrons.empty()))

        && (_cutflow->apply(VETO_SECOND_LEPTON), true);
}

bool SynchSelector::isolationAnd2DCut()
{
    if (_cut->isDisabled())
        return true;

    const LorentzVector *lepton_p4 = 0;
    const PFIsolation *lepton_isolation = 0;

    if (ELECTRON == _lepton_mode)
    {
        const Electron *electron = *_good_electrons.begin();

        lepton_p4 = &(electron->physics_object().p4());

        if (electron->has_pf_isolation())
            lepton_isolation = &(electron->pf_isolation());
    }
    else
    {
        const Muon *muon = *_good_muons.begin();

        lepton_p4 = &(muon->physics_object().p4());

        if (muon->has_pf_isolation())
            lepton_isolation = &(muon->pf_isolation());
    }

    bool result = false;
    if (CUT_2D == _cut_mode)
        result = cut2D(lepton_p4);
    else
    {
        if (lepton_isolation)
            result = isolation(lepton_p4, lepton_isolation);
    }

    return _cut->apply(result)
        && (_cutflow->apply(CUT_LEPTON), true);
}

bool SynchSelector::leadingJetCut()
{
    if (leadingJet()->isDisabled())
        return true;

    float max_pt = 0;
    for(GoodJets::const_iterator jet = _good_jets.begin();
            _good_jets.end() != jet;
            ++jet)
    {
        const float jet_pt = pt(*jet->corrected_p4);
        if (jet_pt > max_pt)
            max_pt = jet_pt;
    }

    return leadingJet()->apply(max_pt)
        && (_cutflow->apply(LEADING_JET), true);
}

bool SynchSelector::htlepCut(const Event *event)
{
    if (htlep()->isDisabled())
        return true;

    const LorentzVector &lepton_p4 = (ELECTRON == _lepton_mode 
        ? (*_good_electrons.begin())->physics_object().p4()
        : (*_good_muons.begin())->physics_object().p4());

    return event->has_missing_energy()
        && htlep()->apply(pt(event->missing_energy().p4()) + pt(lepton_p4))
        && (_cutflow->apply(HTLEP), true);
}

bool SynchSelector::triangularCut(const Event *event)
{
    if (tricut()->isDisabled())
        return true;

    if (!event->has_missing_energy())
        return false;

    const LorentzVector &met = event->missing_energy().p4();

    const float met_pt = pt(met);

    const float dphi_el_met =
        fabs(dphi(goodElectrons()[0]->physics_object().p4(), met));

    const float dphi_ljet_met =
        fabs(dphi(*goodJets()[0].corrected_p4, met));

    const float slope = 1.5 / 75;

    return  dphi_el_met < (slope * met_pt + 1.5)
        && dphi_el_met > (-slope * met_pt + 1.5)
        && dphi_ljet_met < (slope * met_pt + 1.5)
        && dphi_ljet_met > (-slope * met_pt + 1.5)
        && (_cutflow->apply(TRICUT), true);
}

bool SynchSelector::missingEnergy(const Event *event)
{
    if (met()->isDisabled())
        return true;

    return event->has_missing_energy()
        && met()->apply(pt(event->missing_energy().p4()))
        && (_cutflow->apply(MET), true);
}

bool SynchSelector::cut2D(const LorentzVector *lepton_p4)
{
    if (_nice_jets.empty())
        return true;

    GoodJets::const_iterator closest_jet = _nice_jets.end();
    float deltar_min = 999999;

    for(GoodJets::const_iterator jet = _nice_jets.begin();
            _nice_jets.end() != jet;
            ++jet)
    {
        const float deltar = dr(*lepton_p4, *jet->corrected_p4);
        if (deltar < deltar_min)
        {
            deltar_min = deltar;
            closest_jet = jet;
        }
    }

    _closest_jet = closest_jet;

    if (_nice_jets.end() == closest_jet)
        return true;

    return _cut2d_selector->apply(*lepton_p4, *closest_jet->corrected_p4);
}

bool SynchSelector::isolation(const LorentzVector *p4, const PFIsolation *isolation)
{
    return 0.5 < (isolation->charged_hadron()
            + isolation->neutral_hadron()
            + isolation->photon())
        / pt(*p4);
}

void SynchSelector::selectGoodPrimaryVertices(const Event *event)
{
    typedef ::google::protobuf::RepeatedPtrField<PrimaryVertex> PrimaryVertices;

    LockSelectorEventCounterOnUpdate lock(*_primary_vertex_selector);
    for(PrimaryVertices::const_iterator pv = event->primary_vertex().begin();
            event->primary_vertex().end() != pv;
            ++pv)
    {
        if (_primary_vertex_selector->apply(*pv))
            _good_primary_vertices.push_back(&*pv);
    }
}

void SynchSelector::selectGoodElectrons(const Event *event)
{
    typedef ::google::protobuf::RepeatedPtrField<Electron> Electrons;

    const PrimaryVertex &pv = *event->primary_vertex().begin();

    LockSelectorEventCounterOnUpdate lock(*_electron_selector);
    for(Electrons::const_iterator electron = event->electron().begin();
            event->electron().end() != electron;
            ++electron)
    {
        if (!_electron_selector->apply(*electron, pv))
            continue;

        typedef ::google::protobuf::RepeatedPtrField<Electron::ElectronID>
            ElectronIDs;

        bool is_good_lepton = false;
        for(ElectronIDs::const_iterator id = electron->id().begin();
                electron->id().end() != id;
                ++id)
        {
            if (Electron::HyperTight1 != id->name())
                continue;

            if  (id->identification()
                    && id->conversion_rejection())
                is_good_lepton = true;

            break;
        }

        if (is_good_lepton)
            _good_electrons.push_back(&*electron);
    }
}

void SynchSelector::selectGoodMuons(const Event *event)
{
    typedef ::google::protobuf::RepeatedPtrField<Muon> Muons;

    const PrimaryVertex &pv = *event->primary_vertex().begin();

    LockSelectorEventCounterOnUpdate lock(*_muon_selector);
    for(Muons::const_iterator muon = event->muon().begin();
            event->muon().end() != muon;
            ++muon)
    {
        if (_muon_selector->apply(*muon, pv))
            _good_muons.push_back(&*muon);
    }
}



// Helpers
//
std::ostream &bsm::operator <<(std::ostream &out,
        const SynchSelectorDelegate::LeptonMode &lepton_mode)
{
    switch(lepton_mode)
    {
        case SynchSelector::ELECTRON:   out << "Electron";
                                        break;

        case SynchSelector::MUON:       out << "Muon";
                                        break;

        default:                        out << "unknown";
                                        break;
    }

    return out;
}

std::ostream &bsm::operator <<(std::ostream &out,
        const SynchSelectorDelegate::CutMode &cut_mode)
{
    switch(cut_mode)
    {
        case SynchSelector::CUT_2D:     out << "2D Cut";
                                        break;

        case SynchSelector::ISOLATION:  out << "Isolation";
                                        break;

        default:                        out << "unknown";
                                        break;
    }

    return out;
}
