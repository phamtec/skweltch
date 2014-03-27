
#ifndef __MAKEANALYSER_HPP_INCLUDED__
#define __MAKEANALYSER_HPP_INCLUDED__

#include <log4cxx/logger.h>
#include <iostream>

struct BuildStatus {

	bool workDone;
	bool success;

};

/*
	Class used to analyse a make build.
*/

class MakeAnalyser {

	log4cxx::LoggerPtr logger;

public:
	MakeAnalyser(log4cxx::LoggerPtr l) : logger(l) {}
	
	BuildStatus analyse(std::istream *stream);
	
};

#endif // __MAKEANALYSER_HPP_INCLUDED__

