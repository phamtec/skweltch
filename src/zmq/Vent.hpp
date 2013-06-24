
#ifndef __VENT_HPP_INCLUDED__
#define __VENT_HPP_INCLUDED__

#include <zmq.hpp>
#include <string>

class IMessager;
class JsonNode;

class IVentMsgBuilder {

public:

	virtual void init() = 0;
	virtual void buildMessage(int n, zmq::message_t *message) = 0;
	
};

class Vent {

private:
	IMessager *msg;
	IVentMsgBuilder *work;
	
public:
	Vent(IMessager *m, IVentMsgBuilder *w) : msg(m), work(w) {}
	
	void service(JsonNode *root);
	
};

#endif // __VENT_HPP_INCLUDED__
