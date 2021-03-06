
#include "Ports.hpp"
#include "Interrupt.hpp"
#include "IPollWorker.hpp"
#include "Poll.hpp"
#include "StringMsg.hpp"
#include "FileModChecker.hpp"
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

using namespace std;
using namespace boost;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.polldir"));

class PWorker : public IPollWorker {

private:
	string dir;
	long lastCrc;
	int sleeptime;
	
public:
	PWorker(const string &d, int crc, int sl) : dir(d), lastCrc(crc), sleeptime(sl) {}
	
	virtual bool waitEvent();
	virtual int send(int msgid, Poll *poll);
	
	virtual bool shouldQuit() {
		return s_interrupted;
	}
	
};

bool PWorker::waitEvent() {

	// what's the crc
	long crc = FileModChecker(logger).getCrc(dir);
	while (crc == lastCrc) {
	
		// sleep for a bit.
		zclock_sleep(sleeptime);	
		
		// and try for the crc again.
		crc = FileModChecker(logger).getCrc(dir);
	}
	lastCrc = crc;
	
	return true;
}

int PWorker::send(int msgid, Poll *poll) {

	zmq::message_t message(2);
	StringMsg msg(msgid, clock(), dir);
	msg.set(&message);
	
	poll->send(this, message, 0, 0);

	return msgid+1;
	
}

/**
 
 Example args:
 
 "{'pushTo':{'mode':'bind','address':'*','port':5557}}"
 
 "{'dir':'/Users/paul/Documents/test','sleeptime':500}"

 "test"
 
 Then test with:
 
 ./listen connect tcp://localhost:5557
 
*/
int main (int argc, char *argv[])
{
    setup_logging();
    
	JsonObject pipes;
	JsonObject root;
    if (!setup_main(argc, argv, &pipes, &root, &logger)) {
        return 1;
    }
    
	zmq::context_t context(1);
    zmq::socket_t sender(context, ZMQ_PUSH);

 	Ports ports(logger);
    if (!ports.join(&sender, pipes, "pushTo")) {
    	return 1;
    }

	// the directory to watch.
	string dir = root.getString("dir");
	int sleeptime = root.getInt("sleeptime", 1000);

	// get the initial crc (we wait until this changes).
	long crc = FileModChecker(logger).getCrc(dir);
    
    //  Send count tasks
    s_catch_signals ();
    
	// and do the vent.
	Poll p(logger, &sender);
	PWorker w(dir, crc, sleeptime);
	if (p.process(&w)) {
		return 0;
	}
	else {
		return 1;
	}
}
