
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
	
public:
	SendWord(zmq::socket_t *s, int slp) : sender(s), sleeptime(slp) {}
	
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
	SendWord w(&sender, sleeptime);
	WordSplitter splitter(&f);
	splitter.process(&w);
/*	
    //  Send count tasks
    int task_nbr;
    for (task_nbr = 0; task_nbr < count; task_nbr++) {
    	zmq::message_t message(2);
    	
		//  Random number from 1 to the range specified
		int num = within(high) + low;

		// maximum length the number can be. Just fixed size messages.
		message.rebuild(10);
		sprintf((char *)message.data(), "%d", num);
 
     	sender.send(message);
     	
     	if (sleeptime > 0) {
			//  Do the work
			zclock_sleep(sleeptime);
     	}
    }
 */   
    sleep (1); //  Give 0MQ time to deliver
    
    return 0;
}
