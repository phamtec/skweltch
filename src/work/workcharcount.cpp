
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
	
	JsonObject pipes;
 	{
 		stringstream ss(argv[2]);
 		JsonConfig json(&ss);
		if (!json.read(&pipes, &outfile)) {
			return 1;
		}
 	}
	JsonObject root;
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
		int count = word.length();

        // Send results to sink
		// maximum length the number can be. Just fixed size messages.
		message.rebuild(10);
		sprintf((char *)message.data(), "%d", count);
        sender.send(message);

        if ((n / 10) * 10 == n)
            outfile << "." << std::flush;
        n++;
    }
    
    return 0;

}
