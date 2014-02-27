#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK 
#include <boost/test/unit_test.hpp>

#include "../IDataMsg.hpp"
#include "../IgnoreWhileWorking.hpp"
#include "../IClock.hpp"

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

BOOST_AUTO_TEST_SUITE( ignoreTests )

BOOST_GLOBAL_FIXTURE( SetupLogging )

MOCK_BASE_CLASS( mock_data_msg, IDataMsg )
{
	MOCK_METHOD( getId, 0 )
	MOCK_METHOD( getTime, 0 )
};

MOCK_BASE_CLASS( mock_clock, IClock )
{
	MOCK_METHOD( time, 0 )

};

BOOST_AUTO_TEST_CASE( simpleTest )
{
	mock_data_msg first;
	mock_data_msg second;
	mock_data_msg third;
	mock_clock clock;
	
	MOCK_EXPECT(first.getTime).returns(100);
	MOCK_EXPECT(second.getTime).returns(200);
	MOCK_EXPECT(third.getTime).returns(300);
	MOCK_EXPECT(clock.time).once().returns(105);
	MOCK_EXPECT(clock.time).once().returns(250);

	IgnoreWhileWorking ignore(&clock);
	BOOST_CHECK(!ignore.canIgnore(&first));
	ignore.start(&first);
	ignore.end();
	BOOST_CHECK(ignore.canIgnore(&second));
	BOOST_CHECK(!ignore.canIgnore(&third));
	
}

BOOST_AUTO_TEST_SUITE_END()
