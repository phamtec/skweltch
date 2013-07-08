#define BOOST_TEST_MODULE smoke-tests
#include <boost/test/unit_test.hpp>

#include "JsonConfig.hpp"
#include "JsonObject.hpp"
#include "../Ports.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace boost;

BOOST_AUTO_TEST_CASE( pidFileExistsTest )
{
	string configs = "{\"connections\":{\"pullFrom\":{\"pipe\":\"pipe2\",\"direction\":\"from\"},\"pushTo\":{\"pipe\":\"pipe1\",\"direction\":\"to\"}}}";
	string pipess = "{\"pipe1\":{\"node\":\"192.168.0.1\",\"port\":5558},\"pipe2\":{\"node\":\"192.168.0.1\",\"port\":5557},\"pipe3\":{\"node\":\"192.168.0.1\",\"port\":5559}}";

  	JsonObject pipes;
 	{
 		stringstream ss(pipess);
 		JsonConfig json(&ss);
		BOOST_CHECK(json.read(&pipes, &cout));
 	}
	JsonObject config;
 	{
 		stringstream ss(configs);
 		JsonConfig json(&ss);
		BOOST_CHECK(json.read(&config, &cout));
 	}
 	
	Ports p;

	BOOST_CHECK(p.getBindSocket(pipes, config, "pullFrom") == "tcp://*:5557");
	BOOST_CHECK(p.getConnectSocket(pipes, config, "pullFrom") == "tcp://192.168.0.1:5557");
}
