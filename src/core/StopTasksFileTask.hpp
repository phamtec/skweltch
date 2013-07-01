
#ifndef __STOPTASKSFILETASK_HPP_INCLUDED__
#define __STOPTASKSFILETASK_HPP_INCLUDED__

#include <iostream>

#include "IFileTask.hpp"

class IExeRunner;

class StopTasksFileTask : public IFileTask {

private:
	IExeRunner *runner;
	
public:
	StopTasksFileTask(IExeRunner *r) : runner(r) {}

	virtual void process(std::istream *stream);
	
};

#endif // __STOPTASKSFILETASK_HPP_INCLUDED__