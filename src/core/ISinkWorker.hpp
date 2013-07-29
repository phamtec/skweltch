
#ifndef __ISINKWORKER_HPP_INCLUDED__
#define __ISINKWORKER_HPP_INCLUDED__

#include <string>

class ISinkWorker {

public:
	
	virtual void first(int id) = 0;
	virtual void process(int id, const std::string &data) = 0;
	virtual void last(int id) = 0;
	virtual void results(int total_ms) = 0;
	virtual bool shouldQuit() = 0;
	
};

#endif // __ISINKWORKER_HPP_INCLUDED__

