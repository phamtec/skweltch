
#include "nodesHandler.hpp"
#include "../RestContext.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost;
using namespace http::server;

reply::status_type nodesHandler(RestContext *context, const std::string &args, vector<header> *headers, string *content) {

	if (!context->isLoaded()) {
		return reply::no_content;
	}
	
	boost::property_tree::ptree pt;
	JsonNode *root = context->getRoot();
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
			pt.put_child(name, info);
			bg.next();
		}
	}
	{
		JsonNode vent;
		root->getChild("vent", &vent);
		boost::property_tree::ptree info;
		info.add("type", "vent");
		pt.put_child(vent.getString("name"), info);
	}
	{
		JsonNode reap;
		root->getChild("reap", &reap);
		boost::property_tree::ptree info;
		info.add("type", "reap");
		pt.put_child(reap.getString("name"), info);
	}
	
	ostringstream ss;
	property_tree::write_json(ss, pt, true);
	*content = ss.str();
	
	return reply::ok;
	
}

