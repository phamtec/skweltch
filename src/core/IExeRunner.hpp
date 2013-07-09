
#ifndef __IEXERUNNER_HPP_INCLUDED__
#define __IEXERUNNER_HPP_INCLUDED__

#include <string>

/*
	Abstract interface for running and killing a program.
*/

class IExeRunner {
public:

	virtual int run(const std::string &exe) = 0;
	virtual void kill(int pid) = 0;
	
};

#endif // __IEXERUNNER_HPP_INCLUDED__

