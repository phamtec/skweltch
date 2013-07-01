
#include "jsonConfig.hpp"
#include "jsonNode.hpp"
#include "ExeRunner.hpp"
#include "StopTasksFileTask.hpp"
#include "FileProcessor.hpp"

#include <iostream>
#include <fstream>

using namespace std;
using namespace boost;

void start1Background(ExeRunner *er, ostream *pidfile, const std::string &exe, const std::string &config) {
	stringstream cmd;
	cmd << exe << " " << config;
	pid_t pid = er->run(cmd.str());
	*pidfile << pid << endl;
}

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
				for (int i=0; i<count; i++) {
					stringstream cmd;
					cmd << exe << " " << i;
					start1Background(&er, &pidfile, cmd.str(), config.str());
				}
			}
			else {
				start1Background(&er, &pidfile, exe, config.str());
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
