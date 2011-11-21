#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <TRint.h>
#include <TStyle.h>

#include "interface/MatchingSystematic.h"

using namespace std;
using namespace boost;

int main(int argc, char *argv[])
try
{
    int empty_argc = 1;
    char *empty_argv[] = { argv[0] };
    shared_ptr<TRint> root(new TRint("root", &empty_argc, empty_argv));

    shared_ptr<MatchingSystematic> systematic(new MatchingSystematic());
    systematic->load();

    gStyle->SetOptStat(kFALSE);

    systematic->draw();

    root->Run();

    return 0;
}
catch(const std::exception &error)
{
    cerr << error.what() << endl;

    return 1;
}
catch(...)
{
    cerr << "Unknown error" << endl;

    return 1;
}
