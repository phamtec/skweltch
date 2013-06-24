
#include "JsonConfig.hpp"

#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#
using namespace std;
using namespace boost;

void JsonConfig::read(JsonNode *root) {

  	property_tree::read_json(*jsonstream, root->pt);

}

void JsonNode::getChild(const std::string &key, JsonNode *child) {
	child->pt = pt.get_child(key);
}

void JsonNode::start(JsonNode *child) {
	i = pt.begin();
	child->pt = i->second;
	i++;
}

bool JsonNode::hasMore() {
	return i != pt.end();
}

void JsonNode::next(JsonNode *child) {
	child->pt = i->second;
	i++;
}

string JsonNode::getString(const string &key) { 
	return pt.get<string>(key); 
}

int JsonNode::getInt(const string &key, int def) { 
	return pt.get(key, def); 
}
