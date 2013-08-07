
#include "PipeBuilder.hpp"

#include "JsonObject.hpp"
#include "JsonArray.hpp"
#include "JsonNamePredicate.hpp"
#include "JsonPath.hpp"

#include <log4cxx/logger.h>

using namespace std;
using namespace boost;

/**
	The config connections look like this.
	
	"name": {
		"mode":"bind",
		"address": "*",
		"port": 5557
	}
	
	or 
	
	"name": {
		"mode":"connect",
		"block": "SinkElapsed",
		"connection": "pullFrom"
	}
	
	A port looks like:
	
	"name": {
		"mode": "bind",
		"address": "*"
		"port": 9000
	}
	
	or

	"name": {
		"mode": "connect",
		"node": "localhost"
		"port": 9000
	}
	
*/

JsonObject PipeBuilder::collect(JsonObject *config, const JsonObject &block) {

	JsonObject pipes;
	
	JsonObject connections = JsonPath(logger).getPath(block, "connections");
	for (JsonObject::iterator i = connections.begin(); i != connections.end(); i++) {
	
		JsonObject val = connections.getValue(i);
		string name = connections.getKey(i);
		
		string mode = val.getString("mode");
		JsonObject pipe;
		if (mode == "connect") {
			// need to look this block up as some other block's connection.
			string block = val.getString("block");
			JsonObject b = config->findObj(JsonNamePredicate(block));
			if (!b.isObject()) {
				LOG4CXX_ERROR(logger, "syntax error. " << block << " missing.")
				return b;
			}
			string conn = val.getString("connection");
			JsonObject c = JsonPath(logger).getPath(b, "connections." + conn);
			if (!c.isObject()) {
				LOG4CXX_ERROR(logger, "syntax error. " << conn << " missing.")
				return c;
			}
			try {
				pipe.add("node", b.getString("node"));
				pipe.add("port", c.getInt("port", -1));
			}
			catch (runtime_error &e) {
				LOG4CXX_ERROR(logger, "syntax error. " << block << " missing node or port")
				throw e;
			}
		}
		else if (mode == "bind") {
			try {
				pipe.add("address", val.getString("address"));
				pipe.add("port", val.getInt("port", -1));
			}
			catch (runtime_error &e) {
				LOG4CXX_ERROR(logger, "syntax error. " << name << " missing address or port")
				throw e;
			}
		}
		else {
			LOG4CXX_ERROR(logger, "mode must be bind or connect for " << name)
			return JsonObject();
		}
		pipe.add("mode", mode);
		pipes.add(name, pipe);
	}
	
	return pipes;
	
}
