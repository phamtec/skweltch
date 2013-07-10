
#include "Ports.hpp"

#include "JsonObject.hpp";

#include <boost/property_tree/ptree.hpp>
#include <zmq.hpp>

using namespace std;
using namespace boost;
using namespace boost::property_tree;

void Ports::join(zmq::i_socket_t *socket, const JsonObject &pipes, const JsonObject &config, const string &name) {

	JsonObject conn = config.getChild("connections").getChild(name);
	JsonObject pipe = pipes.getChild(conn.getString("pipe"));
	int port = pipe.getInt("port", -1);
	
	if (conn.getString("mode") == "bind") {
		stringstream ss;
		ss << "tcp://*:" << port;
    	socket->bind(ss.str().c_str());
	}
	else {
		string node = pipe.getString("node");
		stringstream ss;
		ss << "tcp://" << node << ":" << port;
    	socket->connect(ss.str().c_str());
	}
	
}
