
#ifndef __RUNNER_HPP_INCLUDED__
#define __RUNNER_HPP_INCLUDED__

#include <string>

class IExeRunner;

class Runner {

private:
	IExeRunner *runner;
	
	void start1Background(std::ostream *pidfile, const std::string &exe, const std::string &config);

public:
	Runner(IExeRunner *r) : runner(r) {}
	
	void startBackground(std::ostream *pidfile, int n, const std::string &exe, const std::string &config);
	void startBackground(std::ostream *pidfile, const std::string &exe, const std::string &config);
	
};

#endif // __RUNNER_HPP_INCLUDED__
