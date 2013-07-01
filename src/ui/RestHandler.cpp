
#include "RestHandler.hpp"
#include "RestContext.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace http::server;

// some restful handlers.
handlerType successHandler;
handlerType badRequestHandler;
handlerType helloHandler;

RestHandler::RestHandler() {

	// some simple handlers.
	handlers["/rest/success"] = &successHandler;
	handlers["/rest/badRequest"] = &badRequestHandler;
	handlers["/rest/hello"] = &helloHandler;
	
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

