
#include "JsonConfig.hpp"
#include "JsonNode.hpp"
#include <zmq.hpp>
#include <czmq.h>
#include <zclock.h>

#include <iostream>
#include <fstream>

using namespace std;

int main (int argc, char *argv[])
{
	if (argc != 3) {
		cerr << "usage: " << argv[0] << " taskId config" << endl;
		return 1;
	}
	
	stringstream outfn;
	outfn << "task" << argv[1] << ".out";
	ofstream outfile(outfn.str().c_str());
	
 	JsonNode root;
 	string pullfrom, pushto;
 	{
 		stringstream ss(argv[2]);
 		JsonConfig json(&ss);
		if (!json.read(&root, &std::cout)) {
			return 1;
		}
 	}
 	
 	pullfrom = root.getString("pullFrom");
 	pushto = root.getString("pushTo");
	
	zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    receiver.connect(pullfrom.c_str());
    
    zmq::socket_t sender(context, ZMQ_PUSH);
    sender.connect(pushto.c_str());

    //  Process tasks forever
    while (1) {

        zmq::message_t message;
        receiver.recv(&message);

		int workload;
		std::istringstream iss(static_cast<char*>(message.data()));
		iss >> workload;

		//  Do the work
 		zclock_sleep(workload);

		outfile << "." << std::flush;

        //  Send results to sink
        message.rebuild();
        sender.send(message);

    }
    

    return 0;

}
