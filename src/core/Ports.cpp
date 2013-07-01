
#include "Ports.hpp"

#include "JsonNode.hpp"

using namespace std;

string Ports::getBindSocket(JsonNode *pipes, JsonNode *root, const string &name) {

  	JsonNode connections;
	root->getChild("connections", &connections);
 	JsonNode cpipe;
	connections.getChild(name, &cpipe);
	JsonNode pipe;
	pipes->getChild(cpipe.getString("pipe"), &pipe);
	int port = pipe.getInt("port", -1);
	stringstream ss;
	ss << "tcp://*:" << port;
	return ss.str();
	
}

string Ports::getConnectSocket(JsonNode *pipes, JsonNode *root, const string &name) {

  	JsonNode connections;
	root->getChild("connections", &connections);
 	JsonNode cpipe;
	connections.getChild(name, &cpipe);
	JsonNode pipe;
	pipes->getChild(cpipe.getString("pipe"), &pipe);
	string node = pipe.getString("node");
	int port = pipe.getInt("port", -1);
	stringstream ss;
	ss << "tcp://" << node << ":" << port;
	return ss.str();
	
}

