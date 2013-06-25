
#ifndef __WORK_HPP_INCLUDED__
#define __WORK_HPP_INCLUDED__

#include <zmq.hpp>
#include <string>

class JsonNode;

class IWorkMsgExecutor {

public:

	virtual void init() = 0;
	virtual void doit(zmq::message_t *message) = 0;

};

class Work {

private:
	IWorkMsgExecutor *work;
	
public:
	Work(IWorkMsgExecutor *w) : work(w) {}
	
	void service(JsonNode *root, zmq::i_socket_t *receiver, zmq::i_socket_t *sender, bool forever);
	
};

#endif // __WORK_HPP_INCLUDED__
