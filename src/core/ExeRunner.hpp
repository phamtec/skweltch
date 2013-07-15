
#ifndef __EXERUNNER_HPP_INCLUDED__
#define __EXERUNNER_HPP_INCLUDED__

#include "IExeRunner.hpp"

/*
	Class used to run and kill a program.
*/

class ExeRunner : public IExeRunner {

public:

	virtual int run(const std::string &exe);

};

#endif // __EXERUNNER_HPP_INCLUDED__

