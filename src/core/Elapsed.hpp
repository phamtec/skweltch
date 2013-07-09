
#ifndef __ELAPSED_HPP_INCLUDED__
#define __ELAPSED_HPP_INCLUDED__

#include <sys/time.h>

/*
	Class used to measure elapsed time in ms.
*/

class Elapsed {

private:
    struct timeval tstart;

public:
	Elapsed();
	
	int getTotal();
	
};

#endif // __ELAPSED_HPP_INCLUDED__

