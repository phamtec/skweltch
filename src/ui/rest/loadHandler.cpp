
#include "loadHandler.hpp"
#include "../RestContext.hpp"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace std;
using namespace boost;
using namespace http::server;

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
