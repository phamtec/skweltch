
#include "TaskMonitor.hpp"

#include "JsonObject.hpp"
#include "JsonArray.hpp"
#include "ExeRunner.hpp"
#include "PipeBuilder.hpp"

#include <sys/wait.h>
#include <czmq.h>
#include <iostream>
#include <fstream>
#include <log4cxx/logger.h>

using namespace std;
using namespace boost;

bool TaskMonitor::start(JsonObject *root, vector<int> *pids, bool demonize) {

   	ExeRunner er(logger);
	
	// now run up the workers.
	try {
		
		// sink.
		if (!runOne(root, root->getChild("sink"), pids)) {
			return false;
		}

		// vent
		if (!demonize)
		{
			if (!doVent(root, pids)) {
				return false;
			}
		}
		
		// allow the sockets to be created.
		zclock_sleep(50);
		
		// the workers.
		JsonArray bg = root->getArray("background");
		for (JsonArray::iterator i=bg.begin(); i != bg.end(); i++) {
		
			int count = bg.getInt(i, "count");
			string name = bg.getString(i, "name");
			string exe = bg.getString(i, "exe");
			
			// build pipes for this node.
			JsonObject pipesjson = PipeBuilder(logger).collect(root, bg.getValue(i));
			stringstream pipes;
			pipesjson.write(false, &pipes);
			
			stringstream config;
			config << "'" << bg.getChildAsString(i, "config") << "'";
			if (count > 0) {
				for (int i=0; i<count; i++) {
					stringstream cmd;
					cmd << exe << " '" << pipes.str() << "' " << config.str() << " " << name << "[" << i << "]";
					pid_t pid = er.run(cmd.str());
					if (pid == 0) {
						return false;
					}
					if (pids) {
						pids->push_back(pid);
					}
				}
			}
			else {
				stringstream cmd;
				cmd << exe << " '" << pipes.str() << "' " << config.str() << " " << name;
				pid_t pid = er.run(cmd.str());
				if (pid == 0) {
					return false;
				}
				if (pids) {
					pids->push_back(pid);
				}
			}
		}

	}
	catch (runtime_error &e) {
		LOG4CXX_ERROR(logger, "error: " << e.what() << " running up backgrounds")
		return false;
	}
	
	// this is used later by the reaper.
	writePidFile(root, pids);
	
	// and if we are reaping, do so.
	if (!demonize) {
		if (!doReap(root, pids)) {
			return false;
		}
	}

	return true;

}

bool TaskMonitor::doVent(JsonObject *root, vector<int> *pids) {

	return runOne(root, root->getChild("vent"), pids);
	
}

void TaskMonitor::writePidFile(JsonObject *root, vector<int> *pids) {

   	string pidfilename = root->getString("pidFile");

	// write out the pids for the reaper. We should just pass this as an arg.
	{
		ofstream pidfile(pidfilename.c_str());
		for (vector<int>::iterator i=pids->begin(); i != pids->end(); i++) {
			pidfile << *i << endl;
		}
	}
}

bool TaskMonitor::doReap(JsonObject *root, vector<int> *pids) {

   	string pidfilename = root->getString("pidFile");

   	ExeRunner er(logger);

	try {
		JsonObject reap = root->getChild("reap");
	
		stringstream exe;
		exe << reap.getString("exe") << " " << pidfilename << " '"<< reap.getChildAsString("config") << "' " << reap.getString("name");
		pid_t pid = er.run(exe.str());
		if (pid == 0) {
			return false;
		}
		if (pids) {
			pids->push_back(pid);
		}
	}
	catch (runtime_error &e) {
		LOG4CXX_ERROR(logger, "error: " << e.what() << " running up reap")
		return false;
	}
	
	return true;
	
}

void TaskMonitor::waitFinish(const vector<int> &pids) {

	// make sure we ack all the children.
	for (vector<int>::const_iterator i=pids.begin(); i != pids.end(); i++) {
		zclock_sleep(20);
		::waitpid(*i, NULL, 0);	
	}
	
}

bool TaskMonitor::runOne(JsonObject *root, const JsonObject &obj, vector<int> *pids)  {

   	ExeRunner er(logger);

	// build pipes for this node.
	JsonObject pipesjson = PipeBuilder(logger).collect(root, obj);
	stringstream pipes;
	pipesjson.write(false, &pipes);

	stringstream exe;
	exe << obj.getString("exe") << " '" << pipes.str() << "' '" << obj.getChildAsString("config") << "' " << obj.getString("name");
	pid_t pid = er.run(exe.str());
	if (pid == 0) {
		return false;
	}
	if (pids) {
		pids->push_back(pid);
	}
	
	return true;
}

