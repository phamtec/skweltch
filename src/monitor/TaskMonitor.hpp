
#ifndef __TASKMONITOR_HPP_INCLUDED__
#define __TASKMONITOR_HPP_INCLUDED__

#include <string>
#include <log4cxx/logger.h>

class TaskMonitor {

	log4cxx::LoggerPtr logger;

public:
	TaskMonitor(log4cxx::LoggerPtr l) : logger(l) {}

	int start(const std::string &jsonconfig);

};

#endif // __TASKMONITOR_HPP_INCLUDED__

