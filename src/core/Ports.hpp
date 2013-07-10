
#ifndef __PORTS_HPP_INCLUDED__
#define __PORTS_HPP_INCLUDED__

#include "JsonObject.hpp"
#include <string>

namespace zmq {
	class i_socket_t;
}

/*
	Given configuration JSON, get a socket by name.
*/

class Ports {

public:

	void join(zmq::i_socket_t *socket, const JsonObject &pipes, const JsonObject &config, const std::string &name);
	
};

#endif // __PORTS_HPP_INCLUDED__

