// Compare Offline and HLT PF Jets 
//
// Created by Samvel Khalatyan, Oct 12, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <TCanvas.h>
#include <TGaxis.h>
#include <TH1.h>
#include <TH2.h>
#include <TRint.h>
#include <TFile.h>

#include "bsm_input/interface/Event.pb.h"
#include "bsm_stat/interface/bsm_stat_fwd.h"
#include "bsm_stat/interface/Utility.h"
#include "interface/AppController.h"
#include "interface/JetEnergyCorrections.h"
#include "interface/MonitorCanvas.h"
#include "interface/JetAnalyzer.h"
#include "interface/SynchSelector.h"

using namespace std;

using boost::shared_ptr;

using bsm::AppController;
using bsm::JetEnergyCorrectionOptions;
using bsm::DeltaCanvas;
using bsm::P4Canvas;
using bsm::JetAnalyzer;
using bsm::SynchSelectorOptions;

typedef shared_ptr<JetAnalyzer> AnalyzerPtr;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        AnalyzerPtr analyzer(new JetAnalyzer());
        boost::shared_ptr<AppController> app(new AppController());
        boost::shared_ptr<JetEnergyCorrectionOptions> jec_options(new JetEnergyCorrectionOptions());
        boost::shared_ptr<SynchSelectorOptions> synch_selector_options(new SynchSelectorOptions());

        jec_options->setDelegate(analyzer->getJetEnergyCorrectionDelegate());
        synch_selector_options->setDelegate(analyzer->getSynchSelectorDelegate());

        app->addOptions(*jec_options);
        app->addOptions(*synch_selector_options);

        app->setAnalyzer(analyzer);

        result = app->run(argc, argv);
        if (result)
        {
            int empty_argc = 1;
            char *empty_argv[] = { argv[0] };

            boost::shared_ptr<TRint>
                root(new TRint("app", &empty_argc, empty_argv));

            TGaxis::SetMaxDigits(3);

            shared_ptr<P4Canvas> reco_leading_jet_canvas(
                    new P4Canvas("Reco Leading Jet"));

            shared_ptr<P4Canvas> hlt_leading_jet_canvas(
                    new P4Canvas("HLT Leading Jet"));

            shared_ptr<P4Canvas> selected_hlt_leading_jet_canvas(
                    new P4Canvas("Selected HLT Leading Jet"));

            if (app->output())
            {
                reco_leading_jet_canvas->write(app->output().get(),
                        *analyzer->recoLeadingJetMonitor());

                hlt_leading_jet_canvas->write(app->output().get(),
                        *analyzer->hltLeadingJetMonitor());

                selected_hlt_leading_jet_canvas->write(app->output().get(),
                        *analyzer->selectedHltLeadingJetMonitor());
            }
            
            if (app->isInteractive())
            {
                reco_leading_jet_canvas->draw(
                        *analyzer->recoLeadingJetMonitor());

                hlt_leading_jet_canvas->draw(
                        *analyzer->hltLeadingJetMonitor());

                selected_hlt_leading_jet_canvas->draw(
                        *analyzer->selectedHltLeadingJetMonitor());

                root->Run();
            }
        }
    }
    catch(const exception &error)
    {
        cerr << error.what() << endl;

        result = false;
    }
    catch(...)
    {
        cerr << "Unknown error" << endl;

        result = false;
    }

    // Clean Up any memory allocated by libprotobuf
    //
    google::protobuf::ShutdownProtobufLibrary();

    return result
        ? 0
        : 1;
}
