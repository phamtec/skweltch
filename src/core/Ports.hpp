
#ifndef __PORTS_HPP_INCLUDED__
#define __PORTS_HPP_INCLUDED__

#include "JsonObject.hpp"
#include <string>

class Ports {

public:

	std::string getBindSocket(const JsonObject &pipes, const JsonObject &config, const std::string &name);
	std::string getConnectSocket(const JsonObject &pipes, const JsonObject &config, const std::string &name);

};

#endif // __PORTS_HPP_INCLUDED__

