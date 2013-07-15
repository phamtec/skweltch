
#include "JsonConfig.hpp"
#include "JsonObject.hpp"
#include "JsonPath.hpp"
#include "TaskMonitor.hpp"

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

void runOne(const string &machine, int iterations) {

	TaskMonitor mon;
	int fail = 0;
	int low = -1;
	int high = -1;
	int sum = 0;
	for (int i=0; i<iterations; i++) {
	
		filesystem::remove("log/monitor.log");
		filesystem::remove("results.json");
		if (mon.start(machine) != 0) {
			BOOST_LOG_TRIVIAL(error) << "couldn't run the machine.";
			return;
		}
	
		if (filesystem::exists("tasks.pid")) {
			BOOST_LOG_TRIVIAL(error) << "reap didn't do it's thing.";
			return;
		}
		
		JsonObject results;
		ifstream s("results.json");
		if (s.is_open()) {
			results.read(&s);
			int elapsed = results.getInt("elapsed", -1);
			if (low < 0 || elapsed < low) {
				low = elapsed;
			}
			if (high < 0 || elapsed > high) {
				high = elapsed;
			}
			sum += elapsed;
		}
		else {
			fail++;
		}
	}
	double avg = (double)sum / (double)iterations;
	double med = (double)(high - low) / 2.0;

	BOOST_LOG_TRIVIAL(info) << machine << "\t" << iterations << "\t" << low << "\t" << high << 
		"\t" << fail << "\t" << avg << "\t" << med;

}

string mutate(int run, string config) {
	
	stringstream newconfig;
	newconfig << "temp/run" << run << ".json";
    filesystem::copy_file(config, newconfig.str(), filesystem::copy_option::overwrite_if_exists);
	return newconfig.str();
	
}

int main (int argc, char *argv[])
{
	log::add_file_log(log::keywords::file_name = "log/tune.log", log::keywords::auto_flush = true);
	
	if (argc != 3) {
		BOOST_LOG_TRIVIAL(error) << "usage: " << argv[0] << " jsonConfig jsonTuneConfig";
		return 1;
	}
	
	ifstream jsonfile(argv[2]);
	JsonConfig c(&jsonfile);
	JsonObject config;
	if (!c.read(&config)) {
		return 1;
	}
	
    int iterations = config.getInt("iterations", -1);
    if (iterations < 0) {
		BOOST_LOG_TRIVIAL(error) << "no iterations in config";
		return 1;
    }
    int mutations = config.getInt("mutations", -1);
    if (mutations < 0) {
		BOOST_LOG_TRIVIAL(error) << "no mutations in config";
		return 1;
    }
    
    // tune it.
	BOOST_LOG_TRIVIAL(info) << "machine\titerations\tlow\thigh\tfail\tavg\tmed";
	for (int i=0; i<mutations; i++) {
    	runOne(mutate(i, argv[1]), iterations);
	}
	
	return 0;
	
}
