
#include "Runner.hpp"
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
	JsonNode r;
	if (!c.read(&r, &std::cout)) {
		return 1;
	}

   	string pidFilename = r.getString("pidFile");
   	
   	ExeRunner er;
	Runner runner(&er);

	// first stop anything.
	StopTasksFileTask t(&er);
	FileProcessor fp(&t);
	fp.processFileIfExistsThenDelete(pidFilename);

	// now run up the workers.
	{
		ofstream pidfile(pidFilename.c_str());
		JsonNode bg;
		r.getChild("background", &bg);
		bg.start();
		while (bg.hasMore()) {
			int count = bg.current()->getInt("count", 0);
			string exe(bg.current()->getString("exe"));
			stringstream config;
			config << "'" << bg.current()->getChildAsString("config") << "'";
			if (count > 0) {
				runner.startBackground(&pidfile, count, exe, config.str());
			}
			else {
				runner.startBackground(&pidfile, exe, config.str());
			}
			bg.next();
		}
	}

	// something to vent.
	{
		JsonNode vent;
		r.getChild("vent", &vent);
		stringstream exe;
		exe << vent.getString("exe") << " '" << vent.getChildAsString("config") << "'";
		er.run(exe.str());
	}
	
	// something to reap.
	{
		JsonNode reap;
		r.getChild("reap", &reap);
		stringstream exe;
		exe << reap.getString("exe") << " '" << reap.getChildAsString("config") << "'";
		er.run(exe.str());
	}

	return 0;
	
}
