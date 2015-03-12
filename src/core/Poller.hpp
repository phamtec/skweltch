
#ifndef __POLLER_HPP_INCLUDED__
#define __POLLER_HPP_INCLUDED__

#include <zmq.hpp>
#include <string>
#include <log4cxx/logger.h>

namespace zmq {
	class i_socket_t;
}

class IPoller {

public:
	virtual int poll(zmq::i_socket_t *socket, zmq::i_socket_t *control, int timeout) = 0;
	
};

class Poller : public IPoller {

private:
	log4cxx::LoggerPtr logger;
	
public:
	Poller(log4cxx::LoggerPtr l) : logger(l) {}

	virtual int poll(zmq::i_socket_t *socket, zmq::i_socket_t *control, int timeout);
	
	static const int NONE = 0;
	static const int MSG = 1;
	static const int CONTROL = 2;
	
};

#endif // __POLLER_HPP_INCLUDED__

