
#include "Ports.hpp"
#include "Interrupt.hpp"

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

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.worksleep"));

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

	zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    zmq::socket_t sender(context, ZMQ_PUSH);
    
 	Ports ports(logger);
    if (!ports.join(&receiver, pipes, "pullFrom")) {
    	return 1;
    }
    if (!ports.join(&sender, pipes, "pushTo")) {
    	return 1;
    }

   	//  Process tasks forever
    int n=0;
    s_catch_signals ();
    while (!s_interrupted) {

       	zmq::message_t message;
 		try {
       		receiver.recv(&message);
		}
		catch (zmq::error_t &e) {
			if (string(e.what()) != "Interrupted system call") {
				LOG4CXX_ERROR(logger, "recv failed." << e.what())
			}
		}
		if (s_interrupted) {
			LOG4CXX_INFO(logger,  "interrupt received, killing server")
			break;
		}

        msgpack::unpacked msg;
        msgpack::unpack(&msg, (const char *)message.data(), message.size());
        msgpack::object obj = msg.get();
        
        if ((n % 10) == 0) {
			LOG4CXX_DEBUG(logger,  "..." << obj << "...")
        }
        n++;
        
 		msgpack::type::tuple<int, int, int> wmsg;
      	obj.convert(&wmsg);
  
		LOG4CXX_DEBUG(logger,  "batch " << wmsg.a0 << " msg " << wmsg.a1)

		//  Do the work
 		zclock_sleep(wmsg.a2);

       //  Send results to sink (just an empty message).
		msgpack::sbuffer sbuf;
		pair<int, int> resultsmsg(2, 0);
		msgpack::pack(sbuf, resultsmsg);
		message.rebuild(sbuf.size());
		memcpy(message.data(), sbuf.data(), sbuf.size());
		try {
 			sender.send(message);
 		}
		catch (zmq::error_t &e) {
			LOG4CXX_ERROR(logger,  "send failed." << e.what())
		}
		if (s_interrupted) {
			LOG4CXX_INFO(logger,  "interrupt sender, killing server")
			break;
		}

   	}
    
	LOG4CXX_INFO(logger, "finished.")

    return 0;

}
