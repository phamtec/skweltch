
#include "ExeRunner.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

using namespace std;
using namespace boost;

pid_t ExeRunner::run(const std::string &exe) {

	cout << "run: " << exe << endl;
	
    int fdset[2], nullfd;
    if (pipe(fdset) != 0) {
        cerr << "couldn't pipe" << endl;
        return 1;
    }
	pid_t pid = fork();
    switch (pid) {
    case -1: /* error */
        cerr << "couldn't fork" << endl;
        return 1;
        
    case 0: /* child */
        close(fdset[0]);
        
        if ((nullfd = open("/dev/null", O_RDONLY)) == -1)
            _exit(1);
        dup2(nullfd, STDIN_FILENO);
        dup2(fdset[1], STDOUT_FILENO);
        dup2(fdset[1], STDERR_FILENO);
        
        const char *args[4];
		args[0] = "/bin/sh";
        args[1] = "-c";
        args[2] = exe.c_str();
        args[3] = NULL;
        execv(args[0], (char* const*)args);
        _exit(1);
        break;
    
    default: /* parent */
        break;
	}
	
    close(fdset[1]);
    close(fdset[0]);    

	return pid;
	
}

void ExeRunner::kill(int pid) {
	::kill(pid, SIGKILL);
}
