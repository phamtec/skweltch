#include "JsonConfig.hpp"
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
		log::add_file_log(log::keywords::file_name = "log/reap.log", log::keywords::auto_flush = true);
		BOOST_LOG_TRIVIAL(error) << "usage: " << argv[0] << " pipes config name";
		return 1;
	}
	
	stringstream outfn;
	outfn << "log/" << argv[3] << ".log";
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

    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
 
 	Ports ports;
    ports.join(&receiver, pipes, root, "pullFrom");

 	int totaltime = root.getInt("totalTime", 5000);
 	zclock_sleep(totaltime);
 	BOOST_LOG_TRIVIAL(info) << "waited " << totaltime << "ms, killing everything.";
 	
	string filename(root.getString("pidFile"));
	ifstream pidfile(filename.c_str());	
	if (pidfile.is_open()) {
		string line;
		while (getline(pidfile, line)) {
			pid_t pid = lexical_cast<int>(line);
			::kill(pid, SIGKILL);
		}
		pidfile.close();
		remove(filename.c_str());
	}

}
