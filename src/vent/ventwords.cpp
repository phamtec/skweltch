
#include "Ports.hpp"
#include "WordSplitter.hpp"
#include "Interrupt.hpp"
#include "IVentWorker.hpp"
#include "Vent.hpp"
#include "StringMsg.hpp"

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
	int sleeptime;
	int sleepevery;
	
public:
	VWorker(istream *is, int slp, int ev) : input(is), sleeptime(slp), sleepevery(ev) {}
	
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
	int sleeptime;
	int sleepevery;
	
public:
	SendWord(Vent *v, IVentWorker *w, int slp, int ev) : vent(v), worker(w), id(0), sleeptime(slp), sleepevery(ev) {}
	
	virtual bool word(const std::string &s);
	
	int getId() { 
		return id; 
	}
};

bool SendWord::word(const std::string &s) {

	zmq::message_t message(2);
	StringMsg msg(id++, s);
	msg.set(&message);
	
	return vent->sendOne(worker, message, sleeptime, sleepevery);

}

int VWorker::sendAll(Vent *vent) {

	SendWord w(vent, this, sleeptime, sleepevery);
	WordSplitter splitter(input);
	splitter.process(&w);
	return w.getId()-1;
	
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

	string filename = root.getString("filename");
	
 	int sleeptime = root.getInt("sleep", 0);
	int sleepevery = root.getInt("every", 0);
	
	LOG4CXX_DEBUG(logger, "sleeping " << sleeptime << " every " << sleepevery)
	
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

	ifstream f(filename.c_str(), ifstream::in);
	if (!f.is_open()) {
		LOG4CXX_ERROR(logger, " no file " << filename)
		return 1;
	}
	
    //  Send count tasks
    s_catch_signals ();

	// and do the vent.
	Vent v(logger, &sink, &sender);
	VWorker w(&f, sleeptime, sleepevery);
	if (v.process(&w)) {
		return 0;
	}
	else {
		return 1;
	}

}
