
#ifndef __BOOSTANALYSER_HPP_INCLUDED__
#define __BOOSTANALYSER_HPP_INCLUDED__

#include <log4cxx/logger.h>
#include <iostream>

struct BuildStatus {

	int targets;
	int passedTests;
	bool workDone;
	bool success;

};

/*
	Class used to analyse a boost build.
*/

class BoostAnalyser {

	log4cxx::LoggerPtr logger;

public:
	BoostAnalyser(log4cxx::LoggerPtr l) : logger(l) {}
	
	BuildStatus analyse(std::istream *stream);
	
};

#endif // __BOOSTANALYSER_HPP_INCLUDED__

