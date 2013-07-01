
#ifndef __EXERUNNER_HPP_INCLUDED__
#define __EXERUNNER_HPP_INCLUDED__

#include "IExeRunner.hpp"

class ExeRunner : public IExeRunner {

public:

	virtual int run(const std::string &exe);
	virtual void kill(int pid);

};

#endif // __EXERUNNER_HPP_INCLUDED__
