
#include "JsonObject.hpp"
#include "JsonPath.hpp"
#include "MachineRunner.hpp"
#include "Interrupt.hpp"
#include "Logging.hpp"

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

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.soak"));

int main (int argc, char *argv[])
{
    setup_logging();
    
	if (argc != 4) {
        LOG4CXX_ERROR(logger, "usage: " << argv[0] << " jsonConfig count iterations");
		return 1;
	}
	
	int count = lexical_cast<int>(argv[2]);
	int iterations = lexical_cast<int>(argv[3]);
		
    s_catch_signals ();
 	MachineRunner runner(logger, &s_interrupted, 1000, 120);
	cout << "group\ti\tvars\tn\tlow\thigh\tfail\tavg\tmed" << endl;
//	runner.setFail(true);
 	for (int i=0; i<count; i++) {
 		LOG4CXX_INFO(logger, "soak start run.")
		try {
			if (!runner.runOne(argv[1], iterations, 0, 0, "", JsonObject())) {
//				LOG4CXX_ERROR(logger, "failed, returning")
//				break;
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
