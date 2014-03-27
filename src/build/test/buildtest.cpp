#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK 
#include <boost/test/unit_test.hpp>

#include "ExeRunner.hpp"
#include "Elapsed.hpp"
#include "../MakeAnalyser.hpp"
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
	
	stringstream ss(
        "[  5%] Built target configLib\n"
        "[ 91%] Built target xpath2Test\n"
        "Scanning dependencies of target buildLib\n"
        "[ 93%] Building CXX object build/CMakeFiles/buildLib.dir/FileModChecker.cpp.o\n"
        "Linking CXX shared library libbuildLib.dylib\n"
        "[ 94%] Built target buildLib\n"
        "Linking CXX executable polldir\n"
        "[100%] Built target buildTest\n"
        "Install the project...\n"
        "-- Install configuration: \"\"\n"
        "-- Installing: /Users/paul/Documents/skweltch/src/bin/libconfigLib.dylib\n"
        "Running tests...\n"
        "Test project /Users/paul/Documents/skweltch/src/unix\n"
        "      Start  1: mainTest\n"
        " 1/1 Test  #1: mainTest .........................   Passed    0.01 sec\n"
        "\n"
        "100% tests passed, 0 tests failed out of 1\n"
        "\n"
        "Total Test time (real) =   0.15 sec"
        );
	
	BuildStatus stat = MakeAnalyser(log4cxx::Logger::getRootLogger()).analyse(&ss);
	BOOST_CHECK(stat.workDone);
	BOOST_CHECK(stat.success);

}

BOOST_AUTO_TEST_CASE( badTest )
{
	
	stringstream ss(
        "[  5%] Built target configLib\n"
        "[ 41%] Built target machineLib\n"
        "Scanning dependencies of target machine\n"
        "[ 43%] Building CXX object machine/CMakeFiles/machine.dir/machine.cpp.o\n"
        "/Users/paul/Documents/skweltch/src/machine/machine.cpp:68:1: error: unknown type name 'x'\n"
        "x               s_catch_signals ();\n"
        "^\n"
        "/Users/paul/Documents/skweltch/src/machine/machine.cpp:68:20: warning: empty parentheses interpreted as a function\n"
        "      declaration [-Wvexing-parse]\n"
        "x               s_catch_signals ();\n"
        "                                ^~\n"
        "/Users/paul/Documents/skweltch/src/machine/machine.cpp:68:20: note: replace parentheses with an initializer to\n"
        "      declare a variable\n"
        "x               s_catch_signals ();\n"
        "                                ^~\n"
        "                                 = 0\n"
        "1 warning and 1 error generated.\n"
        "make[2]: *** [machine/CMakeFiles/machine.dir/machine.cpp.o] Error 1\n"
        "make[1]: *** [machine/CMakeFiles/machine.dir/all] Error 2\n"
        "make: *** [all] Error 2"
        );
	
	BuildStatus stat = MakeAnalyser(log4cxx::Logger::getRootLogger()).analyse(&ss);
	BOOST_CHECK(stat.workDone);
	BOOST_CHECK(!stat.success);

}



BOOST_AUTO_TEST_CASE( failedTestTest )
{
	
	stringstream ss(
        "[  5%] Built target configLib\n"
        "[ 91%] Built target xpath2Test\n"
        "Scanning dependencies of target buildLib\n"
        "[ 93%] Building CXX object build/CMakeFiles/buildLib.dir/FileModChecker.cpp.o\n"
        "Linking CXX shared library libbuildLib.dylib\n"
        "[ 94%] Built target buildLib\n"
        "Linking CXX executable polldir\n"
        "[100%] Built target buildTest\n"
        "Install the project...\n"
        "-- Install configuration: \"\"\n"
        "-- Installing: /Users/paul/Documents/skweltch/src/bin/libconfigLib.dylib\n"
        "Running tests...\n"
        "Test project /Users/paul/Documents/skweltch/src/unix\n"
        "      Start  1: mainTest\n"
        " 1/1 Test  #1: mainTest .........................***Failed    0.01 sec\n"
        "\n"
        "2% tests passed, 1 tests failed out of 1\n"
        "\n"
        "Total Test time (real) =   0.15 sec"
        );
	
	BuildStatus stat = MakeAnalyser(log4cxx::Logger::getRootLogger()).analyse(&ss);
	BOOST_CHECK(stat.workDone);
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

