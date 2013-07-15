
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
#include <msgpack.hpp>

using namespace std;
using namespace boost;

int main (int argc, char *argv[])
{
 	if (argc != 4) {
		log::add_file_log(log::keywords::file_name = "log/pipe.log", log::keywords::auto_flush = true);
		BOOST_LOG_TRIVIAL(error) << "usage: " << argv[0] << " pipes config name";
		return 1;
	}
	
	stringstream outfn;
	outfn << "log/" << argv[3] << ".log";
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

 	int count = root.getInt("every", 100);

	zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    zmq::socket_t sender(context, ZMQ_PUSH);

 	Ports ports;
    ports.join(&receiver, pipes, root, "pipeFrom");
    ports.join(&sender, pipes, root, "pipeTo");

	BOOST_LOG_TRIVIAL(info) << "start...";
	
    //  Process messages forever
    int n=0;
    while (1) {

        zmq::message_t message;
        receiver.recv(&message);
        
        if ((n % count) == 0) {
			msgpack::unpacked msg;
			msgpack::unpack(&msg, (const char *)message.data(), message.size());
			msgpack::object obj = msg.get();
            BOOST_LOG_TRIVIAL(info) << "..." << obj << "...";
        }
        n++;
        
		sender.send(message);
		
     }
    
    return 0;

}
