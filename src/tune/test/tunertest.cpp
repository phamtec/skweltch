#define BOOST_TEST_MODULE tune-tests
#include <boost/test/unit_test.hpp>

#include "JsonObject.hpp"
#include "JsonConfig.hpp"
#include "JsonPath.hpp"
#include "../MachineTuner.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace boost;

BOOST_AUTO_TEST_CASE( tunerTest )
{
	stringstream configjson(
"{\n"
"	\"vent\" : {\n"
"	},\n"
"	\"background\": [\n"
"	],\n"
"	// and this is what is used to reap.\n"
"	\"reap\": {\n"
"		\"name\": \"aaa\",\n"
"		\"config\" : {\n"
"			\"totalTime\": 1000\n"
"		}\n"
"	}\n"
"}\n"
);

	stringstream tunerconfigjson(
"{\n"
"	\"mutations\": 10,\n"
"	\"iterations\": 50,\n"
"	\"success\": {\n"
"    	\"average\" : 5.6082130965593784\n"
"    },\n"
"	\"reap.config\" : { \n"
"		\"group\": 1,\n"
"		\"var\": \"totalTime\",\n"
"		\"type\": \"int\",\n"
"		\"select\": \"hightolow\",\n"
"		\"low\": 50,\n"
"		\"high\": 2000,\n"
"		\"enabled\": true\n"
"	}\n"
"}\n"
);

	JsonObject config;
 	{
 		JsonConfig json(&configjson);
		BOOST_CHECK(json.read(&config));
 	}
	JsonObject tunerconfig;
 	{
 		JsonConfig json(&tunerconfigjson);
		BOOST_CHECK(json.read(&tunerconfig));
 	}

	MachineTuner tuner(&config, &tunerconfig);
	tuner.tune(1, 0);
	BOOST_CHECK(JsonPath().getPath(config, "reap.config").getInt("totalTime", -1) == 2000);
	tuner.tune(1, 1);
	BOOST_CHECK(JsonPath().getPath(config, "reap.config").getInt("totalTime", -1) == 1805);
	tuner.tune(1, 2);
	BOOST_CHECK(JsonPath().getPath(config, "reap.config").getInt("totalTime", -1) == 1610);
	tuner.tune(1, 3);
	BOOST_CHECK(JsonPath().getPath(config, "reap.config").getInt("totalTime", -1) == 1415);
}

BOOST_AUTO_TEST_CASE( tuner2Test )
{
	stringstream configjson(
"{\n"
"	\"vent\" : {\n"
"	},\n"
"	\"background\": [\n"
"	],\n"
"	// and this is what is used to reap.\n"
"	\"reap\": {\n"
"		\"name\": \"aaa\",\n"
"		\"config\" : {\n"
"			\"totalTime\": 1000\n"
"		}\n"
"	}\n"
"}\n"
);

	stringstream tunerconfigjson(
"{\n"
"	\"mutations\": 6,\n"
"	\"iterations\": 30,\n"
"	\"success\": {\n"
"    	\"average\" : 5.6082130965593784\n"
"    },\n"
"	\"reap.config\" : { \n"
"		\"group\": 1,\n"
"		\"var\": \"totalTime\",\n"
"		\"type\": \"int\",\n"
"		\"select\": \"hightolow\",\n"
"		\"low\": 50,\n"
"		\"high\": 500,\n"
"		\"enabled\": true\n"
"	}\n"
"}\n"
);

	JsonObject config;
 	{
 		JsonConfig json(&configjson);
		BOOST_CHECK(json.read(&config));
 	}
	JsonObject tunerconfig;
 	{
 		JsonConfig json(&tunerconfigjson);
		BOOST_CHECK(json.read(&tunerconfig));
 	}

	MachineTuner tuner(&config, &tunerconfig);
	tuner.tune(1, 0);
	tuner.tune(1, 5);
	BOOST_CHECK(JsonPath().getPath(config, "reap.config").getInt("totalTime", -1) == 125);
}
