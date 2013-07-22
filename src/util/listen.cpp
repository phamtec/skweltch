
#include "Interrupt.hpp"

#include <zmq.hpp>
#include <czmq.h>
#include <msgpack.hpp>

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

using namespace std;
using namespace log4cxx;
using namespace log4cxx::helpers;

LoggerPtr logger(Logger::getLogger("org.skweltch"));

int main (int argc, char *argv[])
{
	// Set up a simple configuration that logs on the console.
	PropertyConfigurator::configure("log4cxx.conf");

	if (argc != 3) {
		LOG4CXX_ERROR(logger, "usage: " << argv[0] << " bind|connect port")
		LOG4CXX_ERROR(logger, "\tlistens on the port and unpacks any message that comes through.")
		return 1;
	}
	
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    
    if (string(argv[1]) == "bind") {
 		try {
			receiver.bind(argv[2]);
		}
		catch (zmq::error_t &e) {  	
			LOG4CXX_ERROR(logger, "couldn't bind should be: tcp://*:port")
			return 1;
		}
    }
    else if (string(argv[1]) == "connect") {
 		try {
			receiver.connect(argv[2]);
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
