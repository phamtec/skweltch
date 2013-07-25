#define BOOST_TEST_MODULE track-tests
#include <boost/test/unit_test.hpp>

#include "../MsgTracker.hpp"

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

BOOST_AUTO_TEST_SUITE( trackTests )

BOOST_GLOBAL_FIXTURE( SetupLogging )

BOOST_AUTO_TEST_CASE( simpleTest )
{
	MsgTracker t(log4cxx::Logger::getRootLogger());
	t.setFirst(20);
	for (int i=20; i<=30; i++) {
		t.track(i);
	}
	BOOST_CHECK(!t.complete());
	for (int i=31; i<=40; i++) {
		t.track(i);
	}
	BOOST_CHECK(!t.complete());
	t.setLast(40);
	BOOST_CHECK(t.complete());
}

BOOST_AUTO_TEST_CASE( lastBitNotSetTest )
{
	MsgTracker t(log4cxx::Logger::getRootLogger());
	t.setFirst(20);
	for (int i=20; i<=23; i++) {
		t.track(i);
	}
	t.setLast(24);
	BOOST_CHECK(!t.complete());
}

BOOST_AUTO_TEST_CASE( lastGrowsBucketsTest )
{
	MsgTracker t(log4cxx::Logger::getRootLogger());
	t.setFirst(20);
	t.setLast(24);
	BOOST_CHECK(!t.complete());
}

BOOST_AUTO_TEST_SUITE_END()
