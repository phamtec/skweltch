
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
	
public:
	MachineTuner(JsonObject *c, JsonObject *tc) : config(c), tunerconfig(tc), failonerror(true) {};
	
	void resetFail() { failonerror = true; }
	bool failOnError() { return failonerror; }
	bool tune(int group, int iteration, std::string *varstring);
	bool runOne(const std::string &machine, int iterations, int group, int i, const std::string &vars);

};

#endif // __MACHINETUNER_HPP_INCLUDED__

