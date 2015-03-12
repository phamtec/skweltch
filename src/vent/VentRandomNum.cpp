
#include "Ports.hpp"
#include "Interrupt.hpp"
#include "Vent.hpp"
#include "IVentWorker.hpp"
#include "IntMsg.hpp"
#include "Logging.hpp"
#include "Main.hpp"

#include <iostream>
#include <fstream>
#include <zmq.hpp>
#include <czmq.h>
#include <zclock.h>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <msgpack.hpp>
#include <boost/program_options.hpp>

#define within(num) (int) ((float) num * random () / (RAND_MAX + 1.0))

using namespace std;
using namespace boost;
namespace po = boost::program_options;

class VWorker : public IVentWorker {

private:
	int count;
	int low;
	int high;
	int sleeptime;
	
public:
	VWorker(int c, int l, int h, int s) : count(c), low(l), high(h), sleeptime(s) {}
	
	virtual int sendAll(Vent *vent);
	
	virtual bool shouldQuit() {
		return s_interrupted;
	}
};

int VWorker::sendAll(Vent *vent) {

	int id = 0;
    for (int i = 0; i < count; i++) {
    
		//  Random number from 1 to the range specified
		int num = within(high) + low;

   		zmq::message_t message(2);
 		IntMsg msg(id++, clock(), num);
		msg.set(&message);
		
		if (!vent->sendOne(this, message, sleeptime, 1)) {
			return 0;
		}
    }
    
	return id-1;
}

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.ventrandomnum"));

int main (int argc, char *argv[])
{
    setup_logging();
    
	JsonObject pipes;
	JsonObject root;
    if (!setup_main(argc, argv, &pipes, &root, &logger)) {
        return 1;
    }
    
	int low = root.getInt("low", 1);
 	int high = root.getInt("high", 100);
	
	zmq::context_t context(1);
    zmq::socket_t sender(context, ZMQ_PUSH);
    zmq::socket_t control(context, ZMQ_SUB);
	zmq::socket_t sink(context, ZMQ_PUSH);
	
 	Ports ports(logger);
    if (!ports.join(&sender, pipes, "pushTo")) {
    	return 1;
    }
    if (!ports.join(&control, pipes, "control")) {
    	return 1;
    }
	control.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    if (!ports.join(&sink, pipes, "syncTo")) {
    	return 1;
    }

 	int count = root.getInt("count", 10);
 	int sleeptime = root.getInt("sleep", 0);

    //  Initialize random number generator
    srandom ((unsigned) time (NULL));

    //  Send count tasks
    s_catch_signals ();

	// and do the vent.
	Vent v(logger, &sink, &sender, &control);
	VWorker w(count, low, high, sleeptime);
	if (v.process(&w)) {
		return 0;
	}
	else {
		return 1;
	}

}
