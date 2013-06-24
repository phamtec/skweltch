
#ifndef __JSONCONFIG_HPP_INCLUDED__
#define __JSONCONFIG_HPP_INCLUDED__

#include <boost/property_tree/ptree.hpp>

/**
	A single node in the config file.
*/
class JsonNode {

private:
	friend class JsonConfig;	
	boost::property_tree::ptree pt;
	boost::property_tree::ptree::iterator i;
	
public:

	/*
		use like this:
		
		JsonNode t;
		bg.start(&t);
		while (bg.hasMore()) {
			// use t
			bg.next(&t);
		}
	*/
	void start(JsonNode *child);
	bool hasMore();
	void next(JsonNode *child);
	
	// get a named child.
	void getChild(const std::string &key, JsonNode *child);
	
	// get various data types.
	std::string getString(const std::string &key);
	int getInt(const std::string &key, int def);
	
};

/**
	A class for reading the config file and parseing it.
*/
class JsonConfig {

	std::istream *jsonstream;
	
public:
	JsonConfig(std::istream *s) : jsonstream(s) {}

	void read(JsonNode *root);
	
};

#endif // __JSONCONFIG_HPP_INCLUDED__
