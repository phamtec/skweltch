
#ifndef __WORK_HPP_INCLUDED__
#define __WORK_HPP_INCLUDED__

#include <zmq.hpp>
#include <string>
#include <log4cxx/logger.h>

class IWorkWorker;
class SinkMsg;

namespace zmq {
	class i_socket_t;
}

class IPoller;

class Work {

private:

	log4cxx::LoggerPtr logger;
	IPoller *poller;
    zmq::i_socket_t *receiver;
    zmq::i_socket_t *control;
    
public:
	Work(log4cxx::LoggerPtr l, IPoller *p, zmq::i_socket_t *r, zmq::i_socket_t *ctrl) : 
		logger(l), poller(p), receiver(r), control(ctrl) {}
	
	void process(IWorkWorker *worker);
	
};

#endif // __WORK_HPP_INCLUDED__

