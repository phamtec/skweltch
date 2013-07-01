
#include "jsonConfig.hpp"
#include "jsonNode.hpp"
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
	JsonNode r;
	if (!c.read(&r, &cout)) {
		return 1;
	}

   	string pidFilename = r.getString("pidFile");
   	
   	ExeRunner er;

	// first stop anything.
	StopTasksFileTask t(&er);
	FileProcessor fp(&t);
	fp.processFileIfExistsThenDelete(pidFilename);

	stringstream pipes;
	pipes << "'" << r.getChildAsString("pipes") << "'";

	// now run up the workers.
	{
		JsonNode bg;
		r.getChild("background", &bg);
		bg.start();
		ofstream pidfile(pidFilename.c_str());
		while (bg.hasMore()) {
			int count = bg.current()->getInt("count", 0);
			string exe(bg.current()->getString("exe"));
			stringstream config;
			config << "'" << bg.current()->getChildAsString("config") << "'";
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
			bg.next();
		}
	}
	
	// something to vent.
	{
		JsonNode vent;
		r.getChild("vent", &vent);
		stringstream exe;
		exe << vent.getString("exe") << " " << pipes.str() << " '" << vent.getChildAsString("config") << "'";
		er.run(exe.str());
	}
	
	// something to reap.
	{
		JsonNode reap;
		r.getChild("reap", &reap);
		stringstream exe;
		exe << reap.getString("exe") << "'" << " " << pipes.str() << " '" << reap.getChildAsString("config");
		er.run(exe.str());
	}

	return 0;
	
}
