
#include "JsonObject.hpp"
#include "JsonPath.hpp"
#include "TaskMonitor.hpp"
#include "MachineTuner.hpp"
#include "MachineRunner.hpp"
#include "Interrupt.hpp"

#include <iostream>
#include <fstream>
#include <czmq.h>
#include <zclock.h>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.tune"));

int main (int argc, char *argv[])
{
	log4cxx::PropertyConfigurator::configure("log4cxx.conf");

	if (argc != 3) {
		LOG4CXX_ERROR(logger, "usage: " << argv[0] << " jsonConfig jsonTuneConfig")
		return 1;
	}
	
	JsonObject config;
 	{
 		ifstream ss(argv[1]);
		if (!config.read(logger, &ss)) {
			return 1;
		}
 	}
	JsonObject tuneconfig;
 	{
 		ifstream ss(argv[2]);
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
    
   	filesystem::remove_all("temp");
    filesystem::create_directory("temp");
    
    s_catch_signals ();

 	MachineTuner tuner(logger, &config, &tuneconfig, &s_interrupted);
 	MachineRunner runner(logger, &s_interrupted);

   	// tune it.
   	try {
		for (int i=1; !s_interrupted && i<groups+1; i++) {
		
			// default to fail for each group.
			tuner.resetFail();
			
			// draw the header.
  			int varcount = tuner.varCount(i);
  			if (varcount > 0) {
				cout << "group\ti\t";
				for (int j=0; j<varcount; j++) {
					cout << "v" << j+1 << "\t";
				}
				cout << "n\tlow\thigh\tfail\tavg\tmed" << endl;
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
				if (!runner.runOne(newconfig.str(), iterations, i, j, vars)) {
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
