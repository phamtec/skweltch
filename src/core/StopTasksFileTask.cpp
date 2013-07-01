
#include "StopTasksFileTask.hpp"
#include "IExeRunner.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

using namespace std;
using namespace boost;

void StopTasksFileTask::process(std::istream *stream) {

	string line;
	while (getline(*stream, line)) {
		pid_t pid = lexical_cast<int>(line);
		runner->kill(pid);
	}

}
