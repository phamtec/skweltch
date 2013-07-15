
#include "TaskMonitor.hpp"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>

using namespace std;
using namespace boost;

int main (int argc, char *argv[])
{
	log::add_file_log(log::keywords::file_name = "monitor.log", log::keywords::auto_flush = true);
	
	if (argc != 2) {
		BOOST_LOG_TRIVIAL(error) << "usage: " << argv[0] << " jsonConfig";
		return 1;
	}
	
	TaskMonitor mon;
	return mon.start(argv[1]);
	
}
