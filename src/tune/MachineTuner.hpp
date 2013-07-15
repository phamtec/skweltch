
#ifndef __MACHINETUNER_HPP_INCLUDED__
#define __MACHINETUNER_HPP_INCLUDED__

class JsonObject;
#include <string>

/*
	A machine tuner.
*/

class MachineTuner {

	JsonObject *config;
	JsonObject *tunerconfig;
	
public:
	MachineTuner(JsonObject *c, JsonObject *tc) : config(c), tunerconfig(tc) {};
	
	bool tune(int group, int iteration);
	bool runOne(const std::string &machine, int iterations);

};

#endif // __MACHINETUNER_HPP_INCLUDED__

