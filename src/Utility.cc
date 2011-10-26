// Utility Classes/functions
//
// Useful tools that are heavily used in the analysis
//
// Created by Samvel Khalatyan, Apr 22, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <iomanip>

#include <TH1.h>
#include <TLorentzVector.h>

#include "bsm_input/interface/Algebra.h"
#include "bsm_input/interface/Physics.pb.h"
#include "interface/CorrectedJet.h"
#include "interface/Utility.h"

using namespace std;

using namespace bsm::utility;
using namespace bsm;

Summary:: Summary(const uint32_t &files_total):
    _events_processed(0),
    _files_total(files_total),
    _files_processed(0),
    _total_events_size(0),
    _percent_done(0)
{
}

void Summary::addFilesProcessed()
{
    ++_files_processed;

    uint32_t quotent = 100 * filesProcessed() / filesTotal() / 10;
    if (_percent_done < quotent)
    {
        _percent_done = quotent;

        cout << "Processed " << setw(3) << quotent << "0 %" << endl;
    }
}

ostream &bsm::operator <<(ostream &out, const Summary &summary)
{
    out << "Job Summary" << endl;
    out << "  Processed Events: " << summary.eventsProcessed() << endl;
    out << "  Processed  Files: " << summary.filesProcessed() << endl;
    out << "Average Event Size: " << summary.averageEventSize();

    return out;
}


SupressTHistAddDirectory::SupressTHistAddDirectory():
    _flag(TH1::AddDirectoryStatus())
{
    TH1::AddDirectory(false);
}

SupressTHistAddDirectory::~SupressTHistAddDirectory()
{
    TH1::AddDirectory(_flag);
}



void bsm::utility::set(TLorentzVector *root_p4, const LorentzVector *bsm_p4)
{
    root_p4->SetPxPyPzE(bsm_p4->px(), bsm_p4->py(), bsm_p4->pz(), bsm_p4->e());
}



// PtLess
//
bool PtLess::operator()(const P4Ptr &v1, const P4Ptr &v2)
{
    return pt_less(*v1, *v2);
}



// PtGreater
//
bool PtGreater::operator()(const P4Ptr &v1, const P4Ptr &v2)
{
    return pt_greater(*v1, *v2);
}



// Corrected PtLess
//
bool CorrectedPtLess::operator()(const CorrectedJet &v1, const CorrectedJet &v2)
{
    return _pt_less(v1.corrected_p4, v2.corrected_p4);
}



// Corrected PtGreater
//
bool CorrectedPtGreater::operator()(const CorrectedJet &v1, const CorrectedJet &v2)
{
    return _pt_greater(v1.corrected_p4, v2.corrected_p4);
}
