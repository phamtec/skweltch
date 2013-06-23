
#include "runner.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

using namespace std;
using namespace boost;

pid_t runExe(const std::string &exe) {

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

void start1Background(ofstream *pidfile, const std::string &exe) {
	pid_t pid = runExe(exe);
	*pidfile << pid << endl;
}

void startBackground(ofstream *pidfile, const std::string &exe) {
	cout << "starting..." << endl;
	start1Background(pidfile, exe);
}

void startBackground(ofstream *pidfile, int n, const std::string &exe) {
	cout << "starting (" << n << ")..." << endl;
	for (int i=0; i<n; i++) {
		stringstream cmd;
		cmd << exe << " " << i;
		start1Background(pidfile, cmd.str().c_str());
	}
}

void stopBackground(const std::string &pidfilename) {
	ifstream pidfile(pidfilename.c_str());
	string line;
	if (pidfile.is_open()) {
		cout << "stopping..." << endl;
		while (getline(pidfile, line)) {
			pid_t pid = lexical_cast<int>(line);
			kill(pid, SIGKILL);
		}
   		pidfile.close();
    	remove(pidfilename.c_str());
	}
}

void readConfig(const std::string &jsonfilename, property_tree::ptree *pt) {

	ifstream jsonfile(jsonfilename.c_str());
  	property_tree::read_json(jsonfile, *pt);

}

