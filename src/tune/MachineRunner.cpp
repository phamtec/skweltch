
#include "MachineRunner.hpp"

#include "JsonObject.hpp"
#include "JsonPath.hpp"
#include "TaskMonitor.hpp"

#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random/uniform_int_distribution.hpp>

using namespace std;
using namespace boost;

bool MachineRunner::runOne(const string &machine, int iterations, int group, int iter, const string &vars) {

	TaskMonitor mon(logger);
	int fail = 0;
	int low = -1;
	int high = -1;
	int sum = 0;
	for (int i=0; i<iterations; i++) {
	
 		LOG4CXX_INFO(logger, "runner start run.")
		filesystem::remove("log/monitor.log");
		filesystem::remove("results.json");
		if (mon.start(machine) != 0) {
			throw new runtime_error("couldn't run the machine.");
		}
	
		if (*interrupted) {
			return false;
		}
		
		if (filesystem::exists("tasks.pid")) {
			throw new runtime_error("reap didn't do it's thing.");
		}
		
		JsonObject results;
		ifstream s("results.json");
		if (s.is_open()) {
			results.read(logger, &s);
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
			if (failonerror) {
				return false;
			}
		}
		LOG4CXX_INFO(logger, "runner end run.")
	}
	double avg = (double)sum / (double)iterations;
	double med = (double)(high - low) / 2.0;

	cout << group << "\t" << iter << "\t" << vars << "\t" << iterations << "\t" << low << "\t" << high << 
		"\t" << fail << "\t" << avg << "\t" << med << endl;

	return fail == 0;

}
