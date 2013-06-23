#include "runner.hpp"

using namespace boost;
using namespace std;

int main (int argc, char *argv[])
{

	property_tree::ptree pt;
	readConfig(argv[1], &pt);

	runExe(pt.get<string>("run"));

}
