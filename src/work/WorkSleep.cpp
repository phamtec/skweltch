
#include "Ports.hpp"
#include "Interrupt.hpp"
#include "Work.hpp"
#include "IWorkWorker.hpp"
#include "IntMsg.hpp"
#include "SinkMsg.hpp"

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

class WWorker : public IWorkWorker {

public:
	virtual void process(const zmq::message_t &message, SinkMsg *smsg);
	
	virtual bool shouldQuit() {
		return s_interrupted;
	}

};

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.worksleep"));

void WWorker::process(const zmq::message_t &message, SinkMsg *smsg) {

	IntMsg msg(message);

	LOG4CXX_TRACE(logger,  "msg " << msg.getId());

	//  Do the work
	zclock_sleep(msg.getPayload());
	
	// and set the sink msg.
	smsg->dataMsg(msg.getId(), vector<string>());
	
}

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

    s_catch_signals ();

	// and do the work.
	Work v(logger, &receiver, &sender);
	WWorker w;
	v.process(&w);

    return 0;

}
