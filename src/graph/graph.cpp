
#include "MachineGraph.hpp"

#include <iostream>
#include <fstream>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

using namespace std;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.graph"));

int main (int argc, char *argv[])
{
	log4cxx::PropertyConfigurator::configure("log4cxx.conf");

	if (argc != 2) {
		LOG4CXX_ERROR(logger, "usage: " << argv[0] << " jsonConfig")
		return 1;
	}
	
	ifstream json(argv[1]);
	MachineGraph(log4cxx::Logger::getRootLogger(), &cout).makeDOT(&json);
	
	return 0;
	
}
