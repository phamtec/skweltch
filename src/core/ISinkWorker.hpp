
#ifndef __ISINKWORKER_HPP_INCLUDED__
#define __ISINKWORKER_HPP_INCLUDED__

class ISinkWorker {

public:
	
	virtual void first(int id) = 0;
	virtual void process(int id, int data) = 0;
	virtual void last(int id) = 0;
	virtual void results(int total_ms) = 0;
	virtual bool shouldQuit() = 0;
	
};

#endif // __ISINKWORKER_HPP_INCLUDED__

