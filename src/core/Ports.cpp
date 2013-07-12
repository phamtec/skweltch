
#include "Ports.hpp"

#include "JsonObject.hpp";

#include <zmq.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

using namespace std;
using namespace boost;

void Ports::join(zmq::i_socket_t *socket, const JsonObject &pipes, const JsonObject &config, const string &name) {

	JsonObject conn = config.getChild("connections").getChild(name);
	JsonObject pipe = pipes.getChild(name);
	
	// if there is no pipe, simply don't join
	if (!pipe.isObject()) {
		BOOST_LOG_TRIVIAL(warning) << "no pipe " << name << " so not joining.";
		return;
	}
	
	int port = pipe.getInt("port", -1);
	
	if (conn.getString("mode") == "bind") {
		stringstream ss;
		ss << "tcp://*:" << port;
    	socket->bind(ss.str().c_str());
		BOOST_LOG_TRIVIAL(debug) << name << " bound to " << ss.str();
	}
	else {
		string node = pipe.getString("node");
		stringstream ss;
		ss << "tcp://" << node << ":" << port;
    	socket->connect(ss.str().c_str());
		BOOST_LOG_TRIVIAL(debug) << name << " connected to " << ss.str();
	}
	
}
