
#include "Vent.hpp"

#include "IVentWorker.hpp"
#include "SinkMsg.hpp"

#include <zmq.hpp>
#include <msgpack.hpp>
#include <czmq.h>
#include <zclock.h>

using namespace std;

bool Vent::process(IVentWorker *worker) {

	// let the sink know the first message.
 	{
 		SinkMsg msg;
 		msg.firstMsg(0);
		zmq::message_t message(2);
 		msg.set(&message);
		try {
			sink->send(message);
		}
		catch (zmq::error_t &e) {
			LOG4CXX_ERROR(logger, "sink send failed." << e.what())
			return false;
		}
	}
	
	// send them all.
	int last = worker->sendAll(this);
    
 	// let the sink know the last message was just sent
 	{
 		SinkMsg msg;
 		msg.lastMsg(last);
		zmq::message_t message(2);
 		msg.set(&message);
		try {
			sink->send(message);
		}
		catch (zmq::error_t &e) {
			LOG4CXX_ERROR(logger, "sink send failed." << e.what())
			return false;
		}
	}
	   
	LOG4CXX_INFO(logger, "vent finished (" << last << ")")
	return true;
	
}

bool Vent::sendOne(IVentWorker *worker, const zmq::message_t &message, int sleeptime, int sleepevery) {

	// send the message buffer.
	try {
		sender->send(const_cast<zmq::message_t &>(message));
	}
	catch (zmq::error_t &e) {
		LOG4CXX_ERROR(logger, "send failed." << e.what())
	}
	
	if (worker->shouldQuit()) {
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


