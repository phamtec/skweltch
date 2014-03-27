
#include "Ports.hpp"
#include "Interrupt.hpp"
#include "Work.hpp"
#include "IWorkWorker.hpp"
#include "IntMsg.hpp"
#include "SinkMsg.hpp"
#include "Logging.hpp"
#include "Poller.hpp"
#include "Main.hpp"

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

class WWorker : public SinkWorker {

public:
    WWorker(log4cxx::LoggerPtr l, zmq::i_socket_t *s) : SinkWorker(l, s) {}
    
	virtual void processMsg(const zmq::message_t &message);
	
	virtual bool shouldQuit() {
		return s_interrupted;
	}

};

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.worksleep"));

void WWorker::processMsg(const zmq::message_t &message) {
    
    SinkMsg smsg;
	IntMsg msg(message);

	LOG4CXX_TRACE(logger,  "msg " << msg.getId());

	//  Do the work
	zclock_sleep(msg.getPayload());
	
	// and set the sink msg.
	smsg.dataMsg(msg.getId(), vector<string>());
	
	sendSinkMsg(&smsg);
    
}

int main (int argc, char *argv[])
{
    setup_logging();
    
	JsonObject pipes;
	JsonObject root;
    if (!setup_main(argc, argv, &pipes, &root, &logger)) {
        return 1;
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
	Poller p(logger);
	Work v(logger, &p, &receiver);
	WWorker w(logger, &sender);
	v.process(&w);

    return 0;

}
