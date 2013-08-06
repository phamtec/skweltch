
#include "server.hpp"

#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <fstream>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

using namespace std;
using namespace boost;
using namespace log4cxx;
using namespace log4cxx::helpers;

LoggerPtr logger(Logger::getLogger("org.skweltch.ui"));

int main(int argc, char* argv[])
{
	// Set up a simple configuration that logs on the console.
	PropertyConfigurator::configure("log4cxx.conf");

	if (argc != 4) {
		LOG4CXX_ERROR(logger, "usage: " << argv[0] << " <address> <port> <doc_root>")
		return 1;
	}
	
	try
	{
	
		// Initialise the server.
		http::server::server s(argv[1], argv[2], argv[3]);

		// Run the server until stopped.
		s.run();

	}
	catch (std::exception& e)
	{
		LOG4CXX_ERROR(logger, "exception: " << e.what())
	}

	return 0;
}
