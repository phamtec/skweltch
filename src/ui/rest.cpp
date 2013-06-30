
#include "server.hpp"

#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <fstream>

using namespace std;
using namespace boost;

int main(int argc, char* argv[])
{
     // Check command line arguments.
    if (argc != 4)
    {
      std::cerr << "Usage: rest <address> <port> <doc_root>\n";
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
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}
