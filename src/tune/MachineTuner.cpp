
#include "MachineTuner.hpp"

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

int MachineTuner::varCount(int group) {

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
					vars++;
				}
			}
		}
	}
	return vars;
}

bool MachineTuner::tune(int group, int mutation, string *varstring) {

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
					failonerror = target.getBool("fail");
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
						JsonPath p(logger);
						JsonObject o = p.getPath(*config, key);
						if (select == "hightolow") {
							double range = high - low;
							double unit = range / ((double)mutations);
							double sel = (double)mutations-(double)mutation;
							int val = (int)(unit*sel) + low;
							p.setPathInt(config, key, var, val);
							vars++;
							if (varstring) {
								if (!varstring->empty()) {
									*varstring += "\t";
								}
								*varstring += lexical_cast<string>(val);
							}
						}
						else if (select == "random") {
							boost::random::uniform_int_distribution<> dist(low, high);
							int val = dist(gen);
							p.setPathInt(config, key, var, val);
							vars++;
							if (varstring) {
								if (!varstring->empty()) {
									*varstring += "\t";
								}
								*varstring += lexical_cast<string>(val);
							}
						}
						else {
							LOG4CXX_ERROR(logger, " don't know how to select " << select)
							throw new runtime_error("don't know how to select");
						}
					}
					else {
						LOG4CXX_ERROR(logger,  "don't know how to type " << type)
						throw new runtime_error("don't know the type" );
					}				
				}
			}
		}
	}

	return vars > 0;
		
}
