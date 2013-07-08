#include "JsonConfig.hpp"
#include "Ports.hpp"
#include <zmq.hpp>
#include <iostream>
#include <fstream>
#include <sys/time.h>

using namespace std;

int main (int argc, char *argv[])
{
	ofstream outfile("sink.out");

 	if (argc != 3) {
		outfile << "usage: " << argv[0] << " pipes config" << endl;
		return 1;
	}

 	JsonObject pipes;
 	{
 		stringstream ss(argv[1]);
 		JsonConfig json(&ss);
		if (!json.read(&pipes, &outfile)) {
			return 1;
		}
 	}
	JsonObject root;
 	{
 		stringstream ss(argv[2]);
 		JsonConfig json(&ss);
		if (!json.read(&root, &outfile)) {
			return 1;
		}
 	}
 	
 	Ports ports;
 	string pullfrom = ports.getBindSocket(pipes, root, "pullFrom");
	
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    receiver.bind(pullfrom.c_str());

	// expected number of messages.
 	int expect = root.getInt("expect", 100);
    outfile << "Expecting: " << expect << endl;

    //  Wait for start of batch
    zmq::message_t message;
    receiver.recv(&message);

    //  Start our clock now
    struct timeval tstart;
    gettimeofday (&tstart, NULL);

    //  Process expected confirmations
    int total = 0;
    for (int i = 0; i < expect; i++) {
    	receiver.recv(&message);    	
		int n;
		std::istringstream iss(static_cast<char*>(message.data()));
		iss >> n;
    	total += n;
    }
    outfile << "Finished." << endl;

    //  Calculate and report duration of batch
    struct timeval tend, tdiff;
    gettimeofday(&tend, NULL);
    if (tend.tv_usec < tstart.tv_usec) {
        tdiff.tv_sec = tend.tv_sec - tstart.tv_sec - 1;
        tdiff.tv_usec = 1000000 + tend.tv_usec - tstart.tv_usec;
    }
    else {
        tdiff.tv_sec = tend.tv_sec - tstart.tv_sec;
        tdiff.tv_usec = tend.tv_usec - tstart.tv_usec;
    }
    int total_msec = tdiff.tv_sec * 1000 + tdiff.tv_usec / 1000;

	// get results.
	JsonObject result;
	result.add("average", ((double)total / (double)expect));
	result.add("elapsed", total_msec);
	{
		ofstream results("results.json");
		result.write(true, &results);
    }
    
}
