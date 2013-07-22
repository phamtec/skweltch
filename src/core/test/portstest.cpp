#define BOOST_TEST_MODULE ports-tests
#include <boost/test/unit_test.hpp>

#include "JsonObject.hpp"
#include "../Ports.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <zmq.hpp>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>

using namespace std;
using namespace boost;

struct SetupLogging
{
    SetupLogging() {
		log4cxx::BasicConfigurator::configure();
    }
};

BOOST_AUTO_TEST_SUITE( portsTests )

BOOST_GLOBAL_FIXTURE( SetupLogging )

MOCK_BASE_CLASS( mock_socket, zmq::i_socket_t )
{
	MOCK_METHOD( send, 2 )
	MOCK_METHOD( recv, 2 )
	MOCK_METHOD( bind, 1 )
	MOCK_METHOD( connect, 1 )
	MOCK_METHOD( setsockopt, 3 )
};

BOOST_AUTO_TEST_CASE( calcPortsTest )
{
	string pipesjson = "{\"pullFrom\":{\"address\":\"*\",\"port\":5557,\"mode\":\"bind\"},"
		"\"pushTo\":{\"node\":\"192.168.0.1\",\"port\":5558,\"mode\":\"connect\"}}";

  	JsonObject pipes;
 	{
 		stringstream ss(pipesjson);
		BOOST_CHECK(pipes.read(log4cxx::Logger::getRootLogger(), &ss));
 	}
 	
 	mock_socket s1, s2;
 	
	MOCK_EXPECT(s1.bind).with("tcp://*:5557");
	MOCK_EXPECT(s2.connect).with("tcp://192.168.0.1:5558");
	MOCK_EXPECT(s1.setsockopt).with(mock::any, mock::any, mock::any);
	MOCK_EXPECT(s2.setsockopt).with(mock::any, mock::any, mock::any);
 	
	Ports p(log4cxx::Logger::getRootLogger());
	p.join(&s1, pipes, "pullFrom");
	p.join(&s2, pipes, "pushTo");
}

BOOST_AUTO_TEST_SUITE_END()
