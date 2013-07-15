
#include "ExeRunner.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>

using namespace std;
using namespace boost;

pid_t ExeRunner::run(const std::string &exe) {

	pid_t pid = fork();
    switch (pid) {
    case -1: /* error */
    	BOOST_LOG_TRIVIAL(error) << "couldn't fork";
        return 0;
        
    case 0: /* child */        
        const char *args[4];
        args[0] = "/bin/sh";
        args[1] = "-c";
        args[2] = exe.c_str();
        args[3] = NULL;
        execvp(args[0], (char* const*)args);
    	// The execvp function returns only if an error occurs.
    	BOOST_LOG_TRIVIAL(error) << "an error occurred in execvp";
    	abort();
        break;
    
    default: /* parent */
       	break;
	}
	
	return pid;
	
}
