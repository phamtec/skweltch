
#include "Interrupt.hpp"
#include "Logging.hpp"

#include <zmq.hpp>
#include <czmq.h>
#include <msgpack.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <log4cxx/logger.h>
#include <log4cxx/helpers/exception.h>

using namespace std;
using namespace boost;
namespace po = boost::program_options;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.listen"));

int main (int argc, char *argv[])
{
    setup_logging();
    
    po::positional_options_description pd;
    pd.add("with", 1);
    pd.add("port", 1);
    
    po::options_description desc("options");
    desc.add_options()
    ("help", "produce help message")
    ("with", po::value<string>(), "bind or connect")
    ("port", po::value<string>(), "The port.")
    ;
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
              options(desc).positional(pd).run(), vm);
    po::notify(vm);
    
    // minimal args
    if (vm.count("help") || !vm.count("with") || !vm.count("port")) {
        LOG4CXX_ERROR(logger, desc);
        return 0;
    }
    
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    
    string with = vm["with"].as<string>();
    string port = vm["port"].as<string>();

    if (with == "bind") {
 		try {
			receiver.bind(port.c_str());
		}
		catch (zmq::error_t &e) {  	
			LOG4CXX_ERROR(logger, "couldn't bind should be: tcp://*:port")
			return 1;
		}
    }
    else if (with == "connect") {
 		try {
			receiver.connect(port.c_str());
		}
		catch (zmq::error_t &e) {  	
			LOG4CXX_ERROR(logger, "couldn't connect should be: tcp://localhost:port")
			return 1;
		}
    }
    else {
 		LOG4CXX_ERROR(logger, "bind or connect. You choose.");
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
        
		msgpack::unpacked msg;
		msgpack::unpack(&msg, (const char *)message.data(), message.size());
		msgpack::object obj = msg.get();
		
		LOG4CXX_INFO(logger, obj)
		
	}
    
	LOG4CXX_INFO(logger, "finished.")
   
    return 0;
}
