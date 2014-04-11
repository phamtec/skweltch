
#include "JsonObject.hpp"
#include "JsonPath.hpp"
#include "MachineRunner.hpp"
#include "Interrupt.hpp"
#include "Logging.hpp"
#include "Results.hpp"

#include <iostream>
#include <fstream>
#include <czmq.h>
#include <zclock.h>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;
namespace po = boost::program_options;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.soak"));

int main (int argc, char *argv[])
{
    setup_logging();
    
    po::positional_options_description pd;
    pd.add("jsonConfig", 1);
    pd.add("count", 1);
    pd.add("iterations", 1);
    
    po::options_description desc("options");
    desc.add_options()
    ("help", "produce help message")
    ("jsonConfig", po::value<string>(), "config to use.")
    ("count", po::value<int>(), "The count")
    ("iterations", po::value<int>(), "The number of iterations")
    ;
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
              options(desc).positional(pd).run(), vm);
    po::notify(vm);
    
    // minimal args
    if (vm.count("help") || !vm.count("jsonConfig") || !vm.count("count") || !vm.count("iterations")) {
        LOG4CXX_ERROR(logger, desc);
        return 0;
    }
    
	int count = vm["count"].as<int>();
	int iterations = vm["iterations"].as<int>();
		
    s_catch_signals ();
    ofstream resultsFile("soak.txt");
    StreamResults results(&resultsFile);
 	// the last two numbers will need to be configurable.
 	MachineRunner runner(logger, &results, &s_interrupted, 1000, 10, 5000);
 	
	cout << "group\ti\tvars\tn\tlow\thigh\tfail\tavg\tmed" << endl;
//	runner.setFail(true);
 	for (int i=0; i<count; i++) {
 		LOG4CXX_INFO(logger, "soak start run.")
		try {
			if (!runner.runOne(vm["jsonConfig"].as<string>(), iterations, 0, 0, "", JsonObject())) {
//				LOG4CXX_ERROR(logger, "failed, returning")
//				break;
			}
		}
		catch (runtime_error &e) {
			LOG4CXX_ERROR(logger, e.what())
			return 1;
		}
 		LOG4CXX_INFO(logger, "soak end run.")
		if (s_interrupted) {
			break;
		}
	}
	
	return 0;
	
}
