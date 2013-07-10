#define BOOST_TEST_MODULE smoke-tests
#include <boost/test/unit_test.hpp>

#include "JsonConfig.hpp"
#include "JsonObject.hpp"
#include "../Ports.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <zmq.hpp>

using namespace std;
using namespace boost;

MOCK_BASE_CLASS( mock_socket, zmq::i_socket_t )
{
	MOCK_METHOD( send, 2 )
	MOCK_METHOD( recv, 2 )
	MOCK_METHOD( bind, 1 )
	MOCK_METHOD( connect, 1 )
};

BOOST_AUTO_TEST_CASE( pidFileExistsTest )
{
	string configjson = "{" 
		"\"connections\":{" 
			"\"pullFrom\":{" 
				"\"pipe\":\"pipe2\"," 
				"\"direction\":\"from\","
				"\"mode\":\"bind\""
			"}," 
			"\"pushTo\":{" 
				"\"pipe\":\"pipe1\"," 
				"\"direction\":\"to\","
				"\"mode\":\"connect\""
			"}"
		"}"
	"}";
	string pipesjson = "{"
		"\"pipe1\":{"
			"\"node\":\"192.168.0.1\","
			"\"port\":5558"
		"},"
		"\"pipe2\":{"
			"\"node\":\"192.168.0.1\","
			"\"port\":5557"
		"},"
		"\"pipe3\":{"
			"\"node\":\"192.168.0.1\","
			"\"port\":5559"
		"}"
	"}";

  	JsonObject pipes;
 	{
 		stringstream ss(pipesjson);
 		JsonConfig json(&ss);
		BOOST_CHECK(json.read(&pipes));
 	}
	JsonObject config;
 	{
 		stringstream ss(configjson);
 		JsonConfig json(&ss);
		BOOST_CHECK(json.read(&config));
 	}
 	
 	mock_socket s1, s2;
 	
	MOCK_EXPECT(s1.bind).with("tcp://*:5557");
	MOCK_EXPECT(s2.connect).with("tcp://192.168.0.1:5558");
 	
	Ports p;
//	p.bind(&s1, pipes, config, "pullFrom");
//	p.connect(&s2, pipes, config, "pushTo");
	p.join(&s1, pipes, config, "pullFrom");
	p.join(&s2, pipes, config, "pushTo");
}
