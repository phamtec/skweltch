
#ifndef __VENT_HPP_INCLUDED__
#define __VENT_HPP_INCLUDED__

#include <zmq.hpp>
#include <msgpack.hpp>
#include <string>
#include <log4cxx/logger.h>

class IVentWorker;

namespace zmq {
	class i_socket_t;
}

/*
	A Vent.
	
*/

class Vent {

private:

	log4cxx::LoggerPtr logger;
    zmq::i_socket_t *sink;
    zmq::i_socket_t *sender;
    
public:
	Vent(log4cxx::LoggerPtr l, zmq::i_socket_t *sk, zmq::i_socket_t *sn) : logger(l), sink(sk), sender(sn) {}
	
	bool process(IVentWorker *worker);
	bool sendOne(IVentWorker *worker, const msgpack::sbuffer &sbuf, int sleeptime);
	
};

#endif // __VENT_HPP_INCLUDED__

