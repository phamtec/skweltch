
#include "Ports.hpp"
#include "XPathSplitter.hpp"
#include "Interrupt.hpp"
#include "IVentWorker.hpp"
#include "Vent.hpp"
#include "StringMsg.hpp"

#include <iostream>
#include <fstream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>
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
	string xpath;
	int sleeptime;
	int sleepevery;
	
public:
	VWorker(istream *is, const string &xp, int slp, int ev) : input(is), xpath(xp), sleeptime(slp), sleepevery(ev) {}
	
	virtual int sendAll(Vent *vent);
	
	virtual bool shouldQuit() {
		return s_interrupted;
	}
};


class SendChunk : public IChunk {

private:
	Vent *vent;
	IVentWorker *worker;
	int id;
	int sleeptime;
	int sleepevery;
	
public:
	SendChunk(Vent *v, IVentWorker *w, int slp, int ev) : vent(v), worker(w), id(0), sleeptime(slp), sleepevery(ev) {}
	
	virtual bool chunk(const std::string &s);
	
	int getId() { 
		return id; 
	}
};

bool SendChunk::chunk(const std::string &s) {

	zmq::message_t message(2);
	StringMsg msg(id++, s);
	msg.set(&message);
	
	LOG4CXX_TRACE(logger, "sending chunk.")

	return vent->sendOne(worker, message, sleeptime, sleepevery);

}

int VWorker::sendAll(Vent *vent) {

	LOG4CXX_TRACE(logger, "parsing xpath "<< xpath << ".")

	SendChunk w(vent, this, sleeptime, sleepevery);
	XPathSplitter splitter(logger, input);
	if (!splitter.process(xpath, &w)) {
		return 0;
	}
	
	return w.getId()-1;
	
}

int doWork(JsonObject *pipes, JsonObject *root, istream *f);

int main(int argc, char *argv[])
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
	
	if (filename.find(".gz") > 0) {
		ifstream fs(filename.c_str(), ios_base::in | ios_base::binary);    
		if (!fs.is_open()) {
			LOG4CXX_ERROR(logger, " no file " << filename)
			return 1;
		}
	
		iostreams::filtering_streambuf<iostreams::input> in;
		in.push(iostreams::gzip_decompressor());
		in.push(fs);
		istream f(&in);
	
		return doWork(&pipes, &root, &f);

	}
	else {
		ifstream f(filename.c_str(), ifstream::in);
		if (!f.is_open()) {
			LOG4CXX_ERROR(logger, " no file " << filename)
			return 1;
		}
	
		return doWork(&pipes, &root, &f);
	}

}

int doWork(JsonObject *pipes, JsonObject *root, istream *f) {

	zmq::context_t context(1);
    zmq::socket_t sender(context, ZMQ_PUSH);
	zmq::socket_t sink(context, ZMQ_PUSH);

 	Ports ports(logger);
    if (!ports.join(&sender, *pipes, "pushTo")) {
    	return 1;
    }
    if (!ports.join(&sink, *pipes, "syncTo")) {
    	return 1;
    }

 	int sleeptime = root->getInt("sleep", 0);
	int sleepevery = root->getInt("every", 0);
	string xpath = root->getString("xpath");
	
	LOG4CXX_DEBUG(logger, "sleeping " << sleeptime << " every " << sleepevery)
	
	//  Send count tasks
	s_catch_signals ();

	// and do the vent.
	Vent v(logger, &sink, &sender);
	VWorker w(f, xpath, sleeptime, sleepevery);
	if (v.process(&w)) {
		return 0;
	}
	else {
		return 1;
	}

}
