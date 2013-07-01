#include "JsonConfig.hpp"
#include "JsonNode.hpp"
#include "StopTasksFileTask.hpp"
#include "FileProcessor.hpp"
#include "ExeRunner.hpp"
#include <zmq.hpp>
#include <iostream>
#include <fstream>
#include <czmq.h>
#include <zclock.h>

using namespace std;

int main (int argc, char *argv[])
{

 	if (argc != 2) {
		cerr << "usage: " << argv[0] << " config" << endl;
		return 1;
	}
	
	ofstream outfile("reap.out");

  	JsonNode root;
 	{
 		stringstream ss(argv[1]);
 		JsonConfig json(&ss);
		if (!json.read(&root, &std::cout)) {
			return 1;
		}
 	}
 	 	
 	string pullfrom = root.getString("pullFrom");
	
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
