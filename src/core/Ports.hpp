
#ifndef __PORTS_HPP_INCLUDED__
#define __PORTS_HPP_INCLUDED__

#include "JsonObject.hpp"
#include <string>
#include <log4cxx/logger.h>

namespace zmq {
	class i_socket_t;
}

/*
	Given configuration JSON, get a socket by name.
*/

class Ports {

	log4cxx::LoggerPtr logger;

public:
	Ports(log4cxx::LoggerPtr l) : logger(l) {}

	bool join(zmq::i_socket_t *socket, const JsonObject &ports, const std::string &name);

};

#endif // __PORTS_HPP_INCLUDED__

