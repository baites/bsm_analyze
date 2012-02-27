// Forward declarations
//
// Analysis classes forward declarations
//
// Created by Samvel Khalatyan, May 10, 2011
// Copyright 2011, All rights reserved

#ifndef BSM_FWD
#define BSM_FWD

#include <boost/shared_ptr.hpp>

namespace bsm
{
    class Analyzer;
    class AppController;
    class Options;

    /*
    namespace algorithm
    {
        class ClosestJet;
        class NeutrinoReconstruct;
    }
    */

    class Counter;
    class CounterDelegate;
    class Cut;
    template<class Compare> class Comparator;
    template<class LowerCompare, class UpperCompare, class Logic>
        class RangeComparator;
    class LockCounterOnUpdate;

    typedef boost::shared_ptr<Counter> CounterPtr;
    typedef boost::shared_ptr<Cut> CutPtr;

    class ElectronSelector;
    class JetSelector;
    class P4Selector;
    class MultiplicityCutflow;
    class MuonSelector;
    class PrimaryVertexSelector;
    class WJetSelector;
    class LockSelectorEventCounterOnUpdate;

    class DeltaMonitor;
    class ElectronsMonitor;
    class GenParticleMonitor;
    class JetsMonitor;
    class P4Monitor;
    class MissingEnergyMonitor;
    class MuonsMonitor;
    class PrimaryVerticesMonitor;

    class JetEnergyCorrectionOptions;
    class JetEnergyCorrections;
    class DeltaRJetEnergyCorrections;
    class ChildJetEnergyCorrections;
    class JetEnergyCorrectionDelegate;

    class CorrectedJet;

    class SynchSelectorOptions;
    class SynchSelector;
    class SynchSelectorDelegate;

    class Cut2DSelectorOptions;
    class Cut2DSelector;
    class Cut2DSelectorDelegate;

    class TriggerDelegate;
    class TriggerOptions;

    class H1Proxy;
    class H2Proxy;

    class Summary;
    class Pileup;
    class PileupOptions;
    class PileupDelegate;

    class PtLess;
    class PtGreater;

    class CorrectedPtLess;
    class CorrectedPtGreater;

    class ResonanceReconstructor;
    class SimpleResonanceReconstructor;
    class BtagResonanceReconstructor;
    class SimpleDrResonanceReconstructor;
    class HemisphereResonanceReconstructor;
    class SimpleResonanceReconstructorWithMass;

    struct GenResonance;
}

#endif
