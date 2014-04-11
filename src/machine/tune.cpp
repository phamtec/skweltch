
#include "JsonObject.hpp"
#include "JsonPath.hpp"
#include "TaskMonitor.hpp"
#include "MachineTuner.hpp"
#include "MachineRunner.hpp"
#include "Interrupt.hpp"
#include "Logging.hpp"
#include "Results.hpp"

#include <iostream>
#include <fstream>
#include <czmq.h>
#include <zclock.h>
#include <log4cxx/logger.h>
#include <boost/program_options.hpp>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;
namespace po = boost::program_options;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.tune"));

int main (int argc, char *argv[])
{
    setup_logging();
    
    po::positional_options_description pd;
    pd.add("jsonConfig", 1);
    pd.add("jsonTuneConfig", 1);
    
    po::options_description desc("options");
    desc.add_options()
    ("help", "produce help message")
    ("jsonConfig", po::value<string>(), "config to use.")
    ("jsonTuneConfig", po::value<string>(), "Tuning config to use.")
    ;
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
              options(desc).positional(pd).run(), vm);
    po::notify(vm);
    
    // minimal args
    if (vm.count("help") || !vm.count("jsonConfig") || !vm.count("jsonTuneConfig") ) {
        LOG4CXX_ERROR(logger, desc);
        return 0;
    }
    
    JsonObject config;
 	{
 		ifstream ss(vm["jsonConfig"].as<string>().c_str());
		if (!config.read(logger, &ss)) {
			return 1;
		}
 	}
	JsonObject tuneconfig;
 	{
 		ifstream ss(vm["jsonTuneConfig"].as<string>().c_str());
		if (!tuneconfig.read(logger, &ss)) {
			return 1;
		}
 	}
		
    int iterations = tuneconfig.getInt("iterations", -1);
    if (iterations < 0) {
		LOG4CXX_ERROR(logger, "no iterations in tuneconfig")
		return 1;
    }
    int mutations = tuneconfig.getInt("mutations", -1);
    if (mutations < 0) {
		LOG4CXX_ERROR(logger, "no mutations in tuneconfig")
		return 1;
    }
    int groups = tuneconfig.getInt("groups", -1);
    if (groups < 0) {
		LOG4CXX_ERROR(logger, "no groups in tuneconfig")
		return 1;
    }
    
    JsonObject success = tuneconfig.getChild("success");
    
   	filesystem::remove_all("temp");
    filesystem::create_directory("temp");
    
    s_catch_signals ();

    ofstream resultsFile("tune.txt");    
    StreamResults results(&resultsFile);
 	MachineTuner tuner(logger, &config, &tuneconfig, &s_interrupted);
 	// the last two numbers will need to be configurable.
 	MachineRunner runner(logger, &results, &s_interrupted, 1000, 10, 5000);

   	// tune it.
   	try {
		for (int i=1; !s_interrupted && i<groups+1; i++) {
		
			// default to fail for each group.
			tuner.resetFail();
			
			// draw the header.
  			int varcount = tuner.varCount(i);
  			if (varcount > 0) {
				resultsFile << "group\ti\t";
				for (int j=0; j<varcount; j++) {
					resultsFile << "v" << j+1 << "\t";
				}
				resultsFile << "n\tlow\thigh\tfail\tavg\tmed" << endl;
			}
						
			for (int j=0; !s_interrupted && j<mutations; j++) {
	
				string vars;
				if (!tuner.tune(i, j, &vars)) {
					break; // next group.
				}
				
				stringstream newconfig;
				newconfig << "temp/run" << j << ".json";
				{
					ofstream of(newconfig.str().c_str());
					config.write(true, &of);
					of.close();
				}
				runner.setFail(tuner.failOnError());
				if (!runner.runOne(newconfig.str(), iterations, i, j, vars, success)) {
					if (tuner.failOnError()) {
						break; // next group.
					}
				}
			}
		}
	}
	catch (runtime_error &e) {
		LOG4CXX_ERROR(logger, e.what())
	}
	
	return 0;
	
}
