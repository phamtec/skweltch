
#include "JsonConfig.hpp"
#include "Ports.hpp"
#include "WordSplitter.hpp"
#include <iostream>
#include <fstream>
#include <zmq.hpp>
#include <czmq.h>
#include <zclock.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>

using namespace std;
using namespace boost;

class SendWord : public IWord {

private:
	zmq::socket_t *sender;
	int sleeptime;
	int n;
	
public:
	SendWord(zmq::socket_t *s, int slp) : sender(s), sleeptime(slp), n(0) {}
	
	virtual void word(const std::string &s);
};

void SendWord::word(const std::string &s) {

	zmq::message_t message(s.length()+1);
	
	// maximum length the number can be. Just fixed size messages.
	strcpy((char *)message.data(), s.c_str());

    if ((n % 100) == 0)
		BOOST_LOG_TRIVIAL(info) << "..." << s << "...";
	n++;
	
	sender->send(message);
     	
	if (sleeptime > 0) {
		//  Do the work
		zclock_sleep(sleeptime);
	}
	
}

int main (int argc, char *argv[])
{
	log::add_file_log(log::keywords::file_name = "vent.log", log::keywords::auto_flush = true);
	
 	if (argc != 3) {
		BOOST_LOG_TRIVIAL(error) << "usage: " << argv[0] << " pipes config";
		return 1;
	}
	
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
 	string syncto = ports.getConnectSocket(pipes, root, "syncTo");
 	string pushto = ports.getBindSocket(pipes, root, "pushTo");

	string filename = root.getString("filename");
 	int sleeptime = root.getInt("sleep", 0);
	
	zmq::context_t context(1);
    zmq::socket_t sender(context, ZMQ_PUSH);
    sender.bind(pushto.c_str());

    zmq::socket_t sink(context, ZMQ_PUSH);
    sink.connect(syncto.c_str());
    zmq::message_t message(2);
    memcpy(message.data(), "0", 1);
    sink.send(message);

	ifstream f(filename.c_str(), ifstream::in);
	if (!f.is_open()) {
		BOOST_LOG_TRIVIAL(info) << " no file " << filename;
		return 1;
	}
	
	SendWord w(&sender, sleeptime);
	WordSplitter splitter(&f);
	splitter.process(&w);
    
	BOOST_LOG_TRIVIAL(info) << "finished.";

    return 0;
}
