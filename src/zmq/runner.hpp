
#ifndef __RUNNER_HPP_INCLUDED__
#define __RUNNER_HPP_INCLUDED__

#include <string>

class IExeRunner {
public:

	virtual int run(const std::string &exe) = 0;
	virtual void kill(int pid) = 0;
	
};

class ExeRunner : public IExeRunner {

public:

	virtual int run(const std::string &exe);
	virtual void kill(int pid);

};

class Runner {

private:
	IExeRunner *runner;
	
	void start1Background(std::ostream *pidfile, const std::string &exe);

public:
	Runner(IExeRunner *r) : runner(r) {}
	
	void startBackground(std::ostream *pidfile, int n, const std::string &exe);
	void startBackground(std::ostream *pidfile, const std::string &exe);
	void stopBackground(std::istream *pidfile);
	
};

class IFileTask {

public:

	virtual void process(std::istream *stream) = 0;
	
};

class FileProcessor {

private:
	IFileTask *task;
	
public:
	FileProcessor(IFileTask *t) : task(t) {}

	void processFileIfExistsThenDelete(const std::string &filename);
	
};

class StopTasksFileTask : public IFileTask {

private:
	Runner *runner;
	
public:
	StopTasksFileTask(Runner *r) : runner(r) {}

	virtual void process(std::istream *stream);
	
};

#endif // __RUNNER_HPP_INCLUDED__
