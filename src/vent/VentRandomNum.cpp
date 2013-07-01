
#include "JsonConfig.hpp"
#include "JsonNode.hpp"
#include <iostream>
#include <fstream>
#include <zmq.hpp>
#include <czmq.h>
#include <zclock.h>

#define within(num) (int) ((float) num * random () / (RAND_MAX + 1.0))

using namespace std;

int main (int argc, char *argv[])
{
	if (argc != 2) {
		cerr << "usage: " << argv[0] << " config" << endl;
		return 1;
	}
	
 	ofstream outfile("vent.out");
 	
 	JsonNode root;
 	{
 		stringstream ss(argv[1]);
 		JsonConfig json(&ss);
		if (!json.read(&root, &std::cout)) {
			return 1;
		}
 	}

  	string pushto = root.getString("pushTo");
 	string syncto = root.getString("syncTo");
	int low = root.getInt("low", 1);
 	int high = root.getInt("high", 100);
	
	zmq::context_t context(1);
    zmq::socket_t sender(context, ZMQ_PUSH);
    sender.bind(pushto.c_str());

    zmq::socket_t sink(context, ZMQ_PUSH);
    sink.connect(syncto.c_str());
    zmq::message_t message(2);
    memcpy(message.data(), "0", 1);
    sink.send(message);

 	int count = root.getInt("count", 10);
 	int sleeptime = root.getInt("sleep", 0);

    //  Initialize random number generator
    srandom ((unsigned) time (NULL));

    //  Send count tasks
    int task_nbr;
    for (task_nbr = 0; task_nbr < count; task_nbr++) {
    	zmq::message_t message(2);
    	
		//  Random number from 1 to the range specified
		int num = within(high) + low;

		// maximum length the number can be. Just fixed size messages.
		message.rebuild(10);
		sprintf((char *)message.data(), "%d", num);
 
     	sender.send(message);
     	
     	if (sleeptime > 0) {
			//  Do the work
			zclock_sleep(sleeptime);
     	}
    }
    
    sleep (1); //  Give 0MQ time to deliver
    
    return 0;
}
