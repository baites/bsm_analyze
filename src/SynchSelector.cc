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
#include "interface/Btag.h"
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

    ("jet-pt",
     po::value<float>()->notifier(
         boost::bind(&SynchSelectorOptions::setJetPt, this, _1)),
     "cut all jet pT")

    ("max-btags",
     po::value<float>()->notifier(
         boost::bind(&SynchSelectorOptions::setMaxBtag, this, _1)),
     "maximum number of b-tagged jets in event")

    ("min-btags",
     po::value<float>()->notifier(
         boost::bind(&SynchSelectorOptions::setMinBtag, this, _1)),
     "minimum number of b-tagged jets in event")

    ("toptag-cut",
     po::value<float>()->notifier(
         boost::bind(&SynchSelectorOptions::setToptag, this, _1)),
     "set top-tagged selection where the options are 0, 1")

    ("electron-pt",
     po::value<float>()->notifier(
         boost::bind(&SynchSelectorOptions::setElectronPt, this, _1)),
     "electron pT cut")

    ("qcd-template",
     po::value<bool>()->implicit_value(false)->notifier(
         boost::bind(&SynchSelectorOptions::setQCDTemplate, this, _1)),
     "derived a qcd template")

    ("wjets-template",
     po::value<bool>()->implicit_value(false)->notifier(
         boost::bind(&SynchSelectorOptions::setWjetsTemplate, this, _1)),
     "derived a wjets template based on event with one godd jet")

    ("ltop-pt",
     po::value<float>()->notifier(
         boost::bind(&SynchSelectorOptions::setLtopPt, this, _1)),
     "set min value of ltop pt")
    
    ("chi2",
     po::value<float>()->notifier(
         boost::bind(&SynchSelectorOptions::setChi2Discriminator, this, _1)),
     "set max chi2 disriminator")
    ;
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

void SynchSelectorOptions::setJetPt(const float &value)
{
    if (!delegate())
        return;

    if (0 > value)
    {
        cerr << "only positive values of jet pT are accepted" << endl;

        return;
    }

    delegate()->setJetPt(value);
}

void SynchSelectorOptions::setMaxBtag(const float &value)
{
    if (!delegate())
        return;

    delegate()->setMaxBtag(value);
}

void SynchSelectorOptions::setMinBtag(const float &value)
{
    if (!delegate())
        return;

    delegate()->setMinBtag(value);
}

void SynchSelectorOptions::setToptag(const float &value)
{
    if (!delegate())
        return;

    delegate()->setToptag(value);
}

void SynchSelectorOptions::setElectronPt(const float & value)
{
    if (!delegate()) return;

    delegate()->setElectronPt(value);
}

void SynchSelectorOptions::setQCDTemplate(const bool &value)
{
    if (!delegate())
        return;

    delegate()->setQCDTemplate(value);
}

void SynchSelectorOptions::setWjetsTemplate(const bool &value)
{
    if (!delegate())
        return;

    delegate()->setWjetsTemplate(value);
}

void SynchSelectorOptions::setLtopPt(const float &value)
{
    if (!delegate())
        return;

    if (0 > value)
        throw runtime_error("negative ltop pt is not allowed");

    delegate()->setLtopPt(value);
}

void SynchSelectorOptions::setChi2Discriminator(const float &value)
{
    if (!delegate())
        return;

    if (0 > value)
        throw runtime_error("negative chi2 cut is not allowed");

    delegate()->setChi2Discriminator(value);
}



// Synchronization Exercise Selector
//
SynchSelector::SynchSelector():
    _lepton_mode(ELECTRON),
    _cut_mode(CUT_2D),
    _qcd_template(false),
    _wjets_template(false),
    _weighted_toptag(false)
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

    _leading_jet.reset(new Comparator<>(150));
    monitor(_leading_jet);

    // Do not cut on max number of b-tagged jets by default
    //
    _max_btag.reset(new Comparator<less_equal<float> >(3));
    _max_btag->disable();
    monitor(_max_btag);

    // Do not cut on min number of b-tagged jets by default
    //
    _min_btag.reset(new Comparator<greater_equal<float> >(1));
    _min_btag->disable();
    monitor(_min_btag);

    // Do not apply top tagging selection by default
    //
    _toptag.reset(new Comparator<>(0));
    _toptag->disable();
    monitor(_toptag);

    _htlep.reset(new Comparator<>(150));
    monitor(_htlep);

    // The cut value does not matter as it is not used
    //
    _tricut.reset(new Comparator<>(0));
    monitor(_tricut);

    _met.reset(new Comparator<>(50));
    monitor(_met);

    _reconstruction.reset(new Comparator<logical_and<bool> >(true));
    monitor(_reconstruction);

    _ltop.reset(new Comparator<>(100));
    monitor(_ltop);

    _random_generator.reset(new RandomGenerator<>(140.,250.));
    //_random_generator.reset(new RandomGenerator<>(143.,203.));
    monitor(_random_generator);

    _chi2.reset(new Comparator<less<float> >(15));
    _chi2->disable();
    monitor(_chi2);

    _btag.reset(new Btag());
    monitor(_btag);
}

SynchSelector::SynchSelector(const SynchSelector &object):
    _lepton_mode(object._lepton_mode),
    _cut_mode(object._cut_mode),
    _qcd_template(object._qcd_template),
    _wjets_template(object._wjets_template),
    _triggers(object._triggers.begin(), object._triggers.end()),
    _weighted_toptag(false)
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

    _max_btag = dynamic_pointer_cast<Cut>(object.maxBtag()->clone());
    monitor(_max_btag);

    _min_btag = dynamic_pointer_cast<Cut>(object.minBtag()->clone());
    monitor(_min_btag);

    _toptag = dynamic_pointer_cast<Cut>(object.toptag()->clone());
    monitor(_toptag);

    _htlep = dynamic_pointer_cast<Cut>(object.htlep()->clone());
    monitor(_htlep);

    _tricut = dynamic_pointer_cast<Cut>(object.tricut()->clone());
    monitor(_tricut);

    _met = dynamic_pointer_cast<Cut>(object.met()->clone());
    monitor(_met);

    _reconstruction =
        dynamic_pointer_cast<Cut>(object.reconstruction()->clone());
    monitor(_reconstruction);

    _ltop = dynamic_pointer_cast<Cut>(object.ltop()->clone());
    monitor(_ltop);

    _random_generator = object._random_generator;
    monitor(_random_generator);

    _chi2 = dynamic_pointer_cast<Cut>(object.chi2()->clone());
    monitor(_chi2);

    _btag = dynamic_pointer_cast<Btag>(object._btag->clone());
    monitor(_btag);
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

SynchSelector::CutPtr SynchSelector::maxBtag() const
{
    return _max_btag;
}

SynchSelector::CutPtr SynchSelector::minBtag() const
{
    return _min_btag;
}

SynchSelector::CutPtr SynchSelector::toptag() const
{
    return _toptag;
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

SynchSelector::CutPtr SynchSelector::reconstruction() const
{
    return _reconstruction;
}

SynchSelector::CutPtr SynchSelector::ltop() const
{
    return _ltop;
}

SynchSelector::CutPtr SynchSelector::chi2() const
{
    return _chi2;
}

uint32_t SynchSelector::countBtaggedJets()
{
    if (!_btagged_jets.is_valid())
    {
        uint32_t btags = 0;
        for(GoodJets::const_iterator jet = _good_jets.begin();
                _good_jets.end() != jet;
                ++jet)
        {
            if (_btag->is_tagged(*jet))
                ++btags;
        }

        _btagged_jets.set(btags);
    }

    return _btagged_jets.get();
}

bool SynchSelector::apply(const Event *event)
{
    invalidate_cache();

    _cutflow->apply(PRESELECTION);

    _good_primary_vertices.clear();
    _good_electrons.clear();
    _good_muons.clear();
    _nice_jets.clear();
    _good_jets.clear();
    _ca_jets.clear();
    _top_jets.clear();
    _good_met.reset();
    _closest_jet = _nice_jets.end();

    // QCD template
    if (qcdTemplate())
    {
        tricut()->invert();
        return triggers(event)
               && primaryVertices(event)
               && jets(event)
               && lepton()
               && secondElectronVeto()
               && secondMuonVeto()
               && isolationAnd2DCut()
               && leadingJetCut()
               && maxBtags()
               && minBtags()
               && toptagCut()
               && htlepCut(event)
               && missingEnergy(event)
               && triangularCut(event);
    }

    // Nominal
    return triggers(event)
           && primaryVertices(event)
           && jets(event)
           && lepton()
           && secondElectronVeto()
           && secondMuonVeto()
           && isolationAnd2DCut()
           && leadingJetCut()
           && maxBtags()
           && minBtags()
           && toptagCut()
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

const SynchSelector::GoodJets &SynchSelector::caJets() const
{
    return _ca_jets;
}

const SynchSelector::GoodJets &SynchSelector::topJets() const
{
    return _top_jets;
}

const SynchSelector::GoodMET &SynchSelector::goodMET() const
{
    return _good_met;
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

bool SynchSelector::qcdTemplate() const
{
    return _qcd_template;
}

bool SynchSelector::wjetsTemplate() const
{
    return _wjets_template;
}

bsm::Cut2DSelectorDelegate *SynchSelector::getCut2DSelectorDelegate() const
{
    return _cut2d_selector.get();
}

bsm::BtagDelegate *SynchSelector::getBtagDelegate() const
{
    return _btag.get();
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

void SynchSelector::setJetPt(const float &value)
{
    _good_jet_selector->cut(JetSelector::PT)->setValue(value);
}

void SynchSelector::setMaxBtag(const float &value)
{
    _max_btag->setValue(value);
    _max_btag->enable();
}

void SynchSelector::setMinBtag(const float &value)
{
    _min_btag->setValue(value);
    _min_btag->enable();
}

void SynchSelector::setToptag(const float &value)
{
    _toptag->setValue(value);
    _toptag->enable();
}

void SynchSelector::setElectronPt(const float &value)
{
    _electron_selector->cut(ElectronSelector::PT)->setValue(value);
}

void SynchSelector::setQCDTemplate(const bool &value)
{
    _qcd_template = value;
}

void SynchSelector::setWjetsTemplate(const bool &value)
{
    _wjets_template = value;
}

void SynchSelector::setLtopPt(const float &value)
{
    ltop()->setValue(value);
}

void SynchSelector::setChi2Discriminator(const float &value)
{
    chi2()->setValue(value);
    chi2()->enable();
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
    stopMonitor(_jec);
    _jec = jec;
    monitor(_jec);
}

// Trigger Delegate interface
//
void SynchSelector::setTrigger(const Trigger &trigger)
{
    _triggers.push_back(trigger.hash());
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
    _cutflow->cut(PRESELECTION)->setName("Pre-Selection");
    _cutflow->cut(TRIGGER)->setName("Trigger");
    _cutflow->cut(SCRAPING)->setName("Scraping Veto");
    _cutflow->cut(HBHENOISE)->setName("HBHE Noise");
    _cutflow->cut(PRIMARY_VERTEX)->setName("Good Primary Vertex");
    _cutflow->cut(JET)->setName("2 Good Jets");

    ostringstream lepton;
    lepton << _lepton_mode;

    _cutflow->cut(LEPTON)->setName(string("Good ") + lepton.str());
    _cutflow->cut(VETO_SECOND_ELECTRON)->setName("Veto 2nd electron");
    _cutflow->cut(VETO_SECOND_MUON)->setName("Veto 2nd muon");

    lepton << " " << _cut_mode;
    _cutflow->cut(CUT_LEPTON)->setName(lepton.str());

    _cutflow->cut(LEADING_JET)->setName("Leading Jet");

    ostringstream max_btag;
    max_btag << "btagged jets <= " << maxBtag()->value();
    _cutflow->cut(MAX_BTAG)->setName(max_btag.str());

    ostringstream min_btag;
    min_btag << "btagged jets >= " << minBtag()->value();
    _cutflow->cut(MIN_BTAG)->setName(min_btag.str());

    _cutflow->cut(TOPTAG)->setName("toptag");
    _cutflow->cut(HTLEP)->setName("hTlep");
    _cutflow->cut(TRICUT)->setName("tri-cut");
    _cutflow->cut(MET)->setName("MET");
    _cutflow->cut(RECONSTRUCTION)->setName("reconstruction");
    _cutflow->cut(LTOP)->setName("pt(ltop)");
    _cutflow->cut(CHI2)->setName("Chi2");

    out << "Cutflow [" << _lepton_mode << ": " << _cut_mode << "]" << endl;
    out << *_cutflow << endl;
    out << endl;
}

bool SynchSelector::reconstruction(const bool &value)
{
    if (reconstruction()->isDisabled())
        return true;

    return reconstruction()->apply(value)
           && (_cutflow->apply(RECONSTRUCTION), true);
}

bool SynchSelector::ltop(const float &value)
{
    if (ltop()->isDisabled())
        return true;

    return ltop()->apply(value)
           && (_cutflow->apply(LTOP), true);
}

bool SynchSelector::chi2(const float &value)
{
    if (chi2()->isDisabled())
        return true;

    return chi2()->apply(value)
        && (_cutflow->apply(CHI2), true);
}

// Private
//
bool SynchSelector::triggers(const Event *event)
{
    bool result = _triggers.empty();

    if (!result
            && event->hlt().trigger().size())
    {
        // OR triggers
        //
        typedef ::google::protobuf::RepeatedPtrField<Trigger> PBTriggers;
        for(Triggers::const_iterator trigger = _triggers.begin();
                _triggers.end() != trigger
                && !result;
                ++trigger)
        {
            for(PBTriggers::const_iterator hlt = event->hlt().trigger().begin();
                    event->hlt().trigger().end() != hlt;
                    ++hlt)
            {
                if (hlt->hash() == *trigger)
                {
                    if (hlt->pass())
                        result = true;

                    break;
                }
            }
        }
    }

    return result
           && (_cutflow->apply(TRIGGER), true);
}

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

    // Correct all jets
    //
    typedef ::google::protobuf::RepeatedPtrField<Jet> Jets;

    LockSelectorEventCounterOnUpdate lock_nice_jets(*_nice_jet_selector);
    LockSelectorEventCounterOnUpdate lock_good_jets(*_good_jet_selector);
    const LorentzVector *met = &(event->missing_energy().p4());
    for(Jets::const_iterator jet = event->jet().begin();
            event->jet().end() != jet;
            ++jet)
    {
        CorrectedJet correction = _jec->correctJet(&*jet,
                                  event,
                                  _good_electrons,
                                  _good_muons,
                                  met);

        // Skip jet if energy corrections failed
        //
        if (!correction.corrected_p4)
            continue;

        met = correction.corrected_met.get();
        _good_met = correction.corrected_met;

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

    // This part of the code is use if 2 jet collection is used
    for(Jets::const_iterator jet = event->ca_toptag_jet().begin();
            event->ca_toptag_jet().end() != jet;
            ++jet)
    {
        CorrectedJet correction = _jec->correctJet(&*jet,
                                  event,
                                  _good_electrons,
                                  _good_muons,
                                  met);

        // Skip jet if energy corrections failed
        //
        if (!correction.corrected_p4)
            continue;

        _ca_jets.push_back(correction);

        // Store top tagged jets
        //
        if (!jet->has_toptag())
            continue;

        const Jet_TopTag & toptag = jet->toptag();
        if (!(
                    toptag.n_subjets() > 2 &&
                    toptag.min_mass() > 50 &&
                    toptag.top_mass() > 140 &&
                    toptag.top_mass() < 250
                )) continue;

        _top_jets.push_back(correction);
    }

    // Sort jets by pT
    //
    sort(_nice_jets.begin(), _nice_jets.end(), CorrectedPtGreater());
    sort(_good_jets.begin(), _good_jets.end(), CorrectedPtGreater());
    sort(_ca_jets.begin(), _ca_jets.end(), CorrectedPtGreater());
    sort(_top_jets.begin(), _top_jets.end(), CorrectedPtGreater());

    if (_wjets_template)
        return 1 == _good_jets.size()
               && (_cutflow->apply(JET), true);

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

bool SynchSelector::secondElectronVeto()
{
    return (ELECTRON == _lepton_mode
            ? 1 == _good_electrons.size()
            : _good_electrons.empty())
           && (_cutflow->apply(VETO_SECOND_ELECTRON), true);
}

bool SynchSelector::secondMuonVeto()
{
    return (ELECTRON == _lepton_mode
            ? _good_muons.empty()
            : 1 == _good_muons.size())
           && (_cutflow->apply(VETO_SECOND_MUON), true);
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

bool SynchSelector::maxBtags()
{
    if (maxBtag()->isDisabled())
        return true;

    return maxBtag()->apply(countBtaggedJets())
        && (_cutflow->apply(MAX_BTAG), true);
}

bool SynchSelector::minBtags()
{
    if (minBtag()->isDisabled())
        return true;

    return minBtag()->apply(countBtaggedJets())
                && (_cutflow->apply(MIN_BTAG), true);
}

bool SynchSelector::toptagCut()
{
    if (toptag()->isDisabled())
        return true;

    bool result = false;

    if (toptag()->value() == 0)
        result = (_top_jets.size() == 0);
    else if (toptag()->value() == 1)
        result = (_top_jets.size() > 0);

    return result && (_cutflow->apply(TOPTAG), true);
}

bool SynchSelector::htlepCut(const Event *event)
{
    if (htlep()->isDisabled())
        return true;

    const LorentzVector &lepton_p4 = (ELECTRON == _lepton_mode
                                      ? (*_good_electrons.begin())->physics_object().p4()
                                      : (*_good_muons.begin())->physics_object().p4());

    return goodMET()
           && htlep()->apply(pt(*goodMET()) + pt(lepton_p4))
           && (_cutflow->apply(HTLEP), true);
}

bool SynchSelector::triangularCut(const Event *event)
{
    if (tricut()->isDisabled())
        return true;

    if (!goodMET())
        return false;

    const LorentzVector &met = *goodMET();

    const float met_pt = pt(met);

    const float dphi_el_met =
        fabs(dphi(goodElectrons()[0]->physics_object().p4(), met));

    const float dphi_ljet_met =
        fabs(dphi(*goodJets()[0].corrected_p4, met));

    const float slope = 1.5 / 75;

    bool pass = dphi_el_met < (slope * met_pt + 1.5)
                && dphi_el_met > (-slope * met_pt + 1.5)
                && dphi_ljet_met < (slope * met_pt + 1.5)
                && dphi_ljet_met > (-slope * met_pt + 1.5)
                && (_cutflow->apply(TRICUT), true);

    return tricut()->isInverted() ? !pass : pass;
}

bool SynchSelector::missingEnergy(const Event *event)
{
    if (met()->isDisabled())
        return true;

    return goodMET()
           && met()->apply(pt(*goodMET()))
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

void SynchSelector::invalidate_cache()
{
    _btagged_jets.invalidate();
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

            if (qcdTemplate())
            {
                if (!id->identification() || !id->conversion_rejection())
                    is_good_lepton = true;
            }
            else
            {
                if (id->identification() && id->conversion_rejection())
                    is_good_lepton = true;
            }

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

void SynchSelector::useToptagWeight()
{
    toptag()->disable();
    _weighted_toptag = true;
}

bool SynchSelector::isToptagWeight() const
{
    return _weighted_toptag;
}

static float toptagWeight(float x)
{
    float p0 = 5.40683e-02;
    float p1 = 4.48390e+02;
    float p2 = 2.76646e-02;
    float p3 = 1.85887e-06;

    return p0/(1+exp(-p2*(x-p1)-p3*pow(x-p1,3)));
}

// Overlapping area of two interceting circles
static float overlapArea(
    float d,
    float r = 0.5,
    float R = 0.8
)
{
    if (d > r+R) return 0.0;
    if (d < R-r) return r*r*M_PI;

    float d2 = d*d;
    float r2 = r*r;
    float R2 = R*R;

    float A = r2*acos((d2+r2-R2)/(2*d*r));
    float B = R2*acos((d2+R2-r2)/(2*d*R));
    float C = 0.5*sqrt((-d+r+R)*(d+r-R)*(d-r+R)*(d+r+R));

    return A + B - C;
}

float SynchSelector::toptagWeight()
{
    float total_weight = 0.0;
    uint32_t nperm = pow(2,_ca_jets.size());

    for(uint32_t perm = 1; perm < nperm; ++perm)
    {
        uint32_t index = 0;
        float weight = 1.0;    

        for(SynchSelector::GoodJets::const_iterator cajet = _ca_jets.begin();
                _ca_jets.end() != cajet;
                ++cajet, ++index)
        {
            bool tag = (perm >> index) & 1;

            if (!tag)
                weight *= 1.0 - ::toptagWeight(pt(*cajet->corrected_p4));
            else
            {
                weight *= ::toptagWeight(pt(*cajet->corrected_p4));

                // Loop over the good jets to find
                // those close to leading ca jet
                /*typedef vector<float> Areas;
                typedef vector<SynchSelector::GoodJets::iterator> ConeJets;

                float totalArea = 0.0;
                Areas areas;
                ConeJets coneJets;
                LorentzVector p4;

                for(SynchSelector::GoodJets::iterator jet = _good_jets.begin();
                    _good_jets.end() != jet;
                ++jet)
                {
                float x = dr(*jet->corrected_p4, *cajet->corrected_p4);

                if (x < 1.3)
                {
                    float area = overlapArea(x);
                    // cout << "Area: " << x << " : " << area << endl;
                    totalArea += area;
                    areas.push_back(area);
                    coneJets.push_back(jet);
                    LorentzVector p4jet = *(jet->corrected_p4);
                    p4jet *= area;
                    p4 += p4jet;
                }
                }

                p4 *= (1.0/totalArea);

                float mass = _random_generator->value();
                float e = sqrt(
                p4.px() * p4.px() +
                p4.py() * p4.py() +
                p4.pz() * p4.pz() +
                mass * mass
                );

                for(size_t i = 0; i < coneJets.size(); ++i)
                {
                coneJets[i]->corrected_p4->set_e(
                    e * areas[i] * coneJets[i]->corrected_p4->e() / (p4.e()*totalArea)
                );
                }*/
            }
        }
        total_weight += weight;
    }

    // cout << "leading jet pt: " << pt(*_good_jets[0].corrected_p4);
    // cout << " weight: " << total_weight << endl;

    return total_weight;
}

// Helpers
//
std::ostream &bsm::operator <<(std::ostream &out,
                               const SynchSelectorDelegate::LeptonMode &lepton_mode)
{
    switch(lepton_mode)
    {
    case SynchSelector::ELECTRON:
        out << "Electron";
        break;

    case SynchSelector::MUON:
        out << "Muon";
        break;

    default:
        out << "unknown";
        break;
    }

    return out;
}

std::ostream &bsm::operator <<(std::ostream &out,
                               const SynchSelectorDelegate::CutMode &cut_mode)
{
    switch(cut_mode)
    {
    case SynchSelector::CUT_2D:
        out << "2D Cut";
        break;

    case SynchSelector::ISOLATION:
        out << "Isolation";
        break;

    default:
        out << "unknown";
        break;
    }

    return out;
}


