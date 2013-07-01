
#ifndef __PORTS_HPP_INCLUDED__
#define __PORTS_HPP_INCLUDED__

#include <string>

class JsonNode;

class Ports {

public:

	std::string getBindSocket(JsonNode *pipes, JsonNode *root, const std::string &name);
	std::string getConnectSocket(JsonNode *pipes, JsonNode *root, const std::string &name);

};

#endif // __PORTS_HPP_INCLUDED__

