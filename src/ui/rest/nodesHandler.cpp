
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
	
	rect.add<int>("left", *x);
	rect.add<int>("top", *y);
	rect.add<int>("width", NODE_WIDTH);
	rect.add<int>("height", NODE_HEIGHT);
	
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

void addNode(ptree *pt, boost::property_tree::ptree *node, boost::property_tree::ptree *pipes, 
		const string &type, int *x, int *y, ptree *connections) {
		
	int count = node->get("count", 0);
	string name(node->get<string>("name"));
	ptree info;
	info.add("type", type);
	if (count > 0) {
		info.add<int>("count", count);
	}
	addRect(&info, x, y);
	pt->put_child(name, info);
	
	// and a connection.
	optional<ptree &> child = node->get_child_optional("config.connections");
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
	boost::property_tree::ptree *root = context->getRoot();

	// we need the pipes for below.
	boost::property_tree::ptree pipes = root->get_child("pipes");

	ptree connections;
	
	int x = ((GRAPH_WIDTH - NODE_WIDTH)/2);
	int y = NODE_GAP;
	{
		boost::property_tree::ptree vent = root->get_child("vent");
		addNode(&pt, &vent, &pipes, "vent", &x, &y, &connections);
	}
	x = NODE_GAP;
	y = (NODE_GAP*2) + NODE_HEIGHT;
	{
		boost::property_tree::ptree bg = root->get_child("background");
		for (ptree::iterator i = bg.begin(); i != bg.end(); i++) {
			addNode(&pt, &i->second, &pipes, "background", &x, &y, &connections);
		}
	}
	y += NODE_GAP + NODE_HEIGHT;
	x = ((GRAPH_WIDTH - NODE_WIDTH)/2);
	{
		boost::property_tree::ptree reap = root->get_child("reap");
		addNode(&pt, &reap, &pipes, "reap", &x, &y, &connections);
	}
	
	// now put the connections in.
	pt.put_child("connections", connections);
	
	ostringstream ss;
	property_tree::write_json(ss, pt, true);
	*content = ss.str();
	
	return reply::ok;
	
}

