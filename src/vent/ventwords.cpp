
#include "JsonConfig.hpp"
#include "Ports.hpp"
#include "WordSplitter.hpp"
#include <iostream>
#include <fstream>
#include <zmq.hpp>
#include <czmq.h>
#include <zclock.h>

using namespace std;

class SendWord : public IWord {

private:
	zmq::socket_t *sender;
	int sleeptime;
	ofstream *outfile;
	int n;
	
public:
	SendWord(zmq::socket_t *s, int slp, ofstream *o) : sender(s), sleeptime(slp), outfile(o), n(0) {}
	
	virtual void word(const std::string &s);
};

void SendWord::word(const std::string &s) {

	zmq::message_t message(s.length()+1);
	
	// maximum length the number can be. Just fixed size messages.
	strcpy((char *)message.data(), s.c_str());

	sender->send(message);
     	
	if (sleeptime > 0) {
		//  Do the work
		zclock_sleep(sleeptime);
	}
	
	if ((n / 100) * 100 == n)
		*outfile << "." << std::flush;
	n++;
	
}

int main (int argc, char *argv[])
{
 	ofstream outfile("vent.out");

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
		outfile << " no file " << filename << endl;
		return 1;
	}
	
	SendWord w(&sender, sleeptime, &outfile);
	WordSplitter splitter(&f);
	splitter.process(&w);
    
	outfile << "finished." << endl;

    return 0;
}
