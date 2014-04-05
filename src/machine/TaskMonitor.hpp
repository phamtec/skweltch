
#ifndef __TASKMONITOR_HPP_INCLUDED__
#define __TASKMONITOR_HPP_INCLUDED__

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <log4cxx/logger.h>

class JsonObject;

class TaskMonitor {

private:
	log4cxx::LoggerPtr logger;
	
	bool runOne(JsonObject *root, const JsonObject &obj, std::vector<int> *pids);
	
public:
	TaskMonitor(log4cxx::LoggerPtr l) : logger(l) {}

	bool start(JsonObject *root, std::vector<int> *pids);
	void waitFinish(const std::vector<int> &pids);
	bool doVent(JsonObject *root, std::vector<int> *pids);
	void writePidFile(JsonObject *root, std::vector<int> *pids);
	bool doReap(JsonObject *root, std::vector<int> *pids);
	bool doBlock(JsonObject *root, std::vector<int> *pids, const std::string &block);
};

#endif // __TASKMONITOR_HPP_INCLUDED__

