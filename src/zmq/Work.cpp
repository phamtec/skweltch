#include "Work.hpp"
#include "JsonConfig.hpp"

using namespace std;
using namespace boost;

void Work::service(JsonNode *root, zmq::i_socket_t *receiver, zmq::i_socket_t *sender, bool forever) {

	work->init();

    //  Process tasks forever
    while (1) {

        zmq::message_t message;
        receiver->recv(&message);

		work->doit(&message);

        //  Send results to sink
        message.rebuild();
        sender->send(message);
 		
 		if (!forever) {
 			break;
 		}
    }
    
}
