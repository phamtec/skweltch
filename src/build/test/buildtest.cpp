#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK 
#include <boost/test/unit_test.hpp>

#include "ExeRunner.hpp"
#include "Elapsed.hpp"
#include "../BoostAnalyser.hpp"
#include "../FileModChecker.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <log4cxx/basicconfigurator.h>

using namespace std;
using namespace boost;

struct SetupLogging
{
    SetupLogging() {
		log4cxx::BasicConfigurator::configure();
    }
};

BOOST_AUTO_TEST_SUITE( buildTests )

BOOST_GLOBAL_FIXTURE( SetupLogging )

BOOST_AUTO_TEST_CASE( goodTest )
{
	
	stringstream ss("Performing configuration checks\n"
"\n"
"    - zlib                     : yes\n"
"...patience...\n"
"...patience...\n"
"...patience...\n"
"...found 14819 targets...\n"
"...updating 4 targets...\n"
"darwin.compile.c++ build/test/bin/darwin-4.2.1/release/threading-multi/buildtest.o\n"
"darwin.link build/test/bin/darwin-4.2.1/release/threading-multi/build\n"
"testing.unit-test build/test/bin/darwin-4.2.1/release/threading-multi/build.passed\n"
"Running 1 test case...\n"
"\n"
"*** No errors detected\n"
"common.copy dist/build.passed\n"
"...updated 4 targets...\n"
"\n");
	
	BuildStatus stat = BoostAnalyser(log4cxx::Logger::getRootLogger()).analyse(&ss);
	BOOST_CHECK(stat.workDone);
	BOOST_CHECK(stat.targets == 4);
	BOOST_CHECK(stat.passedTests == 1);
	BOOST_CHECK(stat.success);

}

BOOST_AUTO_TEST_CASE( badTest )
{
	
	stringstream ss("Performing configuration checks\n"
"\n"
"    - zlib                     : yes\n"
"...patience...\n"
"...patience...\n"
"...patience...\n"
"...found 14819 targets...\n"
"...updating 4 targets...\n"
"darwin.compile.c++ build/test/bin/darwin-4.2.1/release/threading-multi/buildtest.o\n"
"build/test/buildtest.cpp: In member function 'void buildTests::smokeTest::test_method()':\n"
"build/test/buildtest.cpp:28: error: 'ccc' was not declared in this scope\n"
"build/test/buildtest.cpp:31: error: expected `;' before '}' token\n"
"\n"
"    \"g++\"  -ftemplate-depth-128 -O3 -finline-functions -Wno-inline -Wall -dynamic -gdwarf-2 -fexceptions -fPIC -ftemplate-depth-300 -DBOOST_PROGRAM_OPTIONS_DYN_LINK=1 -DBOOST_TEST_DYN_LINK=1 -DBOOST_TEST_NO_AUTO_LINK=1 -DNDEBUG  -I\"/opt/boost_1_54_0\" -I\"config\" -I\"core\" -I\"cppzmq-master\" -I\"json_spirit_v4.06/json_spirit\" -I\"msg\" -I\"turtle-1.2.4/include\" -c -o \"build/test/bin/darwin-4.2.1/release/threading-multi/buildtest.o\" \"build/test/buildtest.cpp\"\n"
"\n"
"...failed darwin.compile.c++ build/test/bin/darwin-4.2.1/release/threading-multi/buildtest.o...\n"
"...removing build/test/bin/darwin-4.2.1/release/threading-multi/buildtest.o\n"
"...skipped <pbuild/test/bin/darwin-4.2.1/release/threading-multi>build for lack of <pbuild/test/bin/darwin-4.2.1/release/threading-multi>buildtest.o...\n"
"...skipped <pbuild/test/bin/darwin-4.2.1/release/threading-multi>build.passed for lack of <pbuild/test/bin/darwin-4.2.1/release/threading-multi>build...\n"
"...skipped <pdist>build.passed for lack of <pbuild/test/bin/darwin-4.2.1/release/threading-multi>build.passed...\n"
"...failed updating 1 target...\n"
"...skipped 3 targets...\n"
"\n");
	
	BuildStatus stat = BoostAnalyser(log4cxx::Logger::getRootLogger()).analyse(&ss);
	BOOST_CHECK(stat.workDone);
	BOOST_CHECK(stat.targets == 4);
	BOOST_CHECK(stat.passedTests == 0);
	BOOST_CHECK(!stat.success);

}

BOOST_AUTO_TEST_CASE( noWorkTest )
{
	
	stringstream ss("Performing configuration checks\n"
"\n"
"    - zlib                     : yes\n"
"...patience...\n"
"...patience...\n"
"...patience...\n"
"...found 14823 targets...\n"
"\n");
	
	BuildStatus stat = BoostAnalyser(log4cxx::Logger::getRootLogger()).analyse(&ss);
	BOOST_CHECK(!stat.workDone);
	BOOST_CHECK(stat.targets == 0);
	BOOST_CHECK(stat.passedTests == 0);
	BOOST_CHECK(!stat.success);

}

BOOST_AUTO_TEST_CASE( failedTestTest )
{
	
	stringstream ss("Performing configuration checks\n"
"\n"
"    - zlib                     : yes\n"
"    - has_icu builds           : no\n"
"...patience...\n"
"...patience...\n"
"...patience...\n"
"...found 14917 targets...\n"
"...updating 4 targets...\n"
"darwin.compile.c++ build/test/bin/darwin-4.2.1/debug/threading-multi/buildtest.o\n"
"darwin.link build/test/bin/darwin-4.2.1/debug/threading-multi/build\n"
"testing.unit-test build/test/bin/darwin-4.2.1/debug/threading-multi/build.passed\n"
"Running 3 test cases...\n"
"build/test/buildtest.cpp:51: error in \"goodTest\": check stat.passedTests == 2 failed\n"
"\n"
"*** 1 failure detected in test suite \"build-tests\"\n"
"\n"
"    DYLD_LIBRARY_PATH=\"/Users/paul/Documents/skweltch/src/config/bin/darwin-4.2.1/debug/threading-multi:/Users/paul/Documents/skweltch/src/core/bin/darwin-4.2.1/debug/threading-multi:/opt/boost_1_54_0/bin.v2/libs/program_options/build/darwin-4.2.1/debug/threading-multi:/opt/boost_1_54_0/bin.v2/libs/regex/build/darwin-4.2.1/debug/threading-multi:/opt/boost_1_54_0/bin.v2/libs/test/build/darwin-4.2.1/debug/threading-multi:$DYLD_LIBRARY_PATH\"\n"
"export DYLD_LIBRARY_PATH\n"
"\n"
"     \"build/test/bin/darwin-4.2.1/debug/threading-multi/build\"  && touch  \"build/test/bin/darwin-4.2.1/debug/threading-multi/build.passed\"\n"
"\n"
"...failed testing.unit-test build/test/bin/darwin-4.2.1/debug/threading-multi/build.passed...\n"
"...removing build/test/bin/darwin-4.2.1/debug/threading-multi/build.passed\n"
"...skipped <pdist>build.passed for lack of <pbuild/test/bin/darwin-4.2.1/debug/threading-multi>build.passed...\n"
"...failed updating 1 target...\n"
"...skipped 1 target...\n"
"...updated 2 targets...\n"
"\n");
	
	BuildStatus stat = BoostAnalyser(log4cxx::Logger::getRootLogger()).analyse(&ss);
	BOOST_CHECK(stat.workDone);
	BOOST_CHECK(stat.targets == 4);
	BOOST_CHECK(stat.passedTests == 2);
	BOOST_CHECK(!stat.success);

}

BOOST_AUTO_TEST_CASE( noTestsTest )
{
	
	stringstream ss("...found 8 targets...\n"
"...updating 2 targets...\n"
"darwin.compile.c++ bin/darwin-4.2.1/debug/hello.o\n"
"darwin.link bin/darwin-4.2.1/debug/hello\n"
"...updated 2 targets...\n");

	BuildStatus stat = BoostAnalyser(log4cxx::Logger::getRootLogger()).analyse(&ss);
	BOOST_CHECK(stat.workDone);
	BOOST_CHECK(stat.targets == 2);
	BOOST_CHECK(stat.passedTests == 0);
	BOOST_CHECK(stat.success);

}

BOOST_AUTO_TEST_CASE( noWorkAtAllTest )
{
	stringstream ss("...found 8 targets...\n");
	
	BuildStatus stat = BoostAnalyser(log4cxx::Logger::getRootLogger()).analyse(&ss);
	BOOST_CHECK(!stat.workDone);
	BOOST_CHECK(stat.targets == 0);
	BOOST_CHECK(stat.passedTests == 0);
	BOOST_CHECK(!stat.success);

}

BOOST_AUTO_TEST_CASE( detectFileChangesTest )
{
	// make sure that 20 CRC's of the full source tree don't take longer than 1/2 a second.
	Elapsed t;
	t.start();
	long lastcrc = 0;
	for (int i=0; i<20; i++) {
		// do a CRC.
		long crc = FileModChecker(log4cxx::Logger::getRootLogger()).getCrc(".");
		BOOST_CHECK(crc != 0);
		if (lastcrc != 0) {
			BOOST_CHECK(crc == lastcrc);
		}
		lastcrc = crc;
	}
	BOOST_CHECK(t.getTotal() < 500);

}
 
BOOST_AUTO_TEST_SUITE_END()

