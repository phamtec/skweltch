#define BOOST_TEST_MODULE config-tests
#include <boost/test/unit_test.hpp>

#include "../JsonConfig.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace boost;

BOOST_AUTO_TEST_CASE( configTest )
{
	stringstream json(
"{\n"
"	\"pidFile\": \"pids.pid\",\n"
"	\"exePath\": \"./bin\",\n"
"	\n"
"	// these are all run in the background and remembered.\n"
"	\"background\": [\n"
"		{\n"
"			\"count\": 5, // need 5 of these. first arg to the program is the id.\n"
"			\"exe\": \"exe1\"\n"
"		},\n"
"		{\n"
"			// only 1 of these.\n"
"			\"exe\": \"exe2\"\n"
"		}\n"
"	],\n"
"	\n"
"	// this is what is used to feed the machine.\n"
"	\"run\": \"exe3\",\n"
"	\"config\" : {\n"
"		\"a\": \"x\",\n"
"		\"b\": \"y\"\n"
"	}\n"
"	\n"
"}\n"
);

	JsonConfig c(&json);
	property_tree::ptree r;
	BOOST_CHECK(c.read(&r, &std::cout));

	BOOST_CHECK(r.get<string>("pidFile") == "pids.pid");
	BOOST_CHECK(r.get<string>("exePath") == "./bin");
	property_tree::ptree bg = r.get_child("background");
	property_tree::iterator i = bg.begin();
	BOOST_CHECK(i != bg.end());
	BOOST_CHECK(i->second.get("count", 0) == 5);
	BOOST_CHECK(i->second.get<string>("exe") == "exe1");
	i++;
	BOOST_CHECK(i != bg.end());
	BOOST_CHECK(i->second.get("count", 0) == 0);
	BOOST_CHECK(i->second.get<string>("exe") == "exe2");
	i++;
	BOOST_CHECK(i == bg.end();
	BOOST_CHECK(r.get<string>("run") == "exe3");
	BOOST_CHECK(c.gteChildAsString(r, "config") == "{\"a\":\"x\",\"b\":\"y\"}");
    
}

BOOST_AUTO_TEST_CASE( badConfig1Test )
{
	stringstream json(
"{\n"
"	\"vent\" : {}\n"
"	\"reap\": {}\n"
"}\n"
);

	JsonConfig c(&json);
	property_tree::ptree r;
	BOOST_CHECK(!c.read(&r, &std::cout));
    
}
