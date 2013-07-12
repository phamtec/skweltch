
#include "PipeBuilder.hpp"

#include "JsonObject.hpp";
#include "JsonArray.hpp";
#include "JsonNamePredicate.hpp";

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

using namespace std;
using namespace boost;

JsonObject PipeBuilder::collect(JsonObject *config, const JsonObject &block) {

	JsonObject pipes;
	string blockname = block.getString("name");
	JsonObject connections = block.getChild("config").getChild("connections");
	for (JsonObject::iterator i = connections.begin(); i != connections.end(); i++) {
		JsonObject pipe;
		string name = connections.getKey(i);
		JsonObject val = connections.getValue(i);
		JsonObject b;
		if (val.getString("direction") == "to") {
			// need to look this block up as some other block's connection.
			b = config->findObj(JsonNamePredicate(val.getString("block")));
		}
		else {
			// the block is right here.
			b = block;
		}
		if (b.isObject()) {
			try {
				pipe.add("node", b.getString("node"));
				pipe.add("port", b.getInt("port", -1));
			}
			catch (runtime_error &e) {
				BOOST_LOG_TRIVIAL(error) << "syntax error. " << b.getString("name") << " missing node or port";
				throw e;
			}
			pipes.add(name, pipe);
		}
	}
	
	return pipes;
	
}
