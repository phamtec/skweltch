
#include "ExeRunner.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <log4cxx/logger.h>

using namespace std;
using namespace boost;

pid_t ExeRunner::run(const std::string &exe) {

	LOG4CXX_DEBUG(logger, exe)
 	
	pid_t pid = fork();
    switch (pid) {
    case -1: /* error */
 		LOG4CXX_ERROR(logger, "couldn't fork")
        return 0;
        
    case 0: /* child */        
        const char *args[4];
        args[0] = "/bin/sh";
        args[1] = "-c";
        args[2] = exe.c_str();
        args[3] = NULL;
        execvp(args[0], (char* const*)args);
        _exit(0);
        break;
    
    default: /* parent */
       	break;
	}
	
	return pid;
	
}
