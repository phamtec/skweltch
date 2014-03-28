
#include "Ports.hpp"
#include "Interrupt.hpp"
#include "IPollWorker.hpp"
#include "Poll.hpp"
#include "StringMsg.hpp"
#include "FileModChecker.hpp"
#include "Logging.hpp"
#include "Main.hpp"
#include "Git.hpp"

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

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.pollgit"));

class PWorker : public IPollWorker {
    
private:
	string folder;
	string repos;
	int sleeptime;
	
public:
	PWorker(const string &f, const string &r, int sl) : folder(f), repos(r), sleeptime(sl) {}
	
	virtual bool waitEvent();
	virtual int send(int msgid, Poll *poll);
	
	virtual bool shouldQuit() {
		return s_interrupted;
	}
	
};

bool PWorker::waitEvent() {
    
	while (!shouldQuit()) {
        
        if (Git(logger).poll(folder, repos)) {
            return true;
        }
        
		// sleep for a bit.
		zclock_sleep(sleeptime);

	}
	
	return false;
}

int PWorker::send(int msgid, Poll *poll) {
    
	zmq::message_t message(2);
	StringMsg msg(msgid, clock(), folder);
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

int main(int argc, char *argv[])
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
	string folder = root.getString("folder");
	string repos = root.getString("repos");
	int sleeptime = root.getInt("sleeptime", 1000);

    //  Send count tasks
    s_catch_signals ();
    
	// and do the vent.
	Poll p(logger, &sender);
	PWorker w(folder, repos, sleeptime);
	if (p.process(&w)) {
		return 0;
	}
	else {
		return 1;
	}

}
