
#include "Ports.hpp"
#include "WordSplitter.hpp"
#include "Interrupt.hpp"

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

class SendWord : public IWord {

private:
	zmq::socket_t *sender;
	int batch;
	int sleeptime;
	int sleepevery;
	int count;
	
public:
	SendWord(zmq::socket_t *s, int b, int slp, int ev) : sender(s), batch(b), sleeptime(slp), sleepevery(ev), count(0) {}
	
	virtual bool word(const std::string &s);
	int getCount() { return count; }
};

bool SendWord::word(const std::string &s) {

 	zmq::message_t message(2);
	
	// build the complete message.
	msgpack::type::tuple<int, int, string> wmsg(batch, count, s);
		
	// pack the number up and send it.
	msgpack::sbuffer sbuf;
	msgpack::pack(sbuf, wmsg);
	message.rebuild(sbuf.size());
	memcpy(message.data(), sbuf.data(), sbuf.size());
 
	try {
		sender->send(message);
	}
	catch (zmq::error_t &e) {
		LOG4CXX_ERROR(logger, "send failed." << e.what())
	}
	
	if (s_interrupted) {
		LOG4CXX_INFO(logger, "interrupt received, killing server")
		return false;
	}
	
	count++;
	
	if (sleeptime > 0) {
		if ((count % sleepevery) == 0) {
			zclock_sleep(sleeptime);	
		}
	}

	return true;
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
	int expect = root.getInt("expect", -1);
	
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
	
    s_catch_signals ();

 	// let the sink no the first message.
 	{
		zmq::message_t message(2);
		msgpack::sbuffer sbuf;
		pair<int, int> expectmsg(1, expect);
		msgpack::pack(sbuf, expectmsg);
//		pair<int, int> firstmsg(1, 0);
//		msgpack::pack(sbuf, firstmsg);
		message.rebuild(sbuf.size());
		memcpy(message.data(), sbuf.data(), sbuf.size());
		try {
			sink.send(message);
		}
		catch (zmq::error_t &e) {
			LOG4CXX_ERROR(logger, "sink send failed." << e.what())
			return 1;
		}
	}
	
	LOG4CXX_INFO(logger, "starting... ")

	SendWord w(&sender, 0, sleeptime, sleepevery);
	WordSplitter splitter(&f);
	splitter.process(&w);
    
 	// let the sink no the last message was just sent
 /*	{
		zmq::message_t message(2);
		msgpack::sbuffer sbuf;
		pair<int, int> lastmsg(3, w.getCount());
		msgpack::pack(sbuf, lastmsg);
		message.rebuild(sbuf.size());
		memcpy(message.data(), sbuf.data(), sbuf.size());
		try {
			sink.send(message);
		}
		catch (zmq::error_t &e) {
			LOG4CXX_ERROR(logger, "sink send failed." << e.what())
			return 1;
		}
	}*/
	   
	LOG4CXX_INFO(logger, "finished (" << w.getCount() << ")")
    
    return 0;
}
