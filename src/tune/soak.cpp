
#include "JsonConfig.hpp"
#include "JsonObject.hpp"
#include "JsonPath.hpp"
#include "MachineRunner.hpp"

#include <iostream>
#include <fstream>
#include <czmq.h>
#include <zclock.h>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;

static int s_interrupted = 0;
static void s_signal_handler (int signal_value)
{
    s_interrupted = 1;
}

static void s_catch_signals (void)
{
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
}

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.soak"));

int main (int argc, char *argv[])
{
	log4cxx::PropertyConfigurator::configure("log4cxx.conf");

	if (argc != 4) {
		LOG4CXX_ERROR(logger, "usage: " << argv[0] << " jsonConfig count iterations")
		return 1;
	}
	
	int count = lexical_cast<int>(argv[2]);
	int iterations = lexical_cast<int>(argv[3]);
		
    s_catch_signals ();
 	MachineRunner runner(logger, &s_interrupted);
	cout << "group\ti\tvars\tn\tlow\thigh\tfail\tavg\tmed" << endl;
	runner.setFail(true);
 	for (int i=0; i<count; i++) {
 		LOG4CXX_INFO(logger, "soak start run.")
		try {
			if (!runner.runOne(argv[1], iterations, 0, 0, "")) {
				LOG4CXX_ERROR(logger, "failed, returning")
				break;
			}
		}
		catch (runtime_error &e) {
			LOG4CXX_ERROR(logger, e.what())
			return 1;
		}
 		LOG4CXX_INFO(logger, "soak end run.")
		if (s_interrupted) {
			break;
		}
	}
	
	return 0;
	
}
