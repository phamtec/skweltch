
#include "Ports.hpp"
#include "Interrupt.hpp"
#include "IPollWorker.hpp"
#include "Poll.hpp"
#include "StringMsg.hpp"
#include "FileModChecker.hpp"

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
	int lastCrc;
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
		
	return msgid++;
	
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
		
	// get the initial crc (we wait until this changes). Might as well do this straight away.
	long crc = FileModChecker(logger).getCrc(".");

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
    zmq::socket_t sender(context, ZMQ_PUSH);
	zmq::socket_t sink(context, ZMQ_PUSH);

 	Ports ports(logger);
    if (!ports.join(&sender, pipes, "pushTo")) {
    	return 1;
    }
    if (!ports.join(&sink, pipes, "syncTo")) {
    	return 1;
    }

	// the directory to watch.
	string dir = root.getString("dir");
	int sleeptime = root.getInt("sleeptime", 1000);

    //  Send count tasks
    s_catch_signals ();

	// and do the vent.
	Poll p(logger, &sink, &sender);
	PWorker w(dir, crc, sleeptime);
	if (p.process(&w)) {
		return 0;
	}
	else {
		return 1;
	}
}
