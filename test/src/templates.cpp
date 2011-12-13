#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include <TRint.h>
#include <TStyle.h>

#include "interface/Templates.h"

using namespace std;
using namespace boost;

int main(int argc, char *argv[])
try
{
    int empty_argc = 3;
    char *empty_argv[] = { argv[0], "-b", "-q"};
    shared_ptr<TRint> root(new TRint("root", &empty_argc, empty_argv));

    shared_ptr<Templates> templates(new Templates(1 < argc
                    ? argv[1]
                    : "output_signal_p250_hlt.root",
                2 < argc
                    ? argv[2]
                    : ""));
    templates->load();

    templates->draw();

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
