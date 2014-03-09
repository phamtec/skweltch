
#include "TaskMonitor.hpp"

#include "JsonObject.hpp"
#include "Logging.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

using namespace std;
using namespace boost;
namespace po = boost::program_options;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.monitor"));

int main (int argc, char *argv[])
{
    setup_logging();
    
    po::positional_options_description pd;
    pd.add("jsonConfig", 1);

    po::options_description desc("options");
    desc.add_options()
        ("help", "produce help message")
        ("jsonConfig", po::value<string>(), "the config filename")
        ("demonize", po::value<bool>()->default_value(false), "should we stay running?")
        ("vent", po::value<bool>()->default_value(false), "do a vent.")
        ("reap", po::value<bool>()->default_value(false), "do a reap.")
        ("block", po::value<string>(), "the name of a block to focus on.")
    ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
              options(desc).positional(pd).run(), vm);
    po::notify(vm);

    // minimal args
    if (vm.count("help") || !vm.count("jsonConfig")) {
        LOG4CXX_ERROR(logger, desc);
        return 0;
    }
        
    try {
		ifstream jsonfile(argv[1]);
		JsonObject r;
		if (!r.read(logger, &jsonfile)) {
			return 1;
		}
	
		TaskMonitor mon(logger);
		if (vm["vent"].as<bool>()) {
			if (!mon.doVent(&r, 0)) {
				return 1;
			}
		}
		else if (vm["reap"].as<bool>()) {
			if (!mon.doReap(&r, 0)) {
				return 1;
			}
		}
		else if (vm.count("block")) {
			vector<int> pids;
			if (!mon.doBlock(&r, &pids, vm["block"].as<string>())) {
				return 1;
			}
			// wait till everything is gone.
			mon.waitFinish(pids);
		}
		else {
			vector<int> pids;
			if (!mon.start(&r, &pids, vm["demonize"].as<bool>())) {
				return 1;
			}
			// wait till everything is gone.
			mon.waitFinish(pids);
			
		}
	}
	catch (std::exception& e) {
		LOG4CXX_ERROR(logger, e.what())
	}
	
}
