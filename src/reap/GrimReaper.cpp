#include "JsonConfig.hpp"
#include "Ports.hpp"
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
  	log4cxx::PropertyConfigurator::configure("log4cxx.conf");

	if (argc != 3) {
		LOG4CXX_ERROR(logger, "usage: " << argv[0] << " config name")
		return 1;
	}
	
	{
		stringstream outfn;
		outfn << "org.skweltch." << argv[2];
		logger = log4cxx::Logger::getLogger(outfn.str());
	}
		
	JsonObject root;
 	{
 		stringstream ss(argv[1]);
 		JsonConfig json(&ss);
		if (!json.read(&root)) {
			return 1;
		}
 	}

 	int totaltime = root.getInt("totalTime", 5000);
 	zclock_sleep(totaltime);
	LOG4CXX_INFO(logger, "waited " << totaltime << "ms, killing everything.")
 	
	string filename(root.getString("pidFile"));
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
