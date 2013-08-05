
#include "TaskMonitor.hpp"
#include "Interrupt.hpp"
#include "MachineMsg.hpp"
#include "JsonObject.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <zmq.hpp>
#include <msgpack.hpp>

using namespace std;
using namespace boost;
namespace po = boost::program_options;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.machine"));

int main (int argc, char *argv[])
{
	log4cxx::PropertyConfigurator::configure("log4cxx.conf");

    try {
		po::options_description desc("options");
		desc.add_options()
			("help", "produce help message")
			("port", po::value<int>()->default_value(8000), "the port to bind to.")
		;

		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).
				  options(desc).run(), vm);
		po::notify(vm);

		// minimal args
        if (vm.count("help")) {
			LOG4CXX_ERROR(logger, desc)
            return 0;
        }
 
     	//  Prepare our context and socket
    	zmq::context_t context(1);
    	zmq::socket_t receiver(context, ZMQ_PULL);
    
 		try {
 			stringstream ss;
 			ss << "tcp://*:" << vm["port"].as<int>();
			receiver.bind(ss.str().c_str());
		}
		catch (zmq::error_t &e) {  	
			LOG4CXX_ERROR(logger, "couldn't bind should be: tcp://*:port")
			return 1;
		}
		
		s_catch_signals ();
		while (1) {
	
			zmq::message_t message;
			try {
				receiver.recv(&message);
			}
			catch (zmq::error_t &e) {
				LOG4CXX_ERROR(logger, "recv failed." << e.what())
			}
		
			if (s_interrupted) {
				LOG4CXX_INFO(logger, "interrupt received, killing server")
				break;
			}
		
			MachineMsg msg(message);
		
			ifstream jsonfile(msg.getData().c_str());
			JsonObject r;
			if (!r.read(logger, &jsonfile)) {
				return 1;
			}
	
			vector<int> pids;
			TaskMonitor mon(logger);
			switch (msg.getCode()) {
			case 1:
				{
					if (!mon.start(&r, &pids, true)) {
						return 1;
					}
				}
				break;
				
			case 2:
				if (!mon.doVent(&r, 0)) {
					return 1;
				}
				break;
				
			case 3:
				if (!mon.doReap(&r, 0)) {
					return 1;
				}
				// wait till everything is gone.
				mon.waitFinish(pids);
				break;
			}
		}
	
		LOG4CXX_INFO(logger, "finished.")
	}
	catch (std::exception& e) {
		LOG4CXX_ERROR(logger, e.what())
	}
	
}
