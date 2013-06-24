
#ifndef __RUNNER_HPP_INCLUDED__
#define __RUNNER_HPP_INCLUDED__

#include <fstream>

void startBackground(std::ofstream *pidfile, int n, const std::string &exe);
void startBackground(std::ofstream *pidfile, const std::string &exe);
void stopBackground(const std::string &pidfilename);
int runExe(const std::string &exe);

class IExeRunner {
public:
};

class ExeRunner : IExeRunner {
};

class Runner {

private:
	IExeRunner *runner;
	
public:
	Runner(IExeRunner *r) : runner(r) {}
	
	void startBackground(std::ofstream *pidfile, int n, const std::string &exe);
	void startBackground(std::ofstream *pidfile, const std::string &exe);
	void stopBackground(const std::string &pidfilename);
	int runExe(const std::string &exe);
	
};

#endif // __RUNNER_HPP_INCLUDED__
