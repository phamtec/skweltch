#include "Ports.hpp"
#include "Elapsed.hpp"
#include "Interrupt.hpp"
#include "Sink.hpp"
#include "ISinkWorker.hpp"
#include "Logging.hpp"
#include "Main.hpp"
#include "StringMsg.hpp"
#include "Poller.hpp"

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

class SWorker : public ISinkWorker {

private:
	int count;
	int total;
    zmq::socket_t *result;
	
public:
	SWorker(zmq::socket_t *r) : result(r), count(0), total(0) { }
	
	virtual void first(int id) {}
	virtual void last(int id) {}
	
	virtual void process(int id, std::vector<std::string> *data);
	virtual void results(int total_ms);
	
	virtual bool shouldQuit() {
		return s_interrupted;
	}

};

void SWorker::process(int id, std::vector<std::string> *data) {

	total += lexical_cast<int>((*data)[0]);
	count++;

}

void SWorker::results(int total_ms) {

	JsonObject r;
	r.add("elapsed", total_ms);
	r.add("average", ((double)total / (double)count));
	
    stringstream ss;
    r.write(true, &ss);

    StringMsg msg(0, 0, ss.str());
   	zmq::message_t message(2);
    msg.set(&message);
    result->send(message);

}

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.sinkavg"));

int main (int argc, char *argv[])
{
    setup_logging();
    
	JsonObject pipes;
	JsonObject root;
    if (!setup_main(argc, argv, &pipes, &root, &logger)) {
        return 1;
    }
    
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    zmq::socket_t result(context, ZMQ_PUSH);
    
 	Ports ports(logger);
    if (!ports.join(&receiver, pipes, "pullFrom")) {
    	return 1;
    }
    if (!ports.join(&result, pipes, "resultsTo")) {
    	return 1;
    }

	// turn on interrupts.
    s_catch_signals ();

	// and do the sink.
    Poller p(logger);
	Sink s(logger, &p, &receiver);
	SWorker w(&result);
	if (s.process(&w)) {
		return 0;
	}
	else {
		return 1;
	}

}
