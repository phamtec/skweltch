
#include "JsonConfig.hpp"
#include "Ports.hpp"
#include <zmq.hpp>
#include <czmq.h>
#include <zclock.h>

#include <iostream>
#include <fstream>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>

using namespace std;
using namespace boost;

int main (int argc, char *argv[])
{
	if (argc != 4) {
		log::add_file_log(log::keywords::file_name = "work.log", log::keywords::auto_flush = true);
		BOOST_LOG_TRIVIAL(error) << "usage: " << argv[0] << " taskId pipes config";
		return 1;
	}
	
	stringstream outfn;
	outfn << "work" << argv[1] << ".log";
	log::add_file_log(log::keywords::file_name = outfn.str(), log::keywords::auto_flush = true);
	
	JsonObject pipes;
 	{
 		stringstream ss(argv[2]);
 		JsonConfig json(&ss);
		if (!json.read(&pipes)) {
			return 1;
		}
 	}
	JsonObject root;
 	{
 		stringstream ss(argv[3]);
 		JsonConfig json(&ss);
		if (!json.read(&root)) {
			return 1;
		}
 	}

 	Ports ports;
 	string pullfrom = ports.getConnectSocket(pipes, root, "pullFrom");
 	string pushto = ports.getConnectSocket(pipes, root, "pushTo");

	zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    receiver.connect(pullfrom.c_str());
    
    zmq::socket_t sender(context, ZMQ_PUSH);
    sender.connect(pushto.c_str());

    //  Process tasks forever
    int n=0;
    while (1) {

        zmq::message_t message;
        receiver.recv(&message);

		string word;
		std::istringstream iss(static_cast<char*>(message.data()));
		iss >> word;
		
         if ((n % 100) == 0)
           BOOST_LOG_TRIVIAL(info) << "..." << word << "...";
        n++;
        
		int count = word.length();

        // Send results to sink
		// maximum length the number can be. Just fixed size messages.
		message.rebuild(10);
		sprintf((char *)message.data(), "%d", count);
        sender.send(message);

    }
    
    return 0;

}
