
#ifndef __JSONCONFIG_HPP_INCLUDED__
#define __JSONCONFIG_HPP_INCLUDED__

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <string>

/**
	A class for reading the config file and parseing it.
*/
class JsonConfig {

	std::istream *jsonstream;
	
public:
	JsonConfig(std::istream *s) : jsonstream(s) {}

	bool read(boost::property_tree::ptree *root, std::ostream *outfile);
	std::string getChildAsString(const boost::property_tree::ptree &pt, const std::string &key);
};

#endif // __JSONCONFIG_HPP_INCLUDED__
