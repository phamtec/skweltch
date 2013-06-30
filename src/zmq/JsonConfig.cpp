
#include "JsonConfig.hpp"

#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <iostream>

using namespace std;
using namespace boost;

bool JsonConfig::read(JsonNode *root, ostream *outfile) {

	try
    {
  		property_tree::read_json(*jsonstream, root->pt);
  		return true;
    }
    catch(property_tree::json_parser::json_parser_error &je)
    {
        *outfile << "Error parsing: " << je.filename() << " on line: " << je.line() << std::endl;
        *outfile << je.message() << std::endl;
        return false;
    }
}

void JsonNode::getChild(const std::string &key, JsonNode *child) {
	child->pt = pt.get_child(key);
}

void JsonNode::setCurrent() {
	cur.reset(new JsonNode());
	cur->pt = i->second;
}

void JsonNode::start() {
	i = pt.begin();
	setCurrent();
}

bool JsonNode::hasMore() {
	return i != pt.end();
}

void JsonNode::next() {
	i++;
	if (hasMore()) {
		setCurrent();
	}
}

string JsonNode::getString(const string &key) { 
	return pt.get<string>(key); 
}

int JsonNode::getInt(const string &key, int def) { 
	return pt.get(key, def); 
}

std::string JsonNode::getChildAsString(const std::string &key) {
	boost::property_tree::ptree c = pt.get_child(key);
	ostringstream ss;
	property_tree::write_json(ss, c, false);
	string s = ss.str();
	trim(s);           
	return s;
}
