
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
	if (argc != 3) {
		cerr << "usage: " << argv[0] << " jsonConfig [start|stop|run|wait]" << endl;
		return 1;
	}
	
	ifstream jsonfile(argv[1]);
	JsonConfig c(&jsonfile);
	JsonNode r;
	c.read(&r);

   	string pidFilename = r.getString("pidFile");
   	
   	ExeRunner er;

	if (std::string(argv[2]) == "start" || std::string(argv[2]) == "stop") {
	
		// first stop.
		StopTasksFileTask t(&er);
		FileProcessor fp(&t);
		fp.processFileIfExistsThenDelete(pidFilename);

		if (std::string(argv[2]) == "start") {
	
			Runner runner(&er);
		
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
	}
	else if (std::string(argv[2]) == "run") {
		stringstream exe;
		exe << r.getString("run") << " '" << r.getChildAsString("config") << "'";
		er.run(exe.str());
	}
	else if (std::string(argv[2]) == "wait") {
	}
	else {
		cerr << "usage: " << argv[0] << " jsonConfig [start|stop|run|wait]" << endl;
		return 1;
	}

	return 0;
	
}
