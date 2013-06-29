
#include "Runner.hpp"
#include "IExeRunner.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

using namespace std;
using namespace boost;

void Runner::start1Background(ostream *pidfile, const std::string &exe, const std::string &config) {
	stringstream cmd;
	cmd << exe << " " << config;
	pid_t pid = runner->run(cmd.str());
	*pidfile << pid << endl;
}

void Runner::startBackground(ostream *pidfile, const std::string &exe, const std::string &config) {
	start1Background(pidfile, exe, config);
}

void Runner::startBackground(ostream *pidfile, int n, const std::string &exe, const std::string &config) {
	for (int i=0; i<n; i++) {
		stringstream cmd;
		cmd << exe << " " << i;
		start1Background(pidfile, cmd.str(), config);
	}
}
