
#ifndef __MACHINETUNER_HPP_INCLUDED__
#define __MACHINETUNER_HPP_INCLUDED__

class JsonObject;
#include <string>
#include <boost/random/mersenne_twister.hpp>
#include <log4cxx/logger.h>

/*
	A machine tuner.
*/

class MachineTuner {

	log4cxx::LoggerPtr logger;
	JsonObject *config;
	JsonObject *tunerconfig;
	boost::random::mt19937 gen;
	bool failonerror;
	int *interrupted;
	
public:
	MachineTuner(log4cxx::LoggerPtr l, JsonObject *c, JsonObject *tc, int *i) : 
		logger(l), config(c), tunerconfig(tc), failonerror(true), interrupted(i) {};
	
	void resetFail() { failonerror = true; }
	bool failOnError() { return failonerror; }
	bool tune(int group, int iteration, std::string *varstring);

};

#endif // __MACHINETUNER_HPP_INCLUDED__

