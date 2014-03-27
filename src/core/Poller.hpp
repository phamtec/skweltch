
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
	virtual bool poll(zmq::i_socket_t *socket, int timeout) = 0;
	
};

class Poller : public IPoller {

private:
	log4cxx::LoggerPtr logger;
	
public:
	Poller(log4cxx::LoggerPtr l) : logger(l) {}

	virtual bool poll(zmq::i_socket_t *socket, int timeout);
	
};

#endif // __POLLER_HPP_INCLUDED__

