
#include "TaskMonitor.hpp"

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

using namespace std;
//using namespace boost;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.monitor"));

int main (int argc, char *argv[])
{
	log4cxx::PropertyConfigurator::configure("log4cxx.conf");

	if (argc != 2) {
		LOG4CXX_ERROR(logger, "usage: " << argv[0] << " jsonConfig")
		return 1;
	}
	
	TaskMonitor mon(logger);
	return mon.start(argv[1]);
	
}
