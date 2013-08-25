
#ifndef __ICLOCK_HPP_INCLUDED__
#define __ICLOCK_HPP_INCLUDED__

/*
	Abstract interface for a clock.
*/

class IClock {
public:

	virtual clock_t time() = 0;
	
};

#endif // __ICLOCK_HPP_INCLUDED__

