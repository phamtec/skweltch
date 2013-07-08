
#ifndef __JSONCONFIG_HPP_INCLUDED__
#define __JSONCONFIG_HPP_INCLUDED__

#include <iostream>
#include <string>

class JsonObject;

/**
	A class for reading the config file and parseing it.
*/
class JsonConfig {

	std::istream *jsonstream;
	
public:
	JsonConfig(std::istream *s) : jsonstream(s) {}

	bool read(JsonObject *root, std::ostream *outfile);
	std::string getChildAsString(const JsonObject &obj, const std::string &key);
	
};

#endif // __JSONCONFIG_HPP_INCLUDED__
