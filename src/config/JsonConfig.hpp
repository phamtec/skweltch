
#ifndef __JSONCONFIG_HPP_INCLUDED__
#define __JSONCONFIG_HPP_INCLUDED__

#include <iostream>

class JsonNode;

/**
	A class for reading the config file and parseing it.
*/
class JsonConfig {

	std::istream *jsonstream;
	
public:
	JsonConfig(std::istream *s) : jsonstream(s) {}

	bool read(JsonNode *root, std::ostream *outfile);
	
};

#endif // __JSONCONFIG_HPP_INCLUDED__
