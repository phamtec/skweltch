
#include "Ports.hpp"
#include "Interrupt.hpp"
#include "Work.hpp"
#include "IWorkWorker.hpp"
#include "DataMsg.hpp"
#include "SinkMsg.hpp"
#include "Logging.hpp"
#include "Poller.hpp"

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

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.workcharfreq"));

class WWorker : public IWorkWorker {

private:
    zmq::i_socket_t *sender;
    
public:
    WWorker(zmq::i_socket_t *s) : sender(s) {}
    
	virtual void processMsg(const zmq::message_t &message);
	
	virtual bool shouldQuit() {
		return s_interrupted;
	}

};

void WWorker::processMsg(const zmq::message_t &message) {

    SinkMsg smsg;
	DataMsg<std::string> msg(message);

	LOG4CXX_TRACE(logger,  "msg " << msg.getId());

	string word = msg.getPayload();
	
	// the work.
	string result;
	for (int i=0; i<26; i++) {
		result += "0";
	}
	for (string::const_iterator i=word.begin(); i != word.end(); i++) {
		int index = *i - 'a';
		if (index < 0 || index > 25) {
			throw runtime_error("letter not in range.");
		}
		char ss[2];
		ss[0] = result[index];
		ss[1] = 0;
		int c;
		sscanf(ss, "%x", &c);		
		c++;
		if (c > 16) {
			throw runtime_error("ony handle up to 16 of each letter.");
		}
		sprintf(ss, "%x", c);
		result[index] = ss[0];
	}

	LOG4CXX_TRACE(logger,  "result " << result);

	// and set the sink msg.
	vector<string> v;
	v.push_back(result);
	smsg.dataMsg(msg.getId(), v);
	
	zmq::message_t smessage;
    
    // Send results to sink
	smsg.set(&smessage);
	try {
		sender->send(smessage);
	}
	catch (zmq::error_t &e) {
		if (string(e.what()) != "Interrupted system call") {
			LOG4CXX_ERROR(logger, "send failed." << e.what())
		}
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
	WWorker w(&sender);
	v.process(&w);

    return 0;

}
