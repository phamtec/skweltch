
#include "JsonConfig.hpp"
#include "JsonObject.hpp"
#include "JsonPath.hpp"
#include "TaskMonitor.hpp"
#include "MachineTuner.hpp"

#include <iostream>
#include <fstream>
#include <czmq.h>
#include <zclock.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;

int main (int argc, char *argv[])
{
	log::add_file_log(log::keywords::file_name = "log/tune.log", log::keywords::auto_flush = true);
	
	if (argc != 3) {
		BOOST_LOG_TRIVIAL(error) << "usage: " << argv[0] << " jsonConfig jsonTuneConfig";
		return 1;
	}
	
	JsonObject config;
	{
		ifstream jsonfile(argv[1]);
		JsonConfig c(&jsonfile);
		if (!c.read(&config)) {
			return 1;
		}
	}
		
	JsonObject tuneconfig;
	{
		ifstream jsonfile(argv[2]);
		JsonConfig c(&jsonfile);
		if (!c.read(&tuneconfig)) {
			return 1;
		}
	}
		
    int iterations = tuneconfig.getInt("iterations", -1);
    if (iterations < 0) {
		BOOST_LOG_TRIVIAL(error) << "no iterations in tuneconfig";
		return 1;
    }
    int mutations = tuneconfig.getInt("mutations", -1);
    if (mutations < 0) {
		BOOST_LOG_TRIVIAL(error) << "no mutations in tuneconfig";
		return 1;
    }
    int groups = tuneconfig.getInt("groups", -1);
    if (groups < 0) {
		BOOST_LOG_TRIVIAL(error) << "no groups in tuneconfig";
		return 1;
    }
    
   	filesystem::remove_all("temp");
    filesystem::create_directory("temp");
    
 	MachineTuner tuner(&config, &tuneconfig);

   	// tune it.
   	try {
		BOOST_LOG_TRIVIAL(info) << "group\ti\tvars\tn\tlow\thigh\tfail\tavg\tmed";
		for (int i=1; i<groups+1; i++) {
		
			// default to fail for each group.
			tuner.resetFail();
			
			for (int j=0; j<mutations; j++) {
	
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
		
				if (!tuner.runOne(newconfig.str(), iterations, i, j, vars)) {
					if (tuner.failOnError()) {
						break; // next group.
					}
				}
			}
		}
	}
	catch (runtime_error &e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
	}
	
	return 0;
	
}
