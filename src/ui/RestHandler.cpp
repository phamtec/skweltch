
#include "RestHandler.hpp"
#include "RestContext.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost;
using namespace http::server;

// some restful handlers.
reply::status_type successHandler(RestContext *context, const std::string &args, vector<header> *headers, string *content);
reply::status_type badRequestHandler(RestContext *context, const std::string &args, vector<header> *headers, string *content);
reply::status_type helloHandler(RestContext *context, const std::string &args, vector<header> *headers, string *content);
reply::status_type loadHandler(RestContext *context, const std::string &args, vector<header> *headers, string *content);
reply::status_type nodesHandler(RestContext *context, const std::string &args, vector<header> *headers, string *content);

RestHandler::RestHandler() {

	// some simple handlers.
	handlers["/rest/success"] = &successHandler;
	handlers["/rest/badRequest"] = &badRequestHandler;
	handlers["/rest/hello"] = &helloHandler;
	handlers["/rest/load"] = &loadHandler;
	handlers["/rest/nodes"] = &nodesHandler;
	
}

void RestHandler::add(const string &path, handlerPointer func) {
	handlers[path] = func;
}

bool RestHandler::isRest(const string &path) {

	return path.find("/rest/") == 0;
	
}

reply::status_type RestHandler::call(const string &path, vector<header> *headers, string *content) {

	string p, args;
	size_t q = path.find_first_of("?");
	if (q != string::npos) {
		p = path.substr(0, q);
		args = path.substr(q+1);
	}
	else {
		p = path;
		args = "";
	}
	
	RestContext *context = RestContext::getContext();
	handlerPointer mfp = handlers[p];
	if (mfp) {
		return mfp(context, args, headers, content);
	}
	return reply::bad_request;
	
}

void RestContext::makeHeaders(vector<header> *headers, int size, const string &mimetype) {
	headers->resize(2);
	(*headers)[0].name = "Content-Length";
	(*headers)[0].value = boost::lexical_cast<string>(size);
	(*headers)[1].name = "Content-Type";
	(*headers)[1].value = mimetype;
}

reply::status_type successHandler(RestContext *context, const std::string &args, vector<header> *headers, string *content) {
	return reply::ok;
}

reply::status_type badRequestHandler(RestContext *context, const std::string &args, vector<header> *headers, string *content) {
	return reply::bad_request;
}

reply::status_type helloHandler(RestContext *context, const std::string &args, vector<header> *headers, string *content) {
	string s = "hello";
	context->makeHeaders(headers, s.length(), "text/text");
	content->append(s.c_str(), s.length());
	return reply::ok;
}

reply::status_type loadHandler(RestContext *context, const std::string &args, vector<header> *headers, string *content) {

	vector<string> argsa;
	split(argsa, args, is_any_of("&"));
	// allow more args.
	if (argsa.size() < 1) {
		return reply::bad_request;
	}
	vector<string> nvp;
	split(nvp, argsa[0], is_any_of("="));
	if (nvp.size() != 2 || nvp[0] != "file") {
		return reply::bad_request;
	}
	string fn = nvp[1];
	
	if (!context->load(fn)) {
		return reply::bad_request;
	}
	context->makeHeaders(headers, fn.length(), "text/text");
	content->append(fn.c_str(), fn.length());
	return reply::ok;
}

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
				for (int i=0; i<count; i++) {
					stringstream ss;
					ss << name << "(" << i << ")";
					boost::property_tree::ptree info;
					info.add("type", "background");
					pt.put_child(ss.str(), info);
				}
			}
			else {
				pt.put_child(name, info);
			}
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

