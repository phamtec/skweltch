#include "runner.hpp"

using namespace boost;
using namespace std;

int main (int argc, char *argv[])
{

	property_tree::ptree pt;
	readConfig(argv[1], &pt);

   	string exePath = pt.get<string>("exePath");

	stringstream exe;
	exe << exePath << "/" << pt.get<string>("run");
	
	runExe(exe.str().c_str());

}
