#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK 
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

BOOST_AUTO_TEST_SUITE( trackBigTests )

BOOST_GLOBAL_FIXTURE( SetupLogging )

BOOST_AUTO_TEST_CASE( largeOutOfOrderTest )
{
    MsgTracker t(log4cxx::Logger::getRootLogger());
    t.setFirst(1);
    for (int i=3002; i<=4504; i++) {
        t.track(i);
    }
    BOOST_CHECK(!t.complete());
    for (int i=1; i<=1000; i++) {
        t.track(i);
    }
    BOOST_CHECK(!t.complete());
    t.track(1001);
    BOOST_CHECK(!t.complete());
    for (int i=1002; i<=3000; i++) {
        t.track(i);
    }
    t.setLast(4504);
    BOOST_CHECK(!t.complete());
    t.track(3001);
//    t.dump();
    BOOST_CHECK(t.complete());
}

BOOST_AUTO_TEST_SUITE_END()
