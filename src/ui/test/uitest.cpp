#define BOOST_TEST_MODULE config-tests
#include <boost/test/unit_test.hpp>

#include "../RestHandler.hpp"
#include "../RestContext.hpp"
#include "JsonConfig.hpp"
#include "JsonNode.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace std;
using namespace boost;
using namespace http::server;

BOOST_AUTO_TEST_CASE( smokeTest )
{
	RestHandler handler;
	BOOST_CHECK(handler.isRest("/rest/hello"));
	BOOST_CHECK(handler.isRest("/rest/hello/xxx/yyy"));
	BOOST_CHECK(!handler.isRest("/restx/ssss"));
	BOOST_CHECK(!handler.isRest("/aaaa.txt"));
	BOOST_CHECK(!handler.isRest("/aaaa1.txt"));
}

BOOST_AUTO_TEST_CASE( callTest )
{

	RestHandler handler;
	std::vector<header> headers;
	std::string content;
	BOOST_CHECK(handler.call("/rest/badRequest", &headers, &content) == reply::bad_request);
	BOOST_CHECK(handler.call("/rest/success", &headers, &content) == reply::ok);
	BOOST_CHECK(handler.call("/rest/xxx", &headers, &content) == reply::bad_request);

}

http::server::reply::status_type testHandler(RestContext *context, const std::string &args, vector<header> *headers, string *content) {
	return reply::bad_gateway;
}

BOOST_AUTO_TEST_CASE( newHandlerTest )
{

	RestHandler handler;
	handler.add("/rest/test", &testHandler);
	std::vector<header> headers;
	std::string content;
	BOOST_CHECK(handler.call("/rest/test", &headers, &content) == reply::bad_gateway);

}

http::server::reply::status_type testArgsHandler(RestContext *context, const std::string &args, vector<header> *headers, string *content) {
	if (args == "x=y") {
		return reply::ok;
	}
	else {
		return reply::bad_request;
	}
}

BOOST_AUTO_TEST_CASE( argsTest )
{

	RestHandler handler;
	handler.add("/rest/test", &testArgsHandler);
	std::vector<header> headers;
	std::string content;
	BOOST_CHECK(handler.call("/rest/test?x=y", &headers, &content) == reply::ok);

}

BOOST_AUTO_TEST_CASE( nodesTest )
{

	RestHandler handler;
	std::vector<header> headers;
	std::string content;
	BOOST_CHECK(handler.call("/rest/nodes", &headers, &content) == reply::no_content);

	stringstream json(
"{\n"
"	\"background\": [\n"
"		{\n"
"			\"count\": 10,\n"
"			\"name\": \"xxx\",\n"
"			\"exe\": \"xxx\",\n"
"			\"config\" : {\n"
"			}\n"
"		},\n"
"		{\n"
"			\"name\": \"yyy\",\n"
"			\"exe\": \"yyy\",\n"
"			\"config\" : {\n"
"			}\n"
"		}\n"
"	],\n"
"	\"vent\" : {\n"
"		\"name\": \"zzzz\",\n"
"		\"exe\": \"zzzz\",\n"
"		\"config\" : {\n"
"		}\n"
"	},\n"
"	\"reap\": {\n"
"		\"name\": \"aaa\",\n"
"		\"exe\": \"aaa\",\n"
"		\"config\" : {\n"
"		}\n"
"	}\n"
"}\n");

	JsonConfig c(&json);
	BOOST_CHECK(c.read(RestContext::getContext()->getRoot(), &std::cout));
	RestContext::getContext()->setLoaded();
	BOOST_CHECK(handler.call("/rest/nodes", &headers, &content) == reply::ok);
	vector<string> lines;
	split(lines, content, is_any_of("\n"));
	BOOST_CHECK(lines.size() == 55);
	BOOST_CHECK(lines[7] == "        \"type\": \"background\"");
	
}
