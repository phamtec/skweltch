
#ifndef __EXERUNNER_HPP_INCLUDED__
#define __EXERUNNER_HPP_INCLUDED__

#include "IExeRunner.hpp"
#include <log4cxx/logger.h>

/*
	Class used to run and kill a program.
*/

class ExeRunner : public IExeRunner {

	log4cxx::LoggerPtr logger;

public:
	ExeRunner(log4cxx::LoggerPtr l) : logger(l) {}

	virtual int run(const std::string &exe);

};

#endif // __EXERUNNER_HPP_INCLUDED__

