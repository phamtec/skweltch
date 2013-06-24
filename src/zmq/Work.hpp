
#ifndef __WORK_HPP_INCLUDED__
#define __WORK_HPP_INCLUDED__

#include <zmq.hpp>
#include <string>

class IMessager;
class JsonNode;

class IWorkMsgExecutor {

public:

	virtual void init() = 0;
	virtual void doit(zmq::message_t *message) = 0;

};

class Work {

private:
	IMessager *msg;
	IWorkMsgExecutor *work;
	
public:
	Work(IMessager *m, IWorkMsgExecutor *w) : msg(m), work(w) {}
	
	void service(JsonNode *root);
	
};

#endif // __WORK_HPP_INCLUDED__
