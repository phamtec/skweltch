
#ifndef __IGNOREWHILEWORKING_HPP_INCLUDED__
#define __IGNOREWHILEWORKING_HPP_INCLUDED__

#include <time.h>
#include "IIgnore.hpp"

class IClock;

/*
	Class used to ignore messages that came while you were working.
*/

class IgnoreWhileWorking : public IIgnore {

private:
	IClock *clock;
	clock_t localStart;
	clock_t localEnd;
	clock_t remoteStart;
	
public:
	IgnoreWhileWorking(IClock *c) : clock(c), localStart(0), localEnd(0), remoteStart(0) {}
	
	virtual bool canIgnore(IDataMsg *msg);
	
	void start(IDataMsg *msg);
	void end();
	
};

#endif // __IGNOREWHILEWORKING_HPP_INCLUDED__

