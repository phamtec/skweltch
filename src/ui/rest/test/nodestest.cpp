#define BOOST_TEST_MODULE config-tests
#include <boost/test/unit_test.hpp>

#include "JsonConfig.hpp"
#include "../nodesHandler.hpp"
#include "../../RestContext.hpp"

#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost;
using namespace boost::property_tree;
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
	
	cout << content << endl;
	{
		stringstream jresult(content);
		ptree pt;
  		read_json(jresult, pt);
  		ptree xxx = pt.get_child("xxx");
  		BOOST_CHECK(xxx.get("count", 0) == 10);
  		BOOST_CHECK(xxx.get<string>("type") == "background");
  		ptree rect = xxx.get_child("rect");
  		BOOST_CHECK(rect.get("left", 0) == 10);
  		BOOST_CHECK(rect.get("top", 0) == 10);
  		BOOST_CHECK(rect.get("width", 0) == 100);
  		BOOST_CHECK(rect.get("height", 0) == 100);
	}
	
}
