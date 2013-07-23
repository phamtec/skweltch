#define BOOST_TEST_MODULE work-tests
#include <boost/test/unit_test.hpp>

#include "../Work.hpp"
#include "../IWorkWorker.hpp"

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

BOOST_AUTO_TEST_SUITE( workTests )

BOOST_GLOBAL_FIXTURE( SetupLogging )

MOCK_BASE_CLASS( mock_sink_worker, IWorkWorker )
{
};

BOOST_AUTO_TEST_CASE( simpleTest )
{

}

BOOST_AUTO_TEST_SUITE_END()
