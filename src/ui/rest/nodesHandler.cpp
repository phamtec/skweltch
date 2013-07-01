
#include "nodesHandler.hpp"
#include "../RestContext.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost;
using namespace http::server;

#define NODE_WIDTH 100
#define NODE_HEIGHT 100
#define NODE_GAP 10
#define GRAPH_WIDTH 300

void addRect(boost::property_tree::ptree *info, int *x, int *y) {

	boost::property_tree::ptree rect;
	{
		stringstream ss;
		ss << *x;
		rect.add("left", ss.str());
	}
	{
		stringstream ss;
		ss << *y;
		rect.add("top", ss.str());
	}
	{
		stringstream ss;
		ss << NODE_WIDTH;
		rect.add("width", ss.str());
	}
	{
		stringstream ss;
		ss << NODE_HEIGHT;
		rect.add("height", ss.str());
	}
	info->put_child("rect", rect);
	*x += NODE_WIDTH + NODE_GAP;
	if (*x > GRAPH_WIDTH) {
		*x = NODE_GAP;
		*y += NODE_HEIGHT + NODE_GAP;
	}

}

reply::status_type nodesHandler(RestContext *context, const std::string &args, vector<header> *headers, string *content) {

	if (!context->isLoaded()) {
		return reply::no_content;
	}
	
	boost::property_tree::ptree pt;
	JsonNode *root = context->getRoot();
	int x = NODE_GAP, y = NODE_GAP;
	{
		JsonNode bg;
		root->getChild("background", &bg);
		bg.start();
		while (bg.hasMore()) {
			int count = bg.current()->getInt("count", 0);
			string name(bg.current()->getString("name"));
			boost::property_tree::ptree info;
			info.add("type", "background");
			if (count > 0) {
				stringstream ss;
				ss << count;
				info.add("count", ss.str());
			}
			addRect(&info, &x, &y);
			pt.put_child(name, info);
			bg.next();
		}
	}
	{
		JsonNode vent;
		root->getChild("vent", &vent);
		boost::property_tree::ptree info;
		info.add("type", "vent");
		addRect(&info, &x, &y);
		pt.put_child(vent.getString("name"), info);
	}
	{
		JsonNode reap;
		root->getChild("reap", &reap);
		boost::property_tree::ptree info;
		info.add("type", "reap");
		addRect(&info, &x, &y);
		pt.put_child(reap.getString("name"), info);
	}
	
	ostringstream ss;
	property_tree::write_json(ss, pt, true);
	*content = ss.str();
	
	return reply::ok;
	
}

