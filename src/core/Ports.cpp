
#include "Ports.hpp"

#include <boost/property_tree/ptree.hpp>

using namespace std;
using namespace boost;
using namespace boost::property_tree;

string Ports::getBindSocket(const ptree &pipes, const ptree &root, const string &name) {

	stringstream path;
	path << "connections." << name << ".pipe";
 	ptree pipe = pipes.get_child(path.str());
	int port = pipe.get<int>("port", -1);
	stringstream ss;
	ss << "tcp://*:" << port;
	return ss.str();
	
}

string Ports::getConnectSocket(const ptree &pipes, const ptree &root, const string &name) {

	stringstream path;
	path << "connections." << name << ".pipe";
 	ptree pipe = pipes.get_child(path.str());
	int port = pipe.get<int>("port", -1);
	string node = pipe.get<string>("node");
	stringstream ss;
	ss << "tcp://" << node << ":" << port;
	return ss.str();
	
}

