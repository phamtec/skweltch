#include "JsonConfig.hpp"
#include "StopTasksFileTask.hpp"
#include "FileProcessor.hpp"
#include "ExeRunner.hpp"
#include "Ports.hpp"
#include <zmq.hpp>
#include <iostream>
#include <fstream>
#include <czmq.h>
#include <zclock.h>

using namespace std;

int main (int argc, char *argv[])
{

	ofstream outfile("reap.out");

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

 	int totaltime = root.getInt("totalTime", 5000);
 	zclock_sleep(totaltime);
 	outfile << "waited " << totaltime << "ms, killing everything." << std::endl;
 	
   	ExeRunner er;
	StopTasksFileTask t(&er);
	FileProcessor fp(&t);
	fp.processFileIfExistsThenDelete(root.getString("pidFile"));

}
