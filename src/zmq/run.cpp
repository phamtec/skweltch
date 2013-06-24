#include "runner.hpp"
#include "jsonConfig.hpp"

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

   	string exePath = r.getString("exePath");

	stringstream exe;
	exe << exePath << "/" << r.getString("run");
	
	runExe(exe.str().c_str());

}
