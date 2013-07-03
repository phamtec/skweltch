
#include "JsonConfig.hpp"
#include "Ports.hpp"
#include <zmq.hpp>
#include <czmq.h>
#include <zclock.h>

#include <iostream>
#include <fstream>

using namespace std;

int main (int argc, char *argv[])
{

	if (argc != 4) {
		cerr << "usage: " << argv[0] << " taskId pipes config" << endl;
		return 1;
	}
	
	stringstream outfn;
	outfn << "task" << argv[1] << ".out";
	ofstream outfile(outfn.str().c_str());
	
  	boost::property_tree::ptree pipes;
 	{
 		stringstream ss(argv[2]);
 		JsonConfig json(&ss);
		if (!json.read(&pipes, &outfile)) {
			return 1;
		}
 	}
	boost::property_tree::ptree root;
 	{
 		stringstream ss(argv[3]);
 		JsonConfig json(&ss);
		if (!json.read(&root, &outfile)) {
			return 1;
		}
 	}

 	Ports ports;
 	string pullfrom = ports.getConnectSocket(pipes, root, "pullFrom");
 	string pushto = ports.getConnectSocket(pipes, root, "pushTo");

   	cout << "here" << endl;
 	return 1;
	
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
