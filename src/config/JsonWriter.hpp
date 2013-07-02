
#ifndef __JSONWRITER_HPP_INCLUDED__
#define __JSONWRITER_HPP_INCLUDED__

#include <boost/property_tree/ptree.hpp>
#include <boost/any.hpp>
#include <iostream>

typedef boost::property_tree::basic_ptree<std::string, boost::any> any_ptree;

/**
	A single node in the config file.
*/
class JsonWriter {

private:

	bool prettyPrint;
	
public:

	JsonWriter(bool pp=false) { prettyPrint = pp; }
	
	void write(const any_ptree &pt, std::ostream *s);
	
private:

	void writeNode(const any_ptree &pt, std::ostream *s, int level);
	void writeKey(const std::string &k, std::ostream *s);
	void writeData(const any_ptree &pt, std::ostream *ss);
	void writeString(const std::string &k, std::ostream *s);
	void writeInt(int i, std::ostream *ss);
	void writeDouble(double d, std::ostream *ss);
	
};

#endif // __JSONWRITER_HPP_INCLUDED__
