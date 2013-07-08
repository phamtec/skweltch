
#include "JsonConfig.hpp"
#include "JsonObject.hpp"
#include "JsonArray.hpp"
#include "ExeRunner.hpp"
#include "StopTasksFileTask.hpp"
#include "FileProcessor.hpp"

#include <iostream>
#include <fstream>

using namespace std;

int main (int argc, char *argv[])
{
	if (argc != 2) {
		cerr << "usage: " << argv[0] << " jsonConfig" << endl;
		return 1;
	}
	
	ifstream jsonfile(argv[1]);
	JsonConfig c(&jsonfile);
	JsonObject r;
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
		JsonArray bg = r.getArray("background");
		ofstream pidfile(pidFilename.c_str());
		for (JsonArray::iterator i=bg.begin(); i != bg.end(); i++) {
			int count = bg.getInt(i, "count");
			string exe(bg.getString(i, "exe"));
			stringstream config;
			config << "'" << bg.getChildAsString(i, "config") << "'";
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
		JsonObject vent = r.getChild("vent");
		stringstream exe;
		exe << vent.getString("exe") << " " << pipes.str() << " '" << vent.getChildAsString("config") << "'";
		er.run(exe.str());
	}
	
	// something to reap.
	{
		JsonObject reap = r.getChild("reap");
		stringstream exe;
		exe << reap.getString("exe") << " " << pipes.str() << " '" << reap.getChildAsString("config") << "'";
		er.run(exe.str());
	}

	return 0;
	
}
