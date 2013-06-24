
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

void JsonNode::start() {
	i = pt.begin();
	cur.reset(new JsonNode());
	cur->pt = i->second;
}

bool JsonNode::hasMore() {
	return i != pt.end();
}

void JsonNode::next() {
	i++;
	if (i != pt.end()) {
		cur.reset(new JsonNode());
		cur->pt = i->second;
	}
}

string JsonNode::getString(const string &key) { 
	return pt.get<string>(key); 
}

int JsonNode::getInt(const string &key, int def) { 
	return pt.get(key, def); 
}
