#include "Work.hpp"
#include "Messager.hpp"
#include "JsonConfig.hpp"

using namespace std;
using namespace boost;

void Work::service(JsonNode *root, bool forever) {

	work->init();

    //  Process tasks forever
    while (1) {

        zmq::message_t message;
        msg->receive(&message);

		work->doit(&message);

        //  Send results to sink
        message.rebuild();
 		msg->send(&message);
 		
 		if (!forever) {
 			break;
 		}
    }
    
}
