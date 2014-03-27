
#include "Main.hpp"

#include "JsonObject.hpp"

#include <boost/program_options.hpp>

using namespace std;
using namespace boost;
namespace po = boost::program_options;

bool setup_main(int argc, char *argv[], JsonObject *pipes, JsonObject *root, log4cxx::LoggerPtr *logger) {
    
    po::positional_options_description pd;
    pd.add("pipes", 1);
    pd.add("config", 1);
    pd.add("name", 1);
    
    po::options_description desc("options");
    desc.add_options()
    ("help", "produce help message")
    ("pipes", po::value<string>(), "The JSON for pipes")
    ("config", po::value<string>(), "The JSON for config")
    ("name", po::value<string>(), "The name for this task")
    ;
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
              options(desc).positional(pd).run(), vm);
    po::notify(vm);
    
    // minimal args
    if (vm.count("help") || !vm.count("pipes") || !vm.count("config") || !vm.count("name")) {
        LOG4CXX_ERROR((*logger), desc);
        return false;
    }
    
	{
		stringstream outfn;
		outfn << "org.skweltch." << vm["name"].as<string>();
		*logger = log4cxx::Logger::getLogger(outfn.str());
	}
    

 	{
 		stringstream ss(vm["pipes"].as<string>());
		if (!pipes->read(*logger, &ss)) {
			return false;
		}
 	}
    
 	{
 		stringstream ss(vm["config"].as<string>());
		if (!root->read(*logger, &ss)) {
			return false;
		}
 	}
    
    return true;
}

