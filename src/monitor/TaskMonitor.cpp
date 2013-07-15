
#include "TaskMonitor.hpp"

#include "JsonConfig.hpp"
#include "JsonObject.hpp"
#include "JsonArray.hpp"
#include "ExeRunner.hpp"
#include "PipeBuilder.hpp"

#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

using namespace std;
using namespace boost;

int TaskMonitor::start(const string &jsonconfig) {

	ifstream jsonfile(jsonconfig.c_str());
	JsonConfig c(&jsonfile);
	JsonObject r;
	if (!c.read(&r)) {
		return 1;
	}
    
    string pidfilename = r.getString("pidFile");
   	ExeRunner er;
	vector<int> pids;
	
	// now run up the workers.
	try {
		ofstream pidfile(pidfilename.c_str());
		JsonArray bg = r.getArray("background");
		for (JsonArray::iterator i=bg.begin(); i != bg.end(); i++) {
		
			int count = bg.getInt(i, "count");
			string name = bg.getString(i, "name");
			string exe = bg.getString(i, "exe");
			
			// build pipes for this node.
			JsonObject pipesjson = PipeBuilder().collect(&r, bg.getValue(i));
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
						return 1;
					}
					pidfile << pid << endl;
					pids.push_back(pid);
				}
			}
			else {
				stringstream cmd;
				cmd << exe << " '" << pipes.str() << "' " << config.str() << " " << name;
				pid_t pid = er.run(cmd.str());
				if (pid == 0) {
					return 1;
				}
				pidfile << pid << endl;
				pids.push_back(pid);
			}
		}

		JsonObject vent = r.getChild("vent");
		
		// build pipes for this node.
		JsonObject pipesjson = PipeBuilder().collect(&r, vent);
		stringstream pipes;
		pipesjson.write(false, &pipes);

		stringstream exe;
		exe << vent.getString("exe") << " '" << pipes.str() << "' '" << vent.getChildAsString("config") << "' " << vent.getString("name");
		pid_t pid = er.run(exe.str());
		if (pid == 0) {
			return 1;
		}
		pidfile << pid << endl;
		pids.push_back(pid);
	}
	catch (runtime_error &e) {
		BOOST_LOG_TRIVIAL(error) << "error: " << e.what() << " running up backgrounds";
		return 1;
	}
	
	// something to reap.
	try {
		JsonObject reap = r.getChild("reap");
		
		// build pipes for this node.
		JsonObject pipesjson = PipeBuilder().collect(&r, reap);
		stringstream pipes;
		pipesjson.write(false, &pipes);

		stringstream exe;
		exe << reap.getString("exe") << " '" << pipes.str() << "' '" << reap.getChildAsString("config") << "' " << reap.getString("name");
		pid_t pid = er.run(exe.str());
		if (pid == 0) {
			return 1;
		}
		pids.push_back(pid);
	}
	catch (runtime_error &e) {
		BOOST_LOG_TRIVIAL(error) << "error: " << e.what() << " running up reap";
		return 1;
	}

	// make sure we ack all the children.
	for (vector<int>::iterator i=pids.begin(); i != pids.end(); i++) {
		::waitpid(*i, NULL, 0);	
	}
	
	return 0;

}
