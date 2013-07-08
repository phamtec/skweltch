
#include "Ports.hpp"

#include "JsonObject.hpp";

#include <boost/property_tree/ptree.hpp>

using namespace std;
using namespace boost;
using namespace boost::property_tree;

string Ports::getBindSocket(const JsonObject &pipes, const JsonObject &config, const string &name) {

	JsonObject pipe = pipes.getChild(config.getChild("connections").getChild(name).getString("pipe"));
	int port = pipe.getInt("port", -1);
	stringstream ss;
	ss << "tcp://*:" << port;
	return ss.str();
	
}

string Ports::getConnectSocket(const JsonObject &pipes, const JsonObject &config, const string &name) {

	JsonObject pipe = pipes.getChild(config.getChild("connections").getChild(name).getString("pipe"));
	int port = pipe.getInt("port", -1);
	string node = pipe.getString("node");
	stringstream ss;
	ss << "tcp://" << node << ":" << port;
	return ss.str();
	
}

