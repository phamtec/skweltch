
#ifndef __IWORKWORKER_HPP_INCLUDED__
#define __IWORKWORKER_HPP_INCLUDED__

#include <zmq.hpp>

class SinkMsg;

class IWorkWorker {

public:

	virtual void process(const zmq::message_t &message, SinkMsg *smsg) = 0;
	virtual bool shouldQuit() = 0;

};

#endif // __IWORKWORKER_HPP_INCLUDED__
