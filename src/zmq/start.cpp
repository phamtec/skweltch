
#include "runner.hpp"

#include <iostream>
#include <boost/property_tree/ptree.hpp>

using namespace std;
using namespace boost;

int main (int argc, char *argv[])
{
	if (argc != 2) {
		cerr << "usage: " << argv[0] << " jsonConfig" << endl;
		return 1;
	}
	
	property_tree::ptree pt;
	readConfig(argv[1], &pt);
  	
   	string pidFilename = pt.get<string>("pidFile");
   	
   	// stop anything already started.
	stopBackground(pidFilename);
	
	// and start anything required up.
	{
		ofstream pidfile(pidFilename.c_str());
		
		property_tree::ptree bg = pt.get_child("background");
		for (property_tree::ptree::iterator i = bg.begin(); i != bg.end(); i++) {
			int count = i->second.get("count", 0);
			if (count > 0) {
				startBackground(&pidfile, count, i->second.get<string>("exe"));
			}
			else {
				startBackground(&pidfile, i->second.get<string>("exe"));
			}
		}
	}

	return 0;
	
}
