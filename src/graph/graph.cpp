
#include "MachineGraph.hpp"
#include "Logging.hpp"

#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <log4cxx/logger.h>
#include <log4cxx/helpers/exception.h>

using namespace std;
using namespace boost;
namespace po = boost::program_options;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.graph"));

int main (int argc, char *argv[])
{
    setup_logging();
    
    po::positional_options_description pd;
    pd.add("jsonConfig", 1);
    
    po::options_description desc("options");
    desc.add_options()
    ("help", "produce help message")
    ("jsonConfig", po::value<string>(), "the config filename")
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
    
	ifstream json(argv[1]);
	MachineGraph(log4cxx::Logger::getRootLogger(), &cout).makeDOT(&json);
	
	return 0;
	
}
