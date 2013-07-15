
#include "JsonConfig.hpp"
#include "JsonObject.hpp"
#include "JsonPath.hpp"
#include "MachineTuner.hpp"

#include <iostream>
#include <fstream>
#include <czmq.h>
#include <zclock.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;

int main (int argc, char *argv[])
{
	log::add_file_log(log::keywords::file_name = "log/soak.log", log::keywords::auto_flush = true);
	
	if (argc != 4) {
		BOOST_LOG_TRIVIAL(error) << "usage: " << argv[0] << " jsonConfig count iterations";
		return 1;
	}
	
	int count = lexical_cast<int>(argv[2]);
	int iterations = lexical_cast<int>(argv[3]);
		
 	MachineTuner tuner(0, 0);
 	BOOST_LOG_TRIVIAL(info) << "g\ti\tv\tn\tlow\thigh\tfail\tavg\tmed";
 	for (int i=0; i<count; i++) {
		tuner.runOne(argv[1], iterations, 0, 0, "");
	}
	
	return 0;
	
}
