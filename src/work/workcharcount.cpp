
#include "Ports.hpp"
#include "Interrupt.hpp"
#include "Work.hpp"
#include "IWorkWorker.hpp"
#include "StringMsg.hpp"
#include "SinkMsg.hpp"
#include "Logging.hpp"
#include "Poller.hpp"
#include "Main.hpp"

#include <zmq.hpp>
#include <czmq.h>
#include <zclock.h>
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <msgpack.hpp>

using namespace std;
using namespace boost;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.workcharcount"));

class WWorker : public SinkWorker {

public:
    WWorker(log4cxx::LoggerPtr l, zmq::i_socket_t *s) : SinkWorker(l, s) {}
    
	virtual void processMsg(const zmq::message_t &message);
	
	virtual bool shouldQuit() {
		return s_interrupted;
	}

};

void WWorker::processMsg(const zmq::message_t &message) {

    SinkMsg smsg;
	StringMsg msg(message);

	LOG4CXX_TRACE(logger,  "working on msg: " << msg.getId());

	// the work.
	int length = msg.getPayload().length();

	// and set the sink msg.
	vector<string> v;
	v.push_back(lexical_cast<string>(length));
	smsg.dataMsg(msg.getId(), v);

	LOG4CXX_TRACE(logger,  "sending sink msg: " << smsg.getId())
    
	sendSinkMsg(&smsg);

}

int main(int argc, char *argv[])
{
    setup_logging();
    
	JsonObject pipes;
	JsonObject root;
    if (!setup_main(argc, argv, &pipes, &root, &logger)) {
        return 1;
    }
    
	zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    zmq::socket_t control(context, ZMQ_SUB);
    zmq::socket_t sender(context, ZMQ_PUSH);
    
 	Ports ports(logger);
    if (!ports.join(&receiver, pipes, "pullFrom")) {
    	return 1;
    }
    if (!ports.join(&control, pipes, "control")) {
    	return 1;
    }
	control.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    if (!ports.join(&sender, pipes, "pushTo")) {
    	return 1;
    }

    s_catch_signals ();

	// and do the work.
	Poller p(logger);
	Work v(logger, &p, &receiver, &control);
	WWorker w(logger, &sender);
	v.process(&w);

    return 0;

}
