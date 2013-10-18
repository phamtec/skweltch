
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

class IPoller {

public:
	virtual bool poll(zmq::i_socket_t *socket, int timeout) = 0;
	
};

class Poller : public IPoller {

private:
	log4cxx::LoggerPtr logger;
	
public:
	Poller(log4cxx::LoggerPtr l) : logger(l) {}

	virtual bool poll(zmq::i_socket_t *socket, int timeout);
	
};

class Work {

private:

	log4cxx::LoggerPtr logger;
	IPoller *poller;
    zmq::i_socket_t *receiver;
    zmq::i_socket_t *sender;
    
public:
	Work(log4cxx::LoggerPtr l, IPoller *p, zmq::i_socket_t *r, zmq::i_socket_t *s) : logger(l), poller(p), receiver(r) , sender(s) {}
	
	void process(IWorkWorker *worker);
	void sendSink(const SinkMsg &smsg);
	
};

#endif // __WORK_HPP_INCLUDED__

