#define BOOST_TEST_MODULE msg-tests
#include <boost/test/unit_test.hpp>

#include "../IntMsg.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <msgpack.hpp>

using namespace std;
using namespace boost;

struct SetupLogging
{
    SetupLogging() {
		log4cxx::BasicConfigurator::configure();
    }
};

BOOST_AUTO_TEST_SUITE( msgTests )

BOOST_GLOBAL_FIXTURE( SetupLogging )

BOOST_AUTO_TEST_CASE( intTest )
{
	IntMsg m(1, 2, 3);
	zmq::message_t message(2);
	m.set(&message);
	IntMsg m2(message);
	BOOST_CHECK(m2.getId() == 1);
	BOOST_CHECK(m2.getTime() == 2);
	BOOST_CHECK(m2.getPayload() == 3);
	
}

BOOST_AUTO_TEST_SUITE_END()
