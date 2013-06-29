#include "JsonConfig.hpp"
#include "Sink.hpp"
#include <zmq.hpp>
#include <iostream>
#include <fstream>

using namespace std;

int main (int argc, char *argv[])
{

 	if (argc != 2) {
		cerr << "usage: " << argv[0] << " config" << endl;
		return 1;
	}
	
	ofstream outfile("sink.out");

  	JsonNode root;
 	{
 		stringstream ss(argv[1]);
 		JsonConfig json(&ss);
 		json.read(&root);
 	}
 	string pullfrom = root.getString("pullFrom");
	
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t receiver(context, ZMQ_PULL);
    receiver.bind(pullfrom.c_str());

	Sink s(&outfile);
	s.service(&root, &receiver);

}
