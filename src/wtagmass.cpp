// WtagMass table
//
// Read inputs and apply triggers
//
// Created by Samvel Khalatyan, May 27, 2011
// Copyright 2011, All rights reserved

#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <TRint.h>
#include <TH1.h>

#include "bsm_input/interface/Event.pb.h"
#include "bsm_stat/interface/bsm_stat_fwd.h"
#include "bsm_stat/interface/Utility.h"
#include "interface/AppController.h"
#include "interface/WtagMassAnalyzer.h"

using namespace std;

using boost::shared_ptr;

using bsm::AppController;
using bsm::WtagMassAnalyzer;

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool result = false;
    try
    {
        shared_ptr<WtagMassAnalyzer> analyzer(new WtagMassAnalyzer());
        shared_ptr<AppController> app(new AppController());

        app->setAnalyzer(analyzer);

        result = app->run(argc, argv);
        if (result) 
        {
            typedef bsm::stat::TH1Ptr TH1Ptr;

            int empty_argc = 1;
            char *empty_argv[] = { argv[0] };

            boost::shared_ptr<TRint>
                root(new TRint("app", &empty_argc, empty_argv));

            TH1Ptr mttbar = convert(*analyzer->mttbar());
            mttbar->GetXaxis()->SetTitle("m_{t#bar{t}} [GeV/c^{2}]");
            mttbar->Draw();

            root->Run();
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
