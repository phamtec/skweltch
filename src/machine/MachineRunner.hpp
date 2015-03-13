
#ifndef __MACHINERUNNER_HPP_INCLUDED__
#define __MACHINERUNNER_HPP_INCLUDED__

class JsonObject;
class Results;

#include <string>
#include <boost/random/mersenne_twister.hpp>
#include <log4cxx/logger.h>

/*
	A machine runner.
*/

class MachineRunner {

	boost::random::mt19937 gen;
	bool failonerror;
	log4cxx::LoggerPtr logger;
	int *interrupted;
	int resultsSleep;
	int resultsSleepCount;
    int settleTime;
	Results *results;
    
public:
	MachineRunner(log4cxx::LoggerPtr l, Results *r, int *i, int rs, int rsc, int st) :
		failonerror(false), logger(l), results(r), interrupted(i), resultsSleep(rs), resultsSleepCount(rsc), settleTime(st) {};
	
	void setFail(bool f) { failonerror = f; }
	bool runOne(const std::string &machine, const std::string &results, int control, int iterations, int group, int i, const std::string &vars, const JsonObject &success);

};

#endif // __MACHINERUNNER_HPP_INCLUDED__

