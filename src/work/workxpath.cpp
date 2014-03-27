
#include "Ports.hpp"
#include "Interrupt.hpp"
#include "Work.hpp"
#include "IWorkWorker.hpp"
#include "StringMsg.hpp"
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
#include <pugixml.hpp>

using namespace std;
using namespace boost;

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.workxpath"));

class WWorker : public IWorkWorker {

private:
	string xpath;
    zmq::i_socket_t *sender;
	
public:
	WWorker(const string &xp, zmq::i_socket_t *s) : xpath(xp), sender(s) {}
	
	virtual void processMsg(const zmq::message_t &message);
	
	virtual bool shouldQuit() {
		return s_interrupted;
	}

};

void WWorker::processMsg(const zmq::message_t &message) {
    
    SinkMsg smsg;
	StringMsg msg(message);

	LOG4CXX_TRACE(logger,  "msg " << msg.getId());

	vector<string> v;
	string xml = msg.getPayload();
	pugi::xml_document doc;
	doc.load_buffer(xml.c_str(), xml.length());
	pugi::xpath_node_set nodes = doc.select_nodes(xpath.c_str());
	for (pugi::xpath_node_set::const_iterator i=nodes.begin(); i != nodes.end(); i++) {
		string result;
		if (i->node().empty()) {
			v.push_back(i->attribute().value());
		}
		else {
			v.push_back(i->node().value());
		}
	}
	
	// just send it on.
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

	string xpath = root.getString("xpath");

    s_catch_signals ();

	// and do the work.
	Poller p(logger);
	Work v(logger, &p, &receiver);
	WWorker w(xpath, &sender);
	v.process(&w);

    return 0;

}
