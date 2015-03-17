
#include "Ports.hpp"
#include "WordSplitter.hpp"
#include "Interrupt.hpp"
#include "IVentWorker.hpp"
#include "Vent.hpp"
#include "StringMsg.hpp"
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

log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("org.skweltch.ventwords"));

class VWorker : public IVentWorker {

private:
	istream *input;
	
public:
	VWorker(istream *is) : input(is) {}
	
	virtual int sendAll(Vent *vent);
	
	virtual bool shouldQuit() {
		return s_interrupted;
	}
};


class SendWord : public IWord {

private:
	Vent *vent;
	IVentWorker *worker;
	int id;
	
public:
	SendWord(Vent *v, IVentWorker *w) : vent(v), worker(w), id(0) {}
	
	virtual bool word(const std::string &s);
	
	int getId() { 
		return id; 
	}
};

bool SendWord::word(const std::string &s) {

	zmq::message_t message(2);
	StringMsg msg(id++, clock(), s);
	msg.set(&message);
	LOG4CXX_TRACE(logger, "venting msg " << msg.getId())
	
	return vent->sendOne(worker, message, 0, 0);

}

int VWorker::sendAll(Vent *vent) {

	SendWord w(vent, this);
	WordSplitter splitter(input);
	splitter.process(&w);
	return w.getId()-1;
	
}

int main (int argc, char *argv[])
{
    setup_logging();
    
	JsonObject pipes;
	JsonObject root;
    if (!setup_main(argc, argv, &pipes, &root, &logger)) {
        return 1;
    }
    
	string filename = root.getString("filename");
	
	zmq::context_t context(1);
    zmq::socket_t sender(context, ZMQ_PUSH);
    zmq::socket_t control(context, ZMQ_SUB);
	zmq::socket_t sink(context, ZMQ_PUSH);

 	Ports ports(logger);
    if (!ports.join(&sender, pipes, "pushTo")) {
    	return 1;
    }
    if (!ports.join(&control, pipes, "control")) {
    	return 1;
    }
	control.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    if (!ports.join(&sink, pipes, "syncTo")) {
    	return 1;
    }

	ifstream f(filename.c_str(), ifstream::in);
	if (!f.is_open()) {
		LOG4CXX_ERROR(logger, " no file " << filename)
		return 1;
	}
	
    //  Send count tasks
    s_catch_signals ();

	// and do the vent.
	Vent v(logger, &sink, &sender, &control);
	VWorker w(&f);
	if (v.process(&w)) {
		return 0;
	}
	else {
		return 1;
	}

}
