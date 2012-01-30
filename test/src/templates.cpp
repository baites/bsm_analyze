#include <iostream>
#include <stdexcept>

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <TRint.h>
#include <TStyle.h>

#include "interface/Templates.h"

using namespace std;
using namespace boost;
namespace po = program_options;

int main(int argc, char *argv[])
try
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("input", po::value<string>()->default_value("output_signal_p150_hlt.root"), "input file")
        ("scale-file", po::value<string>()->default_value(string()), "file with sample scales")
        ("mc-error", po::value<bool>()->default_value(true), "plot mc errors because luminosity and trigger")
        ("qcd-type", po::value<string>()->default_value("none"), "qcd type")
        ("pull-plots", po::value<float>()->default_value(0.0), "pull plots providing maximum pull")
        ("ks-chi2", po::value<bool>()->default_value(false), "add ks and chi2 to the plots")
        ("log-scale", po::value<bool>()->default_value(false), "make plots in log scale")
        ("raw-cutflow", po::value<bool>()->default_value(false), "PU + NNLO + TriggerEfficiency")
        ;

    po::variables_map options;
    po::store(po::parse_command_line(argc, argv, desc), options);
    po::notify(options);    

    if (options.count("help")) 
    {
        cout << desc << "\n";
        return 1;
    }

    int empty_argc = 3;
    char *empty_argv[] = { argv[0], "-b", "-q"};
    shared_ptr<TRint> root(new TRint("root", &empty_argc, empty_argv));

    shared_ptr<Templates> templates(
        new Templates(
            options["input"].as<string>(),
            options["scale-file"].as<string>()
        )
    );

    string qcd_type(options["qcd-type"].as<string>());

    if (qcd_type == "none")
        templates->setQCDType(Templates::QCD_NONE);
    else if (qcd_type == "mc")
        templates->setQCDType(Templates::QCD_FROM_MC);
    else if (qcd_type == "data")
        templates->setQCDType(Templates::QCD_FROM_DATA);
    else
    {
        cout << "Unknown qcd-type option (allowed = none, mc, dara)\n";
        return 1;
    }    

    templates->setMCError(options["mc-error"].as<bool>());

    templates->setPullPlots(options["pull-plots"].as<float>());

    templates->setKSChi2(options["ks-chi2"].as<bool>());

    templates->setLogScale(options["log-scale"].as<bool>());

    templates->setRawCutflow(options["raw-cutflow"].as<bool>());

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
