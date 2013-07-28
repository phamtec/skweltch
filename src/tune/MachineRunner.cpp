
#include "MachineRunner.hpp"

#include "JsonObject.hpp"
#include "JsonPath.hpp"
#include "TaskMonitor.hpp"

#include <iostream>
#include <fstream>
#include <cmath>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <czmq.h>

using namespace std;
using namespace boost;

bool MachineRunner::runOne(const string &machine, int iterations, int group, int iter, const string &vars, const JsonObject &success) {

	ifstream jsonfile(machine.c_str());
	JsonObject r;
	if (!r.read(logger, &jsonfile)) {
		return false;
	}

	TaskMonitor mon(logger);
	int fail = 0;
	int low = -1;
	int high = -1;
	int sum = 0;

	// startup the machine.
	vector<int> pids;
	if (!mon.start(&r, &pids, true)) {
		throw new runtime_error("couldn't run the machine.");
	}
	
	for (int i=0; i<iterations; i++) {
	
 		LOG4CXX_INFO(logger, "runner start run.")
		filesystem::remove("results.json");
		
		// try to kill it all.
		vector<int> vpids;
		if (!mon.doVent(&r, &vpids)) {
			return false;
		}
		
		// wait till vent is done.
		mon.waitFinish(vpids);
		
		if (*interrupted) {
			break;
		}
		
		// try to open the results file.
		JsonObject results;
		for (int i=0; i<100; i++) {
			ifstream s("results.json");
			if (s.is_open()) {
				LOG4CXX_DEBUG(logger, "results ready.")
				results.read(logger, &s);
				break;
			}
			
			// allow the file to be written.
			LOG4CXX_DEBUG(logger, "results not ready, waiting...")
			zclock_sleep(50);
		}
		
		if (!results.empty()) {
		
			// compare the results against the success condition.
			bool bad = false;
			JsonObject &o = const_cast<JsonObject &>(success);
			for (JsonObject::iterator i=o.begin(); !bad && i != o.end(); i++) {
				string k = o.getKey(i);
				if (o.isValueDouble(i)) {
					double v = o.getValueDouble(i);
					double rv = results.getDouble(k);
					if (abs(v - rv) > 0.005) {
						LOG4CXX_ERROR(logger, "failure because " << k << "=" << rv << " not " << v)
						bad = true;
					}
				}
				else {
					string v = o.getValueString(i);
					string rv = results.getAsString(k);
					if (v != rv) {
						LOG4CXX_ERROR(logger, "failure because " << k << "=" << rv << " not " << v)
						bad = true;
					}
				}
			}
			if (bad) {
				fail++;
				if (failonerror) {
					break;
				}
			}
			else {
				int elapsed = results.getInt("elapsed", -1);
				if (low < 0 || elapsed < low) {
					low = elapsed;
				}
				if (high < 0 || elapsed > high) {
					high = elapsed;
				}
				sum += elapsed;
			}
		}
		else {
			LOG4CXX_ERROR(logger, "failure because no results.")
			fail++;
			if (failonerror) {
				break;
			}
		}
		LOG4CXX_INFO(logger, "runner end run.")
	}
	
	// try to kill it all.
	if (!mon.doReap(&r, &pids)) {
		return false;
	}

	// wait till everything is gone.
	mon.waitFinish(pids);

	if (filesystem::exists("tasks.pid")) {
		throw new runtime_error("reap didn't do it's thing.");
	}
	
	double avg = (double)sum / (double)iterations;
	double med = (double)(high - low) / 2.0;

	cout << group << "\t" << iter << "\t" << vars << "\t" << iterations << "\t" << low << "\t" << high << 
		"\t" << fail << "\t" << avg << "\t" << med << endl;

	return fail == 0;

}
