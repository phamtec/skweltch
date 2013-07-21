
#include "JsonConfig.hpp"
#include "Ports.hpp"
#include <zmq.hpp>
#include <czmq.h>
#include <zclock.h>

#include <iostream>
#include <fstream>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <msgpack.hpp>

using namespace std;
using namespace boost;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.pipedump"));

int main (int argc, char *argv[])
{
	log4cxx::PropertyConfigurator::configure("log4cxx.conf");

 	if (argc != 4) {
		LOG4CXX_ERROR(logger, "usage: " << argv[0] << " pipes config name")
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
 		JsonConfig json(&ss);
		if (!json.read(&pipes)) {
			return 1;
		}
 	}
	JsonObject root;
 	{
 		stringstream ss(argv[2]);
 		JsonConfig json(&ss);
		if (!json.read(&root)) {
			return 1;
		}
 	}

 	int count = root.getInt("every", 100);

	zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    zmq::socket_t sender(context, ZMQ_PUSH);

	try {
		int linger = -1;
		receiver.setsockopt(ZMQ_LINGER, &linger, sizeof linger);
		sender.setsockopt(ZMQ_LINGER, &linger, sizeof linger);
 	}
	catch (zmq::error_t &e) {
		LOG4CXX_ERROR(logger, e.what())
		return 1;
	}  
  
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
        
        if ((n % count) == 0) {
			msgpack::unpacked msg;
			msgpack::unpack(&msg, (const char *)message.data(), message.size());
			msgpack::object obj = msg.get();
 			LOG4CXX_INFO(logger, "..." << obj << "...")
        }
        n++;
        
		sender.send(message);
		
     }
    
    return 0;

}
