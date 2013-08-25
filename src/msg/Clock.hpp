
#ifndef __CLOCK_HPP_INCLUDED__
#define __CLOCK_HPP_INCLUDED__

#include "IClock.hpp"

/*
	The real system clock.
*/

class Clock : public IClock {

public:

	virtual clock_t time() { return ::clock(); }
	
};

#endif // __CLOCK_HPP_INCLUDED__

