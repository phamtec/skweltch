
#include "nodesHandler.hpp"
#include "../RestContext.hpp"
#include "JsonObject.hpp"
#include "JsonArray.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/optional.hpp>

using namespace std;
using namespace boost;
using namespace http::server;

#define NODE_WIDTH 100
#define NODE_HEIGHT 100
#define NODE_GAP 10
#define GRAPH_WIDTH 340

void addRect(JsonObject *info, int *x, int *y) {

	JsonObject rect;
	
	rect.add("left", *x);
	rect.add("top", *y);
	rect.add("width", NODE_WIDTH);
	rect.add("height", NODE_HEIGHT);
	
	info->add("rect", rect);
	*x += NODE_WIDTH + NODE_GAP;
	if (*x > GRAPH_WIDTH) {
		*x = NODE_GAP;
		*y += NODE_HEIGHT + NODE_GAP;
	}

}

void addConnection(JsonObject *info, const JsonObject &connection, const string &name) {
	if (connection.getString("direction") == "from") {
		info->add("to", name);
	}
	else {
		info->add("from", name);
	}
}

void addNode(JsonObject *pt, const JsonObject &node, 
		const string &type, int *x, int *y, JsonArray *connections) {
		
	int count = node.getInt("count", 0);
	string name(node.getString("name"));
	
	JsonObject info;
	info.add("type", type);
	if (count > 0) {
		info.add("count", count);
	}
	addRect(&info, x, y);
	pt->add(name, info);
	
	// and a connection.
	JsonObject config = node.getChild("config");
	JsonObject confconn = config.getChild("connections");
	if (!confconn.empty()) {
		for (JsonObject::iterator i = confconn.begin(); i != confconn.end(); i++) {
			JsonObject o = confconn.getValue(i);
			if (o.getString("direction") == "to") {
				JsonObject c;
				c.add("from", name);
				c.add("to", o.getString("block"));
				connections->add(&c);
			}
		}
	}
}

reply::status_type nodesHandler(RestContext *context, const std::string &args, vector<header> *headers, string *content) {

	if (!context->isLoaded()) {
		return reply::no_content;
	}
	
	JsonObject pt;
	JsonObject *root = context->getRoot();

	JsonArray connections;
	
	int x = ((GRAPH_WIDTH - NODE_WIDTH)/2);
	int y = NODE_GAP;
	{
		JsonObject vent = root->getChild("vent");
		addNode(&pt, vent, "vent", &x, &y, &connections);
	}
	x = NODE_GAP;
	y = (NODE_GAP*2) + NODE_HEIGHT;
	{
		JsonArray bg = root->getArray("background");
		for (JsonArray::iterator i = bg.begin(); i != bg.end(); i++) {
			addNode(&pt, bg.getValue(i), "background", &x, &y, &connections);
		}
	}
	y += NODE_GAP + NODE_HEIGHT;
	x = ((GRAPH_WIDTH - NODE_WIDTH)/2);
	{
		JsonObject reap = root->getChild("reap");
		addNode(&pt, reap, "reap", &x, &y, &connections);
	}
	
	// now put the connections in.
	if (!connections.empty()) {
		pt.add("connections", connections);
	}
	
	ostringstream ss;
	pt.write(true, &ss);
	*content = ss.str();
	
	return reply::ok;
	
}

