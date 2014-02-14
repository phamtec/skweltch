
#ifndef __IPOLLWORKER_HPP_INCLUDED__
#define __IPOLLWORKER_HPP_INCLUDED__

class Poll;

/**
	An interface to the Poll worker.
*/

class IPollWorker {

public:

	// the worker can stash it's own data that it get's before sending.
	virtual bool waitEvent() = 0;
	
	// send them message with the id.
	virtual int send(int msgid, Poll *poll) = 0;
	
	// return true if we should quit.
	virtual bool shouldQuit() = 0;
	
};

#endif // __IPOLLWORKER_HPP_INCLUDED__
