
#include "JsonConfig.hpp"
#include "Vent.hpp"
#include "Messager.hpp"
#include <iostream>
#include <fstream>

#define within(num) (int) ((float) num * random () / (RAND_MAX + 1.0))

using namespace std;

class RandomMsgBuilder : public IVentMsgBuilder {

private:
 	int low, high;
	
public:
	RandomMsgBuilder(int l, int h) : low(l), high(h) {}
	
	virtual void init();
	virtual void buildMessage(int n, zmq::message_t *message);

};

void RandomMsgBuilder::init() {

    //  Initialize random number generator
    srandom ((unsigned) time (NULL));
}

void RandomMsgBuilder::buildMessage(int n, zmq::message_t *message) {

	//  Random number from 1 to the range specified
	int num = within(high) + low;

	// maximum length the number can be. Just fixed size messages.
	message->rebuild(10);
	sprintf((char *)message->data(), "%d", num);

}

int main (int argc, char *argv[])
{
	if (argc != 2) {
		cerr << "usage: " << argv[0] << " config" << endl;
		return 1;
	}
	
 	ofstream outfile("vent.out");
 	
 	JsonNode root;
 	{
 		stringstream ss(argv[1]);
 		JsonConfig json(&ss);
 		json.read(&root);
 	}

  	string pushto = root.getString("pushTo");
 	string syncto = root.getString("syncTo");
	int low = root.getInt("low", 1);
 	int high = root.getInt("high", 100);
	
	zmq::context_t context(1);
    zmq::socket_t sender(context, ZMQ_PUSH);
    sender.bind(pushto.c_str());

    zmq::socket_t sink(context, ZMQ_PUSH);
    sink.connect(syncto.c_str());
    zmq::message_t message(2);
    memcpy(message.data(), "0", 1);
    sink.send(message);

	ZmQMessager msg;
	msg.setSender(&sender);
	msg.setReceiver(&sink);
	
	RandomMsgBuilder vrn(low, high);
	Vent v(&msg, &vrn);
	v.service(&root);

    return 0;
}
