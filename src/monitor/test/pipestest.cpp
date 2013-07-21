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

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>

using namespace std;
using namespace boost;

BOOST_AUTO_TEST_CASE( pipesTest )
{
	log4cxx::BasicConfigurator::configure();

	string configjson = "{\n"
"	\"pidFile\": \"tasks.pid\",\n"
"	\"vent\" : {\n"
"		\"name\": \"Block1\",\n"
"		\"node\": \"localhost\",\n"
"		\"connections\": {\n"
"			\"pushTo\": {\n"
"				\"direction\": \"to\",\n"
"				\"mode\":\"bind\",\n"
"				\"address\": \"*\",\n"
"				\"port\": 5557\n"
"			},\n"
"			\"syncTo\": {\n"
"				\"direction\": \"to\",\n"
"				\"mode\":\"connect\",\n"
"				\"block\": \"Block3\",\n"
"				\"connection\": \"pullFrom\"\n"
"			}\n"
"		}\n"
"	},\n"
"	\"background\": [\n"
"		{\n"
"			\"name\": \"Block2\",\n"
"			\"node\": \"localhost\",\n"
"			\"connections\": {\n"
"				\"pullFrom\": {\n"
"					\"direction\": \"from\",\n"
"					\"mode\":\"connect\",\n"
"					\"block\": \"Block1\",\n"
"					\"connection\": \"pushTo\"\n"
"				},\n"
"				\"pushTo\": {\n"
"					\"direction\": \"to\",\n"
"					\"block\": \"Block3\",\n"
"					\"mode\":\"connect\",\n"
"					\"connection\": \"pullFrom\"\n"
"				}\n"
"			}\n"
"		},\n"
"		{\n"
"			\"name\": \"Block3\",\n"
"			\"node\": \"localhost\",\n"
"			\"connections\": {\n"
"				\"pullFrom\": {\n"
"					\"direction\": \"from\",\n"
"					\"mode\":\"bind\",\n"
"					\"address\": \"*\",\n"
"					\"port\": 5559\n"
"				}\n"
"			}\n"
"		}\n"
"	]\n"
"}\n";

	JsonObject config;
 	{
 		stringstream ss(configjson);
 		JsonConfig json(&ss);
		BOOST_CHECK(json.read(&config));
 	}
 	
 	JsonArray bg = config.getArray("background");
 	JsonArray::iterator i = bg.begin();
 	JsonObject block = bg.getValue(i);
 	
 	PipeBuilder	builder(log4cxx::Logger::getRootLogger());
 	JsonObject pipes = builder.collect(&config, block);
 	stringstream s;
 	pipes.write(false, &s);

 	BOOST_CHECK(s.str() == "{\"pullFrom\":{\"node\":\"localhost\",\"port\":5557,\"mode\":\"connect\"},\"pushTo\":{\"node\":\"localhost\",\"port\":5559,\"mode\":\"connect\"}}");

	block = config.getChild("vent");
 	pipes = builder.collect(&config, block);
 	stringstream s2;
 	pipes.write(false, &s2);
	
 	BOOST_CHECK(s2.str() == "{\"pushTo\":{\"address\":\"*\",\"port\":5557,\"mode\":\"bind\"},\"syncTo\":{\"node\":\"localhost\",\"port\":5559,\"mode\":\"connect\"}}");

}
