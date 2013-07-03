
#include "jsonConfig.hpp"
#include "ExeRunner.hpp"
#include "StopTasksFileTask.hpp"
#include "FileProcessor.hpp"

#include <iostream>
#include <fstream>

using namespace std;
using namespace boost;

int main (int argc, char *argv[])
{
	if (argc != 2) {
		cerr << "usage: " << argv[0] << " jsonConfig" << endl;
		return 1;
	}
	
	ifstream jsonfile(argv[1]);
	JsonConfig c(&jsonfile);
	boost::property_tree::ptree r;
	if (!c.read(&r, &cout)) {
		return 1;
	}

   	string pidFilename = r.get<string>("pidFile");
   	
   	ExeRunner er;

	// first stop anything.
	StopTasksFileTask t(&er);
	FileProcessor fp(&t);
	fp.processFileIfExistsThenDelete(pidFilename);

	stringstream pipes;
	pipes << "'" << c.getChildAsString(r, "pipes") << "'";

	// now run up the workers.
	{
		boost::property_tree::ptree bg = r.get_child("background");
		ofstream pidfile(pidFilename.c_str());
		for (boost::property_tree::ptree::iterator i=bg.begin(); i != bg.end(); i++) {
			int count = i->second.get("count", 0);
			string exe(i->second.get<string>("exe"));
			stringstream config;
			config << "'" << c.getChildAsString(i->second, "config") << "'";
			if (count > 0) {
				for (int i=0; i<count; i++) {
					stringstream cmd;
					cmd << exe << " " << i << " " << pipes.str() << " " << config.str();
					pid_t pid = er.run(cmd.str());
					pidfile << pid << endl;
				}
			}
			else {
				stringstream cmd;
				cmd << exe << " " << pipes.str() << " " << config.str();
				pid_t pid = er.run(cmd.str());
				pidfile << pid << endl;
			}
		}
	}
	
	// something to vent.
	{
		boost::property_tree::ptree vent = r.get_child("vent");
		stringstream exe;
		exe << vent.get<string>("exe") << " " << pipes.str() << " '" << c.getChildAsString(vent, "config") << "'";
		er.run(exe.str());
	}
	
	// something to reap.
	{
		boost::property_tree::ptree reap = r.get_child("reap");
		stringstream exe;
		exe << reap.get<string>("exe") << "'" << " " << pipes.str() << " '" << c.getChildAsString(reap, "config");
		er.run(exe.str());
	}

	return 0;
	
}
