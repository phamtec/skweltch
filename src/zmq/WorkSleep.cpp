
#include "Work.hpp"
#include "Messager.hpp"
#include "JsonConfig.hpp"
#include "zhelpers.hpp"

#include <iostream>
#include <fstream>

using namespace std;

class SleepMsgExecutor : public IWorkMsgExecutor {

private:
	ofstream *outfile;
	
public:
	SleepMsgExecutor(ofstream *f) : outfile(f) {}
	
	virtual void init() {}
	virtual void doit(zmq::message_t *message);

};

void SleepMsgExecutor::doit(zmq::message_t *message) {

	int workload;
	std::istringstream iss(static_cast<char*>(message->data()));
	iss >> workload;

	//  Do the work
	s_sleep(workload);

	*outfile << "." << std::flush;
	
}

int main (int argc, char *argv[])
{
	if (argc != 3) {
		cerr << "usage: " << argv[0] << " taskId config" << endl;
		return 1;
	}
	
	stringstream outfn;
	outfn << "task" << argv[1] << ".out";
	ofstream outfile(outfn.str().c_str());
	
 	JsonNode root;
 	string pullfrom, pushto;
 	{
 		stringstream ss(argv[2]);
 		JsonConfig json(&ss);
 		json.read(&root);
 	}
 	pullfrom = root.getString("pullFrom");
 	pushto = root.getString("pushTo");
	
	zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    receiver.connect(pullfrom.c_str());
    
    zmq::socket_t sender(context, ZMQ_PUSH);
    sender.connect(pushto.c_str());

	ZmQMessager msg;
	msg.setSender(&sender);
	msg.setReceiver(&receiver);
	
	SleepMsgExecutor e(&outfile);
	Work w(&msg, &e);
	w.service(&root);

    return 0;

}
