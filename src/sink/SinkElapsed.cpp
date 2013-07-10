#include "JsonConfig.hpp"
#include "Ports.hpp"
#include "Elapsed.hpp"
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
 	if (argc != 4) {
		log::add_file_log(log::keywords::file_name = "sink.log", log::keywords::auto_flush = true);
		BOOST_LOG_TRIVIAL(error) << "usage: " << argv[0] << " pipes config name";
		return 1;
	}
	
	stringstream outfn;
	outfn << argv[3] << ".log";
	log::add_file_log(log::keywords::file_name = outfn.str(), log::keywords::auto_flush = true);
	
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

    //  Wait for start of batch. It's an empty message.
    zmq::message_t message;
    receiver.recv(&message);

    //  Start our clock now
    Elapsed elapsed;

    //  Process expected confirmations
    for (int i = 0; i < expect; i++) {
    	receiver.recv(&message);	
    }
    BOOST_LOG_TRIVIAL(info) << "Finished.";
    
     //  Calculate and report duration of batch
    int total_msec = elapsed.getTotal();

	// get results.
	JsonObject result;
	result.add("elapsed", total_msec);
	{
		ofstream results("results.json");
		result.write(true, &results);
    }
}
