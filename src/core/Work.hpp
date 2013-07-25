
#ifndef __WORK_HPP_INCLUDED__
#define __WORK_HPP_INCLUDED__

#include <zmq.hpp>
#include <string>
#include <log4cxx/logger.h>

class IWorkWorker;

namespace zmq {
	class i_socket_t;
}

class Work {

private:

	log4cxx::LoggerPtr logger;
    zmq::i_socket_t *receiver;
    zmq::i_socket_t *sender;
    
public:
	Work(log4cxx::LoggerPtr l, zmq::i_socket_t *r, zmq::i_socket_t *s) : logger(l), receiver(r) , sender(s) {}
	
	void process(IWorkWorker *worker);
	
};

#endif // __WORK_HPP_INCLUDED__

