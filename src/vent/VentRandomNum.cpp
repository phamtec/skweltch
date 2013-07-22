
#include "Ports.hpp"
#include "Interrupt.hpp"

#include <iostream>
#include <fstream>
#include <zmq.hpp>
#include <czmq.h>
#include <zclock.h>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <msgpack.hpp>

#define within(num) (int) ((float) num * random () / (RAND_MAX + 1.0))

using namespace std;
using namespace boost;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.ventrandomnum"));

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

	int low = root.getInt("low", 1);
 	int high = root.getInt("high", 100);
	int expect = root.getInt("expect", -1);
	
	zmq::context_t context(1);
    zmq::socket_t sender(context, ZMQ_PUSH);
	zmq::socket_t sink(context, ZMQ_PUSH);
	
 	Ports ports(logger);
    if (!ports.join(&sender, pipes, "pushTo")) {
    	return 1;
    }
    if (!ports.join(&sink, pipes, "syncTo")) {
    	return 1;
    }

 	int count = root.getInt("count", 10);
 	int sleeptime = root.getInt("sleep", 0);

    //  Initialize random number generator
    srandom ((unsigned) time (NULL));

    //  Send count tasks
    s_catch_signals ();
    int batch = 0;
    int msg = 0;
    
 	// let the sink no the first message.
 	{
		zmq::message_t message(2);
		msgpack::sbuffer sbuf;
//		pair<int, int> firstmsg(1, msg);
//		msgpack::pack(sbuf, firstmsg);
		pair<int, int> expectmsg(1, expect);
		msgpack::pack(sbuf, expectmsg);
		message.rebuild(sbuf.size());
		memcpy(message.data(), sbuf.data(), sbuf.size());
		sink.send(message);
	}
	   
    for (int i = 0; i < count; i++) {
    
    	zmq::message_t message(2);
    	
		//  Random number from 1 to the range specified
		int num = within(high) + low;

		// build the complete message.
		msgpack::type::tuple<int, int, int> wmsg(batch, msg++, num);
		
		// pack the number up and send it.
		msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, wmsg);
 		message.rebuild(sbuf.size());
		memcpy(message.data(), sbuf.data(), sbuf.size());
 
 		try {
 			sender.send(message);
 		}
		catch (zmq::error_t &e) {
			LOG4CXX_ERROR(logger, "send failed." << e.what())
		}
    	
 		if (s_interrupted) {
			LOG4CXX_INFO(logger, "interrupt received, killing server")
			break;
		}
	
    	if (sleeptime > 0) {
			//  Do the work
			zclock_sleep(sleeptime);
     	}
     	
    }
    
 	// let the sink no the last message was just sent
/* 	{
		zmq::message_t message(2);
		msgpack::sbuffer sbuf;
		pair<int, int> lastmsg(3, count);
		msgpack::pack(sbuf, lastmsg);
		message.rebuild(sbuf.size());
		memcpy(message.data(), sbuf.data(), sbuf.size());
		sink.send(message);
	}*/
	   
	LOG4CXX_INFO(logger, "finished.")
    
    return 0;
}
