
#include "Poller.hpp"

#include <zmq.hpp>
#include <msgpack.hpp>

using namespace std;

bool Poller::poll(zmq::i_socket_t *socket, int timeout) {

	void *addr = *socket;
	LOG4CXX_TRACE(logger, addr)
	
	zmq::pollitem_t items[] = { { addr, 0, ZMQ_POLLIN, 0 } };
 	zmq::poll (&items[0], 1, timeout);
 	
	LOG4CXX_TRACE(logger, "events: " << items[0].events)
	LOG4CXX_TRACE(logger, "revents: " << items[0].revents)
 	
 	return items[0].revents & ZMQ_POLLIN;
 	
}
