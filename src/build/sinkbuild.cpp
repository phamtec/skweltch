#include "Ports.hpp"
#include "Elapsed.hpp"
#include "Interrupt.hpp"
#include "ISinkWorker.hpp"
#include "Logging.hpp"

#include <zmq.hpp>
#include <czmq.h>
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

#include <msgpack.hpp>

using namespace std;
using namespace boost;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.sinkbuild"));

int main (int argc, char *argv[])
{
    setup_logging();
    
 	if (argc != 4) {
        LOG4CXX_ERROR(logger, "usage: " << argv[0] << " pipes config name");
		return 1;
	}
	    
	{
		stringstream outfn;
		outfn << "org.skweltch." << argv[3];
		logger = log4cxx::Logger::getLogger(outfn.str());
	}
		
 	JsonObject pipes;
 	{
 		stringstream ss(argv[1]);
		if (!pipes.read(logger, &ss)) {
			return 1;
		}
 	}
	JsonObject root;
 	{
 		stringstream ss(argv[2]);
		if (!root.read(logger, &ss)) {
			return 1;
		}
 	}
 	
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    zmq::socket_t control(context, ZMQ_SUB);
    
 	Ports ports(logger);
    if (!ports.join(&receiver, pipes, "pullFrom")) {
    	return 1;
    }
    if (!ports.join(&control, pipes, "control")) {
    	return 1;
    }

	// turn on interrupts.
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

}
