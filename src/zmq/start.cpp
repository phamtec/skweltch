
#include "runner.hpp"
#include "jsonConfig.hpp"

#include <iostream>

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
	c.read(&r);

   	string pidFilename = r.getString("pidFile");
   	string exePath = r.getString("exePath");
   	
   	// stop anything already started.
	stopBackground(pidFilename);
	
	// and start anything required up.
	{
		ofstream pidfile(pidFilename.c_str());
		
		JsonNode bg;
		r.getChild("background", &bg);
		bg.start();
		while (bg.hasMore()) {
			int count = bg.current()->getInt("count", 0);
			stringstream exe;
			exe << exePath << "/" << bg.current()->getString("exe");
			if (count > 0) {
				startBackground(&pidfile, count, exe.str().c_str());
			}
			else {
				startBackground(&pidfile, exe.str().c_str());
			}
			bg.next();
		}

	}

	return 0;
	
}
