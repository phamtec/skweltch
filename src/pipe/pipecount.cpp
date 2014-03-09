
#include "Ports.hpp"
#include "Logging.hpp"

#include <zmq.hpp>
#include <czmq.h>
#include <zclock.h>

#include <iostream>
#include <fstream>
#include <log4cxx/logger.h>
#include <log4cxx/helpers/exception.h>
#include <msgpack.hpp>

using namespace std;
using namespace boost;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.pipecount"));

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

 	int count = root.getInt("every", 100);

	zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    zmq::socket_t sender(context, ZMQ_PUSH);

 	Ports ports(logger);
    if (!ports.join(&receiver, pipes, "pipeFrom")) {
    	return 1;
    }
    if (!ports.join(&sender, pipes, "pipeTo")) {
    	return 1;
    }

	LOG4CXX_INFO(logger, "start...")
	
    //  Process messages forever
    int n=0;
    while (1) {

        zmq::message_t message;
        receiver.recv(&message);
        
        if (n == 0) {
 			LOG4CXX_INFO(logger, "first message...")
        }
        
		sender.send(message);
		
        if ((n % count) == 0) {
 			LOG4CXX_INFO(logger,n)
        }
        n++;
        
     }
    
    return 0;

}
