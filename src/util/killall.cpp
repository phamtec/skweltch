
#include "Logging.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <signal.h>
#include <czmq.h>
#include <log4cxx/logger.h>
#include <log4cxx/helpers/exception.h>

using namespace std;
using namespace boost;
namespace po = boost::program_options;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.killall"));

int main (int argc, char *argv[])
{
    setup_logging();
    
    po::positional_options_description pd;
    pd.add("pidfile", 1);
    
    po::options_description desc("options");
    desc.add_options()
    ("help", "produce help message")
    ("pidfile", po::value<string>(), "file containing a list of pids to kill.")
    ;
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
              options(desc).positional(pd).run(), vm);
    po::notify(vm);
    
    // minimal args
    if (vm.count("help") || !vm.count("pidfile")) {
        LOG4CXX_ERROR(logger, desc);
        return 0;
    }
    
    string filename(vm["pidfile"].as<string>().c_str());
	ifstream pidfile(filename.c_str());
	if (pidfile.is_open()) {
		string line;
		while (getline(pidfile, line)) {
			pid_t pid = lexical_cast<int>(line);
			::kill(pid, SIGTERM);
 			zclock_sleep(20);
			::kill(pid, SIGKILL);
		}
	}
    
    return 0;
}
