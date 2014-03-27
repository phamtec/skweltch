
#include "TaskMonitor.hpp"

#include "JsonObject.hpp"
#include "JsonArray.hpp"
#include "JsonNamePredicate.hpp"
#include "ExeRunner.hpp"
#include "PipeBuilder.hpp"

#include <sys/wait.h>
#include <czmq.h>
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <log4cxx/logger.h>

using namespace std;
using namespace boost;

bool TaskMonitor::start(JsonObject *root, vector<int> *pids) {

   	ExeRunner er(logger);
	
	// now run up the workers.
	try {
		
        if (root->has("sink")) {
            // sink.
            if (!runOne(root, root->findObj(JsonNamePredicate(root->getString("sink"))), pids)) {
                return false;
            }
        }
        
        // vent
        if (root->has("vent")) {
            if (!doVent(root, pids)) {
                return false;
            }
        }
        
		// allow the sockets to be created.
		zclock_sleep(50);
		
		// the other blocks.
		JsonArray bg = root->getArray("blocks");
		for (JsonArray::iterator i=bg.begin(); i != bg.end(); i++) {
		
			string name = bg.getString(i, "name");
			
            // skip over a sink or vent if there was one.
            if (root->has("sink") && root->getString("sink") == name) {
                continue;
            }
            if (root->has("vent") && root->getString("vent") == name) {
                continue;
            }
            
			int count = bg.getInt(i, "count");
            
            if (bg.has(i, "exe")) {
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

	}
	catch (runtime_error &e) {
		LOG4CXX_ERROR(logger, "error: " << e.what() << " running up backgrounds")
		return false;
	}

	return true;

}

bool TaskMonitor::doVent(JsonObject *root, vector<int> *pids) {

	return runOne(root, root->findObj(JsonNamePredicate(root->getString("vent"))), pids);
	
}

bool TaskMonitor::doReap(JsonObject *root, vector<int> *pids) {

   	ExeRunner er(logger);

	for (vector<int>::iterator i=pids->begin(); i != pids->end(); i++) {
        ::kill(*i, SIGTERM);
        zclock_sleep(20);
        ::kill(*i, SIGKILL);
	}

	return true;
	
}

bool TaskMonitor::doBlock(JsonObject *root, std::vector<int> *pids, const std::string &block) {

 	return runOne(root, root->findObj(JsonNamePredicate(block)), pids);
	
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

