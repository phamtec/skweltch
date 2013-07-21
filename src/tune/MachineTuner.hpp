
#ifndef __MACHINETUNER_HPP_INCLUDED__
#define __MACHINETUNER_HPP_INCLUDED__

class JsonObject;
#include <string>
#include <boost/random/mersenne_twister.hpp>

/*
	A machine tuner.
*/

class MachineTuner {

	JsonObject *config;
	JsonObject *tunerconfig;
	boost::random::mt19937 gen;
	bool failonerror;
	int *interrupted;
	
public:
	MachineTuner(JsonObject *c, JsonObject *tc, int *i) : 
		config(c), tunerconfig(tc), failonerror(true), interrupted(i) {};
	
	void resetFail() { failonerror = true; }
	bool failOnError() { return failonerror; }
	bool tune(int group, int iteration, std::string *varstring);

};

#endif // __MACHINETUNER_HPP_INCLUDED__

