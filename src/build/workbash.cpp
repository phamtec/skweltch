
#include "Ports.hpp"
#include "Interrupt.hpp"
#include "Work.hpp"
#include "IWorkWorker.hpp"
#include "StringMsg.hpp"
#include "SinkMsg.hpp"
#include "ExeRunner.hpp"
#include "BoostAnalyser.hpp"
#include "Logging.hpp"

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

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.workcharcount"));

class WWorker : public IWorkWorker {

private:
	string cmd;
	string logfile;
	vector<int> msgids;
	
public:
	WWorker(const string &c, const string &l) : cmd(c), logfile(l) {}
	
	virtual bool process(const zmq::message_t &message, SinkMsg *smsg);
	virtual int getTimeout() { return 1000; }
	virtual void timeout(Work *work);
	
	virtual bool shouldQuit() {
		return s_interrupted;
	}

};

bool WWorker::process(const zmq::message_t &message, SinkMsg *smsg) {

	// no messages for 1 second, go ahead and build if we received any.
	StringMsg msg(message);

	LOG4CXX_TRACE(logger,  "msg " << msg.getId());

	// we will need to build, we need to make sure we send a sink for EVERY message we ignore.
	msgids.push_back(msg.getId());
	
	// don't send any of the sink commands.
	return false;
}

void WWorker::timeout(Work *work) {

	if (msgids.size() > 0) {
	
		// run the cmd.
		int pid = ExeRunner(logger).run(cmd);
	
		// and wait to die.
		::waitpid(pid, NULL, 0);	
	
		// analyse the log file.
//		ifstream f(logfile.c_str());
//		BuildStatus stat = BoostAnalyser(logger).analyse(&f);
	
		// send an ignore for all but the last message.
		for (size_t i=0; i<msgids.size()-1; i++) {
			SinkMsg smsg;
			vector<string> v;
			v.push_back("working");
			smsg.dataMsg(i, v);
			work->sendSink(smsg);
		}
		
		// and set the sink msg.
		SinkMsg smsg;
		vector<string> v;
//		v.push_back((stat.success || !stat.workDone) ? "success": "fail");
		v.push_back("done");
		smsg.dataMsg(msgids[msgids.size()-1], v);
		work->sendSink(smsg);
		
		// done.
		msgids.clear();
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
	Work v(logger, &p, &receiver, &sender);
	WWorker w(cmd, logfile);
	v.process(&w);

    return 0;

}
