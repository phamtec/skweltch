
#include "nodesHandler.hpp"
#include "../RestContext.hpp"
#include "JsonObject.hpp"
#include "JsonArray.hpp"
#include "JsonPath.hpp"
#include "GraphLayout.hpp"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/optional.hpp>

using namespace std;
using namespace boost;
using namespace http::server;

#define NODE_WIDTH 100
#define NODE_HEIGHT 100

void addRect(JsonObject *info, int x, int y) {

	JsonObject rect;
	rect.add("left", x);
	rect.add("top", y);
	rect.add("width", NODE_WIDTH);
	rect.add("height", NODE_HEIGHT);
	info->add("rect", rect);

}

void addNode(JsonObject *pt, const JsonObject &node, 
		const string &type, int x, int y, JsonArray *connections) {
		
	string name = node.getString("name");
	
	JsonObject info;
	info.add("type", type);
	
	int count = node.getInt("count", 0);
	if (count > 0) {
		info.add("count", count);
	}
	addRect(&info, x, y);
	
	pt->add(name, info);
	
	// and a connection.
	JsonObject confconn = JsonPath().getPath(node, "config.connections");
	if (!confconn.empty()) {
		for (JsonObject::iterator i = confconn.begin(); i != confconn.end(); i++) {
			JsonObject o = confconn.getValue(i);
			if (o.getString("direction") == "to") {
				JsonObject c;
				c.add("from", name);
				c.add("to", o.getString("block"));
				connections->add(c);
			}
		}
	}
}

void addEdges(GraphLayout *l, const map<string, int> &ids, const JsonObject &node) {

	string name = node.getString("name");

	// and a connection.
	JsonObject confconn = JsonPath().getPath(node, "config.connections");
	if (!confconn.empty()) {
		for (JsonObject::iterator i = confconn.begin(); i != confconn.end(); i++) {
			JsonObject o = confconn.getValue(i);
			if (o.getString("direction") == "to") {
				int from = ids.at(name);
				int to = ids.at(o.getString("block"));
				if (from == to) {
					throw new runtime_error("from same as to for " + name);
				}
				l->addEdge(from, to);
			}
		}
	}

}

reply::status_type nodesHandler(RestContext *context, const std::string &args, vector<header> *headers, string *content) {

	if (!context->isLoaded()) {
		return reply::no_content;
	}
	
	int size = 300;
	int centerx = 150;
	int centery = 150;
	vector<string> argsa;
	split(argsa, args, is_any_of("&"));
	for (vector<string>::iterator i=argsa.begin(); i != argsa.end(); i++) {
		vector<string> nvp;
		split(nvp, *i, is_any_of("="));
		if (nvp[0] == "size") {
			size = lexical_cast<int>(nvp[1]);
		}
		else if (nvp[0] == "centerx") {
			centerx = lexical_cast<int>(nvp[1]);
		}
		else if (nvp[0] == "centery") {
			centery = lexical_cast<int>(nvp[1]);
		}
	}
	
	JsonObject pt;
	JsonObject *root = context->getRoot();

	GraphLayout l;
	
	JsonArray bg = root->getArray("background");
	
	// add the vertexes.
	l.addVertexes(bg.size() + 2);
	
	// create the ids.
	map<string, int> ids;
	{
		int id = 0;
		ids[root->getChild("vent").getString("name")] = id++;
		for (JsonArray::iterator i = bg.begin(); i != bg.end(); i++) {
			ids[bg.getValue(i).getString("name")] = id++;
		}
		ids[root->getChild("reap").getString("name")] = id++;
	}
	
	// add the edges.
	{
		addEdges(&l, ids, root->getChild("vent"));
		for (JsonArray::iterator i = bg.begin(); i != bg.end(); i++) {
			addEdges(&l, ids, bg.getValue(i));
		}
		addEdges(&l, ids, root->getChild("reap"));
	}
	
	// lay the graph out.
	std::vector<std::pair<int, int> > centers;
	try {
		l.layout(&centers, size, centerx, centery);
	}
	catch (runtime_error &e) {
		BOOST_LOG_TRIVIAL(error) << e.what();
		return reply::bad_request;
	}
	
	JsonArray connections;
	
	int id = ids[root->getChild("vent").getString("name")];
	addNode(&pt, root->getChild("vent"), "vent", centers[id].first, centers[id].second, &connections);
	{
		for (JsonArray::iterator i = bg.begin(); i != bg.end(); i++) {
			id = ids[bg.getValue(i).getString("name")];
			addNode(&pt, bg.getValue(i), "background", centers[id].first, centers[id].second, &connections);
		}
	}
	id = ids[root->getChild("reap").getString("name")];
	addNode(&pt, root->getChild("reap"), "reap", centers[id].first, centers[id].second, &connections);
	
	// now put the connections in.
	if (!connections.empty()) {
		pt.add("connections", connections);
	}
	
	ostringstream ss;
	pt.write(true, &ss);
	*content = ss.str();
	
	return reply::ok;
	
}

