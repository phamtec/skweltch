#include "Ports.hpp"
#include "Logging.hpp"

#include <zmq.hpp>
#include <iostream>
#include <fstream>
#include <czmq.h>
#include <zclock.h>
#include <boost/lexical_cast.hpp>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

using namespace std;
using namespace boost;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.grimreaper"));

int main (int argc, char *argv[])
{
    setup_logging();
    
	if (argc != 4) {
        LOG4CXX_ERROR(logger, "usage: " << argv[0] << " pidFile config name");
		return 1;
	}
	
 	{
		stringstream outfn;
		outfn << "org.skweltch." << argv[3];
		logger = log4cxx::Logger::getLogger(outfn.str());
	}
		
	JsonObject root;
 	{
 		stringstream ss(argv[2]);
		if (!root.read(logger, &ss)) {
			return 1;
		}
 	}

 	int totaltime = root.getInt("totalTime", 5000);
 	zclock_sleep(totaltime);
	LOG4CXX_INFO(logger, "waited " << totaltime << "ms, killing everything.")
 	
	string filename(argv[1]);
	ifstream pidfile(filename.c_str());	
	if (pidfile.is_open()) {
		string line;
		while (getline(pidfile, line)) {
			pid_t pid = lexical_cast<int>(line);
			::kill(pid, SIGTERM);
 			zclock_sleep(20);
			::kill(pid, SIGKILL);
		}
		pidfile.close();
		remove(filename.c_str());
	}

}
