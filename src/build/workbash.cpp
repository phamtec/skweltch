
#include "Ports.hpp"
#include "Interrupt.hpp"
#include "StringMsg.hpp"
#include "Poller.hpp"
#include "ExeRunner.hpp"
#include "MakeAnalyser.hpp"
#include "Logging.hpp"
#include "Work.hpp"
#include "IWorkWorker.hpp"

#include <zmq.hpp>
#include <czmq.h>
#include <zclock.h>
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <msgpack.hpp>

using namespace std;
using namespace boost;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.workbash"));

class WWorker : public IWorkWorker {
    
private:
    string cmd;
    string logfile;
    int sleeptime;
    zmq::i_socket_t *sender;
	int msgid;
    
public:
	WWorker(const string &c, const string &l, int sl, zmq::i_socket_t *s) : cmd(c), logfile(l), sleeptime(sl), sender(s), msgid(-1) {}
	
	virtual void processMsg(const zmq::message_t &message);
	virtual void timeout(Work *work);
	virtual int getTimeout() { return sleeptime; }
	
	virtual bool shouldQuit() {
		return s_interrupted;
	}
    
};

void WWorker::processMsg(const zmq::message_t &message) {
    
    // no messages for 1 second, go ahead and build if we received any.
    StringMsg msg(message);
    
    LOG4CXX_TRACE(logger,  "msg " << msg.getId());
    
    // we will need to build, we need to make sure we send a sink for EVERY message we ignore.
    msgid = msg.getId();

}

void WWorker::timeout(Work *work) {
    
    if (msgid >= 0) {
        
        // there was a timeout, give the worker a chance to send on anyway.
        
        // run the cmd.
        int pid = ExeRunner(logger).run(cmd);
        
        // and wait to die.
        ::waitpid(pid, NULL, 0);
        
        // analyse the log file.
        ifstream f(logfile.c_str());
        BuildStatus stat = MakeAnalyser(logger).analyse(&f);
        
        // and set the sink msg.
        StringMsg smsg(msgid, 0, (stat.workDone && stat.success) ? "success" : "failed");
        
        
        zmq::message_t message;
        
        // Send results to sink
        smsg.set(&message);
        try {
            sender->send(message);
        }
        catch (zmq::error_t &e) {
            if (string(e.what()) != "Interrupted system call") {
                LOG4CXX_ERROR(logger, "send failed." << e.what())
            }
        }
        
        msgid = -1;
    }
}


int main (int argc, char *argv[])
{
    setup_logging();
    
	if (argc != 4) {
        LOG4CXX_ERROR(logger, "usage: " << argv[0] << " pipes config name");
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

	// make sure we are rooted at the path.
	string dir = root.getString("dir");
	filesystem::current_path(dir);

	string cmd = root.getString("cmd");
	string logfile = root.getString("logfile");
    int sleeptime = root.getInt("sleeptime", 1000);
    
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
    WWorker w(cmd, logfile, sleeptime, &sender);
    Work v(logger, &p, &receiver);
 	v.process(&w);

    return 0;

}
