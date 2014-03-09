
#include "JsonObject.hpp"
#include "JsonArray.hpp"
#include "Logging.hpp"
#include "PipeBuilder.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <log4cxx/logger.h>
#include <log4cxx/helpers/exception.h>

using namespace std;
using namespace boost;
namespace po = boost::program_options;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.blockinfo"));

int main (int argc, char *argv[])
{
    setup_logging();
    
    po::positional_options_description pd;
    pd.add("jsonConfig", 1);
    pd.add("block", 1);
    
    po::options_description desc("options");
    desc.add_options()
    ("help", "produce help message")
    ("jsonConfig", po::value<string>(), "the machine config filename")
    ("block", po::value<string>(), "the name of a block to give info about.")
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

    string jsonConfig = vm["jsonConfig"].as<string>();
    if (!filesystem::exists(jsonConfig)) {
        LOG4CXX_ERROR(logger, jsonConfig << " does not exist");
        return 1;
    }
    string block = vm["block"].as<string>();
    
    try {
 		ifstream jsonfile(jsonConfig);
		JsonObject r;
		if (!r.read(logger, &jsonfile)) {
			return 1;
		}
        
        JsonArray blocks = r.getArray("blocks");
        for (JsonArray::iterator i = blocks.begin(); i != blocks.end(); i++) {
            JsonObject obj = blocks.getValue(i);
            if (obj.getString("name") == block) {
                cout << "exe: " << obj.getString("exe") << endl;
                
                JsonObject pipes = PipeBuilder(logger).collect(&r, obj);
                
                cout << "pipes: ";
                pipes.write(false, &cout);
                cout << endl;
                cout << "config: " << obj.getChildAsString("config") << endl;
                
                cout << "test with: " << endl;
                
                for (JsonObject::iterator i = pipes.begin(); i != pipes.end(); i++) {
                    
                    JsonObject val = pipes.getValue(i);
                    string name = pipes.getKey(i);
                    
                    string mode = val.getString("mode");
                    cout << "./listen " << mode;
                    if (mode == "bind") {
                        cout << " tcp://" << val.getString("address");
                    }
                    else if (mode == "connect") {
                        cout << " tcp://" << val.getString("node");
                    }
                    else {
                        cout << "\t\tbad mode" << endl;
                    }
                    cout << ":" << val.getInt("port", -1) << endl;
                }
                
                
                // no more.
                break;
            }
        }
 	}
	catch (std::exception& e) {
		LOG4CXX_ERROR(logger, e.what())
	}
    
    return 0;
}

