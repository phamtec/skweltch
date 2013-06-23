#include "zhelpers.hpp"

#include <iostream>
#include <fstream>

using namespace std;

int main (int argc, char *argv[])
{
	if (argc != 2) {
		cerr << "usage: " << argv[0] << " taskId" << endl;
		return 1;
	}
	
    zmq::context_t context(1);

    //  Socket to receive messages on
    zmq::socket_t receiver(context, ZMQ_PULL);
    receiver.connect("tcp://localhost:5557");

    //  Socket to send messages to
    zmq::socket_t sender(context, ZMQ_PUSH);
    sender.connect("tcp://localhost:5558");

	stringstream outfn;
	outfn << "task" << argv[1] << ".out";
	ofstream outfile(outfn.str().c_str());

    //  Process tasks forever
    while (1) {

        zmq::message_t message;
        int workload;           //  Workload in msecs

        receiver.recv(&message);

        std::istringstream iss(static_cast<char*>(message.data()));
        iss >> workload;

        //  Do the work
        s_sleep(workload);

        //  Send results to sink
        message.rebuild();
        sender.send(message);

        //  Simple progress indicator for the viewer
        outfile << "." << std::flush;
    }
    
    return 0;
}
