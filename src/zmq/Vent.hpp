
#ifndef __VENT_HPP_INCLUDED__
#define __VENT_HPP_INCLUDED__

#include <zmq.hpp>
#include <string>

class JsonNode;

class IVentMsgBuilder {

public:

	virtual void init() = 0;
	virtual void buildMessage(int n, zmq::message_t *message) = 0;
	
};

class Vent {

private:
	IVentMsgBuilder *work;
	
public:
	Vent(IVentMsgBuilder *w) : work(w) {}
	
	void service(JsonNode *root, zmq::i_socket_t *sender);
	
};

#endif // __VENT_HPP_INCLUDED__
