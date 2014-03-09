
#ifndef __POLL_HPP_INCLUDED__
#define __POLL_HPP_INCLUDED__

#include <zmq.hpp>
#include <string>
#include <log4cxx/logger.h>

class IPollWorker;

namespace zmq {
	class i_socket_t;
}

/*
	A Poll.
	
*/

class Poll {

private:

	log4cxx::LoggerPtr logger;
    zmq::i_socket_t *sender;
    int count;
    
public:
	Poll(log4cxx::LoggerPtr l, zmq::i_socket_t *sn) : logger(l), sender(sn), count(0) {}
	
	bool process(IPollWorker *worker);
	
	// used by the worker to actually send a message.
	bool send(IPollWorker *worker, const zmq::message_t &message, int sleeptime, int sleepevery);
	
};

#endif // __POLL_HPP_INCLUDED__

