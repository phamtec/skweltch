
#include "JsonConfig.hpp"
#include "Ports.hpp"
#include <iostream>
#include <fstream>
#include <zmq.hpp>
#include <czmq.h>
#include <zclock.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <msgpack.hpp>

#define within(num) (int) ((float) num * random () / (RAND_MAX + 1.0))

using namespace std;
using namespace boost;

int main (int argc, char *argv[])
{
	
 	if (argc != 4) {
		log::add_file_log(log::keywords::file_name = "vent.log", log::keywords::auto_flush = true);
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
 	string syncto = ports.getConnectSocket(pipes, root, "syncTo");
 	string pushto = ports.getBindSocket(pipes, root, "pushTo");

	int low = root.getInt("low", 1);
 	int high = root.getInt("high", 100);
	
	zmq::context_t context(1);
    zmq::socket_t sender(context, ZMQ_PUSH);
    sender.bind(pushto.c_str());

	zmq::socket_t sink(context, ZMQ_PUSH);
	sink.connect(syncto.c_str());

	// pack the number up and send it.
    zmq::message_t message(2);
	msgpack::sbuffer sbuf;
	msgpack::pack(sbuf, 1);
	message.rebuild(sbuf.size());
	memcpy(message.data(), sbuf.data(), sbuf.size());
    sink.send(message);

 	int count = root.getInt("count", 10);
 	int sleeptime = root.getInt("sleep", 0);

    //  Initialize random number generator
    srandom ((unsigned) time (NULL));

    //  Send count tasks
    for (int i = 0; i < count; i++) {
    
    	zmq::message_t message(2);
    	
		//  Random number from 1 to the range specified
		int num = within(high) + low;

		// pack the number up and send it.
		msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, num);
 		message.rebuild(sbuf.size());
		memcpy(message.data(), sbuf.data(), sbuf.size());
 
     	sender.send(message);
     	
     	if (sleeptime > 0) {
			//  Do the work
			zclock_sleep(sleeptime);
     	}
    }
    
    sleep (1); //  Give 0MQ time to deliver
    
    return 0;
}
