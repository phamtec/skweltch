#define BOOST_TEST_MODULE pipes-tests
#include <boost/test/unit_test.hpp>

#include "JsonConfig.hpp"
#include "JsonObject.hpp"
#include "JsonArray.hpp"
#include "../PipeBuilder.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace boost;

BOOST_AUTO_TEST_CASE( pipesTest )
{
	string configjson = "{\n"
"	\"pidFile\": \"tasks.pid\",\n"
"	\"vent\" : {\n"
"		\"name\": \"Block1\",\n"
"		\"node\": \"192.168.0.1\",\n"
"		\"port\": 5557,\n"
"		\"config\" : {\n"
"			\"connections\": {\n"
"				\"pushTo\": {\n"
"					\"block\": \"Block2\",\n"
"					\"direction\": \"to\",\n"
"					\"mode\":\"bind\"\n"
"				},\n"
"				\"syncTo\": {\n"
"					\"block\": \"Block3\",\n"
"					\"direction\": \"to\",\n"
"					\"mode\":\"connect\"\n"
"				}\n"
"			}\n"
"		}\n"
"	},\n"
"	\"background\": [\n"
"		{\n"
"			\"count\": 3,\n"
"			\"name\": \"Block2\",\n"
"			\"node\": \"192.168.0.1\",\n"
"			\"port\": 5558,\n"
"			\"config\" : {\n"
"				\"connections\": {\n"
"					\"pullFrom\": {\n"
"						\"direction\": \"from\",\n"
"						\"mode\":\"connect\"\n"
"					},\n"
"					\"pushTo\": {\n"
"						\"block\": \"Block3\",\n"
"						\"direction\": \"to\",\n"
"						\"mode\":\"connect\"\n"
"					}\n"
"				}\n"
"			}\n"
"		},\n"
"		{\n"
"			\"name\": \"Block3\",\n"
"			\"node\": \"192.168.0.1\",\n"
"			\"port\": 5559,\n"
"			\"config\" : {\n"
"				\"connections\": {\n"
"					\"pullFrom\": {\n"
"						\"direction\": \"from\",\n"
"						\"mode\":\"bind\"\n"
"					}\n"
"				}\n"
"			}\n"
"		}\n"
"	],\n"
"	\"reap\": {\n"
"		\"name\": \"Block4\",\n"
"		\"node\": \"192.168.0.1\",\n"
"		\"port\": 5560,\n"
"		\"config\" : {\n"
"			\"connections\": {\n"
"				\"pullFrom\": {\n"
"					\"direction\": \"from\",\n"
"					\"mode\":\"bind\"\n"
"				}\n"
"			}\n"
"		}\n"
"	}\n"
"}";

	JsonObject config;
 	{
 		stringstream ss(configjson);
 		JsonConfig json(&ss);
		BOOST_CHECK(json.read(&config));
 	}
 	
 	JsonArray bg = config.getArray("background");
 	JsonArray::iterator i = bg.begin();
 	JsonObject block = bg.getValue(i);
 	PipeBuilder	builder;
 	JsonObject pipes = builder.collect(&config, block);
 	stringstream s;
 	pipes.write(false, &s);

 	BOOST_CHECK(s.str() == "{\"pullFrom\":{\"node\":\"192.168.0.1\",\"port\":5558},\"pushTo\":{\"node\":\"192.168.0.1\",\"port\":5559}}");
	
}
