
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

    virtual void processMsg(const zmq::message_t &message) = 0;
        // process a message that was received.
    
	virtual bool shouldQuit() = 0;
		// return true if it's all over.
		
	virtual int getTimeout() { return 1000; }
		// there is a default timeout of 1 second.
		
	virtual void timeout(Work *work) {}
		// by default, a timeout does nothing.

};

#endif // __IWORKWORKER_HPP_INCLUDED__
