
#include "PipeBuilder.hpp"

#include "JsonObject.hpp";
#include "JsonArray.hpp";
#include "JsonNamePredicate.hpp";
#include "JsonPath.hpp";

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

using namespace std;
using namespace boost;

JsonObject PipeBuilder::collect(JsonObject *config, const JsonObject &block) {

	JsonObject pipes;
	
	JsonObject connections = JsonPath().getPath(block, "config.connections");
	for (JsonObject::iterator i = connections.begin(); i != connections.end(); i++) {
	
		JsonObject val = connections.getValue(i);
		// need to look this block up as some other block's connection.
		JsonObject b = val.getString("direction") == "to" ? config->findObj(JsonNamePredicate(val.getString("block"))) : block;

		if (b.isObject()) {
			try {
				JsonObject pipe;
				pipe.add("node", b.getString("node"));
				pipe.add("port", b.getInt("port", -1));
				pipes.add(connections.getKey(i), pipe);
			}
			catch (runtime_error &e) {
				BOOST_LOG_TRIVIAL(error) << "syntax error. " << b.getString("name") << " missing node or port";
				throw e;
			}
		}
	}
	
	return pipes;
	
}
