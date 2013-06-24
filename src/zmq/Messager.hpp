
#ifndef __MESSAGER_HPP_INCLUDED__
#define __MESSAGER_HPP_INCLUDED__

#include <zmq.hpp>

class IMessager {

public:

	virtual void send(zmq::message_t *message) = 0;
	virtual void receive(zmq::message_t *message) = 0;

};

class ZmQMessager: public IMessager {

private:
	zmq::socket_t *sender;
	zmq::socket_t *receiver;
	

public:

	void setSender(zmq::socket_t *s) { sender = s; }
	void setReceiver(zmq::socket_t *r) { receiver = r; }

	virtual void send(zmq::message_t *message);
	virtual void receive(zmq::message_t *message);
	
};

#endif // __MESSAGER_HPP_INCLUDED__
