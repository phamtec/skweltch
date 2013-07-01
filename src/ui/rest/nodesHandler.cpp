
#include "nodesHandler.hpp"
#include "../RestContext.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

using namespace std;
using namespace boost;
using namespace boost::property_tree;
using namespace http::server;

#define NODE_WIDTH 100
#define NODE_HEIGHT 100
#define NODE_GAP 10
#define GRAPH_WIDTH 340

void addRect(ptree *info, int *x, int *y) {

	ptree rect;
	
	rect.add("left", lexical_cast<string>(*x));
	rect.add("top", lexical_cast<string>(*y));
	rect.add("width", lexical_cast<string>(NODE_WIDTH));
	rect.add("height", lexical_cast<string>(NODE_HEIGHT));
	
	info->put_child("rect", rect);
	*x += NODE_WIDTH + NODE_GAP;
	if (*x > GRAPH_WIDTH) {
		*x = NODE_GAP;
		*y += NODE_HEIGHT + NODE_GAP;
	}

}

void addConnection(ptree *info, ptree *connection, const string &name) {
	if (connection->get<string>("direction") == "from") {
		info->add("to", name);
	}
	else {
		info->add("from", name);
	}
}

void addNode(ptree *pt, JsonNode *node, JsonNode *pipes, 
		const string &type, int *x, int *y, ptree *connections) {
		
	int count = node->getInt("count", 0);
	string name(node->getString("name"));
	ptree info;
	info.add("type", type);
	if (count > 0) {
		stringstream ss;
		ss << count;
		info.add("count", ss.str());
	}
	addRect(&info, x, y);
	pt->put_child(name, info);
	
	// and a connection.
	optional<ptree &> child = node->getPt()->get_child_optional("config.connections");
	if (child) {
		for (ptree::iterator i = child->begin(); i != child->end(); i++) {
			string pipename = i->second.get<string>("pipe"); 
			ptree::assoc_iterator j = connections->find(pipename);
			if (j == connections->not_found()) {
				ptree info;
				addConnection(&info, &i->second, name);
				connections->put_child(pipename, info);
			}
			else {
				addConnection(&j->second, &i->second, name);
			}
		}
	}
}

reply::status_type nodesHandler(RestContext *context, const std::string &args, vector<header> *headers, string *content) {

	if (!context->isLoaded()) {
		return reply::no_content;
	}
	
	ptree pt;
	JsonNode *root = context->getRoot();

	// we need the pipes for below.
	JsonNode pipes;
	root->getChild("pipes", &pipes);

	ptree connections;
	
	int x = ((GRAPH_WIDTH - NODE_WIDTH)/2);
	int y = NODE_GAP;
	{
		JsonNode vent;
		root->getChild("vent", &vent);
		addNode(&pt, &vent, &pipes, "vent", &x, &y, &connections);
	}
	x = NODE_GAP;
	y = (NODE_GAP*2) + NODE_HEIGHT;
	{
		JsonNode bg;
		root->getChild("background", &bg);
		bg.start();
		while (bg.hasMore()) {
			addNode(&pt, bg.current(), &pipes, "background", &x, &y, &connections);
			bg.next();
		}
	}
	y += NODE_GAP + NODE_HEIGHT;
	x = ((GRAPH_WIDTH - NODE_WIDTH)/2);
	{
		JsonNode reap;
		root->getChild("reap", &reap);
		addNode(&pt, &reap, &pipes, "reap", &x, &y, &connections);
	}
	
	// now put the connections in.
	pt.put_child("connections", connections);
	
	ostringstream ss;
	property_tree::write_json(ss, pt, true);
	*content = ss.str();
	
	return reply::ok;
	
}

