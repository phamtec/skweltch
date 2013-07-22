
#include "Vent.hpp"

#include "IVentWorker.hpp"

#include <zmq.hpp>
#include <msgpack.hpp>
#include <czmq.h>
#include <zclock.h>

using namespace std;

bool Vent::process(IVentWorker *worker) {

	// let the sink no the first message.
 	{
		zmq::message_t message(2);
		msgpack::sbuffer sbuf;
		pair<int, int> firstmsg(1, 0);
		msgpack::pack(sbuf, firstmsg);
		message.rebuild(sbuf.size());
		memcpy(message.data(), sbuf.data(), sbuf.size());
		try {
			sink->send(message);
		}
		catch (zmq::error_t &e) {
			LOG4CXX_ERROR(logger, "sink send failed." << e.what())
			return false;
		}
	}
	
	LOG4CXX_INFO(logger, "starting... ")

	// send them all.
	int last = worker->sendAll(this);
    
 	// let the sink know the last message was just sent
 	{
		zmq::message_t message(2);
		msgpack::sbuffer sbuf;
		pair<int, int> lastmsg(3, last);
		msgpack::pack(sbuf, lastmsg);
		message.rebuild(sbuf.size());
		memcpy(message.data(), sbuf.data(), sbuf.size());
		try {
			sink->send(message);
		}
		catch (zmq::error_t &e) {
			LOG4CXX_ERROR(logger, "sink send failed." << e.what())
			return false;
		}
	}
	   
	LOG4CXX_INFO(logger, "finished (" << last << ")")
	return true;
	
}

bool Vent::sendOne(IVentWorker *worker, const msgpack::sbuffer &sbuf, int sleeptime) {

	// send the message buffer.
 	zmq::message_t message(2);
	message.rebuild(sbuf.size());
	memcpy(message.data(), sbuf.data(), sbuf.size());

	try {
		sender->send(message);
	}
	catch (zmq::error_t &e) {
		LOG4CXX_ERROR(logger, "send failed." << e.what())
	}
	
	if (worker->shouldQuit()) {
		LOG4CXX_INFO(logger, "interrupt received, killing server")
		return false;
	}

	if (sleeptime > 0) {
		//  Do the work
		zclock_sleep(sleeptime);
	}

	return true;
}


