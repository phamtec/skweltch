#include "JsonConfig.hpp"
#include <zmq.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

#define within(num) (int) ((float) num * random () / (RAND_MAX + 1.0))

using namespace std;

int main (int argc, char *argv[])
{
	if (argc != 2) {
		cerr << "usage: " << argv[0] << " config" << endl;
		return 1;
	}
	
 	ofstream outfile("vent.out");
 	
 	string pushto, syncto;
 	{
 		stringstream ss(argv[1]);
 		JsonConfig json(&ss);
 		JsonNode r;
 		json.read(&r);
 		pushto = r.getString("pushTo");
 		syncto = r.getString("syncTo");
 	}
	
   	zmq::context_t context (1);

    //  Socket to send messages on
    zmq::socket_t  sender(context, ZMQ_PUSH);
    sender.bind(pushto.c_str());

    //  The first message is "0" and signals start of batch
    zmq::socket_t sink(context, ZMQ_PUSH);
    sink.connect(syncto.c_str());
    zmq::message_t message(2);
    memcpy(message.data(), "0", 1);
    sink.send(message);

    //  Initialize random number generator
    srandom ((unsigned) time (NULL));

    //  Send 100 tasks
    int task_nbr;
    int total_msec = 0;     //  Total expected cost in msecs
    for (task_nbr = 0; task_nbr < 100; task_nbr++) {
        int workload;
        //  Random workload from 1 to 100msecs
        workload = within (100) + 1;
        total_msec += workload;

        message.rebuild(10);
        sprintf ((char *) message.data(), "%d", workload);
        sender.send(message);
    }
    outfile << "Total expected cost: " << total_msec << " msec" << std::endl;
    sleep (1);              //  Give 0MQ time to deliver

    return 0;
}
