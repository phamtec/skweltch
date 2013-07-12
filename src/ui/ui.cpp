
#include "server.hpp"

#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <fstream>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>

using namespace std;
using namespace boost;

int main(int argc, char* argv[])
{
	log::add_file_log(log::keywords::file_name = "ui.log", log::keywords::auto_flush = true);
	
	if (argc != 4) {
		BOOST_LOG_TRIVIAL(error) << "Usage: rest <address> <port> <doc_root>\n";
		return 1;
	}
	
	try
	{
	
		// Initialise the server.
		http::server::server s(argv[1], argv[2], argv[3]);

		// Run the server until stopped.
		s.run();

	}
	catch (std::exception& e)
	{
		BOOST_LOG_TRIVIAL(error) << "exception: " << e.what();
	}

	return 0;
}
