
#include "Runner.hpp"
#include "jsonConfig.hpp"
#include <fstream>

using namespace boost;
using namespace std;

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

   	ExeRunner er;
   	Runner runner(&er);
	StopTasksFileTask t(&runner);
	FileProcessor fp(&t);
	fp.processFileIfExistsThenDelete(r.getString("pidFile"));

	return 0;
	
}
