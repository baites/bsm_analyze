// HistogramProducer: ElectronIDAnalyzer
//
// Produce histograms related to the electronid
//
// Created by Victor E. Bazterra, August 3th 2011
//

#ifndef BSM_ELECTRONID_ANALYZER
#define BSM_ELECTRONID_ANALYZER

#include "interface/HistogramProducer.h"
#include "bsm_input/interface/Event.pb.h"

namespace bsm
{

class ElectronIDAnalyzer : public HistogramProducer
{
public:

    ElectronIDAnalyzer();
    ElectronIDAnalyzer(const ElectronIDAnalyzer & object) : HistogramProducer(object) {}

    virtual void onFileOpen(const std::string &filename, const Input *) {}
    virtual void process(const Event *);

    ObjectInterface(ElectronIDAnalyzer);    
};

}

#endif
