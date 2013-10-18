
#ifndef __IWORKWORKER_HPP_INCLUDED__
#define __IWORKWORKER_HPP_INCLUDED__

#include <zmq.hpp>

class SinkMsg;
class Work;

/**
	An interface to the Work worker.
*/

class IWorkWorker {

public:

	virtual bool process(const zmq::message_t &message, SinkMsg *smsg) = 0;
		// process the message, building a sinkmsg. If true is returned,the sinkmsg is sent on.
		
	virtual bool shouldQuit() = 0;
		// return true if it's all over.
		
	virtual int getTimeout() { return 1000; }
		// there is a default timeout of 1 second.
		
	virtual void timeout(Work *work) {}
		// by default, a timeout does nothing.

};

#endif // __IWORKWORKER_HPP_INCLUDED__
