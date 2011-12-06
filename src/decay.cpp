// Generate decay table
//
// Created by Samvel Khalatyan, Dec 5, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <TCanvas.h>
#include <TFile.h>
#include <TGaxis.h>
#include <TH1.h>
#include <TH2.h>
#include <TRint.h>

#include "bsm_stat/interface/Utility.h"
#include "interface/AppController.h"
#include "interface/DecayAnalyzer.h"
#include "interface/MonitorCanvas.h"

using namespace std;
using namespace boost;
using namespace bsm;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        boost::shared_ptr<DecayAnalyzer> analyzer(new DecayAnalyzer());
        boost::shared_ptr<AppController> app(new AppController());

        app->setAnalyzer(analyzer);

        result = app->run(argc, argv);
        if (result)
        {
            typedef bsm::stat::TH1Ptr TH1Ptr;
            typedef bsm::stat::TH2Ptr TH2Ptr;

            int empty_argc = 1;
            char *empty_argv[] = { argv[0] };

            boost::shared_ptr<TRint>
                root(new TRint("app", &empty_argc, empty_argv));

            TH2Ptr decay_level_1 = convert(*analyzer->decay_level_1());
            decay_level_1->SetName("decay_level_1");
            decay_level_1->GetXaxis()->SetTitle("Particle");
            decay_level_1->GetYaxis()->SetTitle("Parent");

            TH2Ptr decay_level_2 = convert(*analyzer->decay_level_2());
            decay_level_2->SetName("decay_level_2");
            decay_level_2->GetXaxis()->SetTitle("Particle");
            decay_level_2->GetYaxis()->SetTitle("Parent");

            TH2Ptr decay_level_3 = convert(*analyzer->decay_level_3());
            decay_level_3->SetName("decay_level_3");
            decay_level_3->GetXaxis()->SetTitle("Particle");
            decay_level_3->GetYaxis()->SetTitle("Parent");

            TH2Ptr decay_level_4 = convert(*analyzer->decay_level_4());
            decay_level_4->SetName("decay_level_4");
            decay_level_4->GetXaxis()->SetTitle("Particle");
            decay_level_4->GetYaxis()->SetTitle("Parent");

            TH2Ptr decay_level_5 = convert(*analyzer->decay_level_5());
            decay_level_5->SetName("decay_level_5");
            decay_level_5->GetXaxis()->SetTitle("Particle");
            decay_level_5->GetYaxis()->SetTitle("Parent");

            if (app->output())
            {
                decay_level_1->Write();
                decay_level_2->Write();
                decay_level_3->Write();
                decay_level_4->Write();
                decay_level_5->Write();
            }
        }
    }
    catch(const std::exception &error)
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
