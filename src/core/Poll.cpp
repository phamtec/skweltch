
#include "Poll.hpp"

#include "IPollWorker.hpp"
#include "SinkMsg.hpp"

#include <zmq.hpp>
#include <msgpack.hpp>
#include <czmq.h>
#include <zclock.h>

using namespace std;

bool Poll::process(IPollWorker *worker) {

	int msgNum = 0;
	
	LOG4CXX_INFO(logger, "starting...")

	while (worker->waitEvent()) {

		// send them all.
		msgNum = worker->send(msgNum, this);

		if (worker->shouldQuit()) {
			LOG4CXX_INFO(logger, "interrupt received, finishing")
			// quitting breaks.
			return false;
		}

	}
	
	LOG4CXX_INFO(logger, "finished (" << msgNum << ")")
	return true;
	
}

bool Poll::send(IPollWorker *worker, const zmq::message_t &message, int sleeptime, int sleepevery) {

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


