
#ifndef __JSONNODE_HPP_INCLUDED__
#define __JSONNODE_HPP_INCLUDED__

#include <boost/property_tree/ptree.hpp>

/**
	A single node in the config file.
*/
class JsonNode {

private:
	friend class JsonConfig;	
	boost::property_tree::ptree pt;
	boost::property_tree::ptree::iterator i;
	std::auto_ptr<JsonNode> cur;
	
	void setCurrent();

public:

	/*
		use like this:
		
		c.start();
		while (c.hasMore()) {
			// use c.current()
			c.next();
		}
	*/
	void start();
	bool hasMore();
	void next();
	JsonNode *current() { return cur.get(); }
	
	// get a named child.
	void getChild(const std::string &key, JsonNode *child);
	
	// get various data types.
	std::string getString(const std::string &key);
	int getInt(const std::string &key, int def);
	
	// get an entire subnode as a JsonString.
	std::string getChildAsString(const std::string &key);
	
};

#endif // __JSONNODE_HPP_INCLUDED__
