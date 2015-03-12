
#include "Ports.hpp"

#include "JsonObject.hpp"

#include <zmq.hpp>
#include <log4cxx/logger.h>

using namespace std;
using namespace boost;

/**
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
bool Ports::join(zmq::i_socket_t *socket, const JsonObject &ports, const string &name) {

	JsonObject pipe = ports.getChild(name);
	
	// if there is no pipe, simply don't join
	if (!pipe.isObject()) {
		LOG4CXX_WARN(logger, "no pipe " << name << " so not joining.")
		return true;
	}
	
	int port = pipe.getInt("port", -1);
	string mode = pipe.getString("mode");
	if (mode == "bind") {
		if (!bind(&logger, socket, pipe.getString("address"), port, name)) {
			return false;
		}
	}
	else if (mode == "connect") {
		string node = pipe.getString("node");
		stringstream ss;
		ss << "tcp://" << node << ":" << port;
 		try {
    		socket->connect(ss.str().c_str());
    	}
		catch (zmq::error_t &e) {  	
			LOG4CXX_ERROR(logger, "couldn't connect should be: tcp://localhost:port")
			return false;
		}
		LOG4CXX_INFO(logger, name << " connected to " << ss.str())
	}
	else {
		LOG4CXX_ERROR(logger, "for " << name << ", bind or connect. Pick one.")
		return false;
	}

	// this might be from a setting at some point, at the moment it's just global.
	try {
		int linger = -1;
		socket->setsockopt(ZMQ_LINGER, &linger, sizeof linger);
 	}
	catch (zmq::error_t &e) {
		LOG4CXX_ERROR(logger, e.what())
		return false;
	}  
 
	return true;
}

bool Ports::bind(log4cxx::LoggerPtr *logger, zmq::i_socket_t *socket, const string &address, int port, const string &name) {
	stringstream ss;
	ss << "tcp://" << address << ":" << port;
	try {
		socket->bind(ss.str().c_str());
	}
	catch (zmq::error_t &e) {  	
		LOG4CXX_ERROR((*logger), "couldn't bind should be: tcp://address:port")
		return false;
	}
	LOG4CXX_INFO((*logger), name << " bound to " << ss.str())
	return true;
}
