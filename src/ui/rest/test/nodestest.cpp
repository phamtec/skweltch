#define BOOST_TEST_MODULE config-tests
#include <boost/test/unit_test.hpp>

#include "JsonConfig.hpp"
#include "../nodesHandler.hpp"
#include "../../RestContext.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace std;
using namespace boost;
using namespace http::server;

BOOST_AUTO_TEST_CASE( nodesTest )
{
	std::vector<header> headers;
	std::string content;
	BOOST_CHECK(nodesHandler(RestContext::getContext(), "", &headers, &content) == reply::no_content);


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
	BOOST_CHECK(nodesHandler(RestContext::getContext(), "", &headers, &content) == reply::ok);
	vector<string> lines;
	split(lines, content, is_any_of("\n"));
	BOOST_CHECK(lines.size() == 55);
	BOOST_CHECK(lines[7] == "        \"type\": \"background\"");
	
}
