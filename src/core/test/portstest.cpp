#define BOOST_TEST_MODULE smoke-tests
#include <boost/test/unit_test.hpp>

#include "JsonConfig.hpp"
#include "../Ports.hpp"

#include <turtle/mock.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace boost;

BOOST_AUTO_TEST_CASE( pidFileExistsTest )
{
	string roots = "{\"connections\":{\"pullFrom\":{\"pipe\":\"pipe2\",\"direction\":\"from\"},\"pushTo\":{\"pipe\":\"pipe1\",\"direction\":\"to\"}}}";
	string pipess = "{\"pipe1\":{\"node\":\"localhost\",\"port\":\"5558\"},\"pipe2\":{\"node\":\"localhost\",\"port\":\"5557\"},\"pipe3\":{\"node\":\"localhost\",\"port\":\"5559\"}}";

  	boost::property_tree::ptree pipes;
 	{
 		stringstream ss(pipess);
 		JsonConfig json(&ss);
		BOOST_CHECK(json.read(&pipes, &cout));
 	}
	boost::property_tree::ptree root;
 	{
 		stringstream ss(roots);
 		JsonConfig json(&ss);
		BOOST_CHECK(json.read(&root, &cout));
 	}
 	
	Ports p;
	cout << p.getBindSocket(pipes, root, "pullFrom") << endl;
	BOOST_CHECK(p.getBindSocket(pipes, root, "pullFrom") == "tcp://*:5557");
}
