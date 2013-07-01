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
"	\"pipes\": {\n"
"		\"pipe1\": {\n"
"			\"node\": \"localhost\",\n"
"			\"port\": 5558\n"
"		},\n"
"		\"pipe2\": {\n"
"			\"node\": \"localhost\",\n"
"			\"port\": 5557\n"
"		}\n"
"	},\n"
"	\"background\": [\n"
"		{\n"
"			\"count\": 10,\n"
"			\"name\": \"xxx\",\n"
"			\"exe\": \"xxx\",\n"
"			\"config\" : {\n"
"				\"connections\": {\n"
"					\"pullFrom\": {\n"
"						\"pipe\": \"pipe2\",\n"
"						\"direction\": \"from\" \n"
"					}\n"
"				}\n"
"			}\n"
"		},\n"
"		{\n"
"			\"name\": \"yyy\",\n"
"			\"exe\": \"yyy\",\n"
"			\"config\" : {\n"
"				\"connections\": {\n"
"					\"pushTo\": {\n"
"						\"pipe\": \"pipe2\",\n"
"						\"direction\": \"to\" \n"
"					}\n"
"				}\n"
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
  		BOOST_CHECK(rect.get("left", 0) > 0);
  		BOOST_CHECK(rect.get("top", 0) > 0);
  		BOOST_CHECK(rect.get("width", 0) > 0);
  		BOOST_CHECK(rect.get("height", 0) > 0);
  		
  		ptree pipe = pt.get_child("connections.pipe2");
 		BOOST_CHECK(pipe.get<string>("from") == "yyy");
 		BOOST_CHECK(pipe.get<string>("to") == "xxx");
 	}
	
}
