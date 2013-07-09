#include "JsonConfig.hpp"
#include "Ports.hpp"
#include <zmq.hpp>
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>

using namespace std;
using namespace boost;

int main (int argc, char *argv[])
{
	log::add_file_log(log::keywords::file_name = "sink.log", log::keywords::auto_flush = true);
	
 	if (argc != 3) {
		BOOST_LOG_TRIVIAL(error) << "usage: " << argv[0] << " pipes config";
		return 1;
	}
	
 	JsonObject pipes;
 	{
 		stringstream ss(argv[1]);
 		JsonConfig json(&ss);
		if (!json.read(&pipes)) {
			return 1;
		}
 	}
	JsonObject root;
 	{
 		stringstream ss(argv[2]);
 		JsonConfig json(&ss);
		if (!json.read(&root)) {
			return 1;
		}
 	}
 	
 	Ports ports;
 	string pullfrom = ports.getBindSocket(pipes, root, "pullFrom");
	
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    receiver.bind(pullfrom.c_str());

 	int expect = root.getInt("expect", 100);
    BOOST_LOG_TRIVIAL(info) << "Expecting: " << expect;

    //  Wait for start of batch
    zmq::message_t message;
    receiver.recv(&message);

    //  Start our clock now
    struct timeval tstart;
    gettimeofday (&tstart, NULL);

    //  Process expected confirmations
    for (int i = 0; i < expect; i++) {
    	receiver.recv(&message);	
    }
    BOOST_LOG_TRIVIAL(info) << "Finished.";
    
    //  Calculate and report duration of batch
    struct timeval tend, tdiff;
    gettimeofday (&tend, NULL);

    if (tend.tv_usec < tstart.tv_usec) {
        tdiff.tv_sec = tend.tv_sec - tstart.tv_sec - 1;
        tdiff.tv_usec = 1000000 + tend.tv_usec - tstart.tv_usec;
    }
    else {
        tdiff.tv_sec = tend.tv_sec - tstart.tv_sec;
        tdiff.tv_usec = tend.tv_usec - tstart.tv_usec;
    }
    int total_msec = tdiff.tv_sec * 1000 + tdiff.tv_usec / 1000;

	// get results.
	JsonObject result;
	result.add("elapsed", total_msec);
	{
		ofstream results("results.json");
		result.write(true, &results);
    }
}
