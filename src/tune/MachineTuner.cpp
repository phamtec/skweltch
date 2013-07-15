
#include "MachineTuner.hpp"

#include "JsonObject.hpp"
#include "JsonPath.hpp"
#include "TaskMonitor.hpp"

#include <iostream>
#include <fstream>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;

bool MachineTuner::tune(int group, int mutation) {

	int mutations = tunerconfig->getInt("mutations", -1);
	if (mutations < 0) {
		throw new runtime_error("invalid mutations");
	}
	
	int vars = 0;
	for (JsonObject::iterator i=tunerconfig->begin(); i != tunerconfig->end(); i++) {
	
		string key = tunerconfig->getKey(i);
		JsonObject target = tunerconfig->getValue(i);
		if (target.isObject()) {
			if (key != "success") {
				if (!target.getBool("enabled")) {
					continue;
				}
				if (target.getInt("group", -1) == group) {
					string type = target.getString("type");
					if (type == "int") {
						int low = target.getInt("low", -1);
						if (low < 0) {
							throw new runtime_error("invalid low value for " + key);
						}
						int high = target.getInt("high", -1);
						if (high < 0) {
							throw new runtime_error("invalid high value for " + key);
						}
					
						string var = target.getString("var");
						string select = target.getString("select");
						JsonPath p;
						JsonObject o = p.getPath(*config, key);
						if (select == "hightolow") {
							double range = high - low;
							double unit = range / ((double)mutations);
							double sel = (double)mutations-(double)mutation;
							int val = (int)(unit*sel) + low;
							p.setPathInt(config, key, var, val);
							vars++;
						}
						else if (select == "random") {
						}
						else {
							BOOST_LOG_TRIVIAL(error) << " don't know how to select " << select;
							throw new runtime_error("don't know how to select");
						}
					}
					else {
						BOOST_LOG_TRIVIAL(error) << " don't know how to type " << type;
						throw new runtime_error("don't know the type" );
					}				
				}
			}
		}
	}

	return vars > 0;
		
}

bool MachineTuner::runOne(const string &machine, int iterations) {

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
			return false;
		}
	
		if (filesystem::exists("tasks.pid")) {
			BOOST_LOG_TRIVIAL(error) << "reap didn't do it's thing.";
			return false;
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

	BOOST_LOG_TRIVIAL(info) << machine << "\t\t\t" << iterations << "\t" << low << "\t" << high << 
		"\t" << fail << "\t" << avg << "\t" << med;

	return fail == 0;

}
