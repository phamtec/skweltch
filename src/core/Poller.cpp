
#include "Poller.hpp"

#include <zmq.hpp>
#include <msgpack.hpp>

using namespace std;

int Poller::poll(zmq::i_socket_t *socket, zmq::i_socket_t *control, int timeout) {

	void *saddr = *socket;
	void *caddr = *control;
	
	zmq::pollitem_t items[] = { 
		{ saddr, 0, ZMQ_POLLIN, 0 }, 
		{ caddr, 0, ZMQ_POLLIN, 0 } 
	};
 	zmq::poll (&items[0], 2, timeout);
 	
 	if (items[0].revents & ZMQ_POLLIN) {
 		return MSG;
 	}
 	else if (items[1].revents & ZMQ_POLLIN) {
 		return CONTROL;
 	}
 	else {
 		return NONE;
 	}

}
