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
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>

using namespace std;
using namespace boost;

int main (int argc, char *argv[])
{
 	if (argc != 4) {
		log::add_file_log(log::keywords::file_name = "reap.log", log::keywords::auto_flush = true);
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
 	string pullfrom = ports.getBindSocket(pipes, root, "pullFrom");
	
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    receiver.bind(pullfrom.c_str());

 	int totaltime = root.getInt("totalTime", 5000);
 	zclock_sleep(totaltime);
 	BOOST_LOG_TRIVIAL(info) << "waited " << totaltime << "ms, killing everything.";
 	
   	ExeRunner er;
	StopTasksFileTask t(&er);
	FileProcessor fp(&t);
	fp.processFileIfExistsThenDelete(root.getString("pidFile"));

}
