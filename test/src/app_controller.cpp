// Test Application Controller
//
// Use default arguments that are supported by the Application Controller
//
// Created by Samvel Khalatyan, Jul 31, 2011
// Copyright 2011, All rights reserved

#include <boost/shared_ptr.hpp>

#include "interface/AppController.h"

using bsm::AppController;

int main(int argc, char *argv[])
{
    boost::shared_ptr<AppController> app(new AppController());

    return app->run(argc, argv);
}
