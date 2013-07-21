
#include "JsonConfig.hpp"
#include "Ports.hpp"
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

static int s_interrupted = 0;
static void s_signal_handler (int signal_value)
{
    s_interrupted = 1;
}

static void s_catch_signals (void)
{
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
}

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

	int low = root.getInt("low", 1);
 	int high = root.getInt("high", 100);
	int expect = root.getInt("expect", -1);
	
	zmq::context_t context(1);
    zmq::socket_t sender(context, ZMQ_PUSH);
	zmq::socket_t sink(context, ZMQ_PUSH);

	try {
		int linger = -1;
		sender.setsockopt(ZMQ_LINGER, &linger, sizeof linger);
		sink.setsockopt(ZMQ_LINGER, &linger, sizeof linger);
 	}
	catch (zmq::error_t &e) {
		LOG4CXX_ERROR(logger, e.what())
		return 1;
	}  
	
 	Ports ports(logger);
    if (!ports.join(&sender, pipes, "pushTo")) {
    	return 1;
    }
    if (!ports.join(&sink, pipes, "syncTo")) {
    	return 1;
    }

 	// pack the number up and send it.
    zmq::message_t message(2);
	msgpack::sbuffer sbuf;
	pair<int, int> expectmsg(1, expect);
	msgpack::pack(sbuf, expectmsg);
	message.rebuild(sbuf.size());
	memcpy(message.data(), sbuf.data(), sbuf.size());
    sink.send(message);

 	int count = root.getInt("count", 10);
 	int sleeptime = root.getInt("sleep", 0);

    //  Initialize random number generator
    srandom ((unsigned) time (NULL));

    //  Send count tasks
    s_catch_signals ();
    for (int i = 0; i < count; i++) {
    
    	zmq::message_t message(2);
    	
		//  Random number from 1 to the range specified
		int num = within(high) + low;

		// pack the number up and send it.
		msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, num);
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
    
	LOG4CXX_INFO(logger, "finished.")
    
    return 0;
}
