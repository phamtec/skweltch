#include "Vent.hpp"
#include "Messager.hpp"
#include "JsonConfig.hpp"

using namespace std;
using namespace boost;

void Vent::service(JsonNode *root) {

 	int count = root->getInt("count", 10);

	work->init();

    //  Send count tasks
    int task_nbr;
    for (task_nbr = 0; task_nbr < count; task_nbr++) {
    	zmq::message_t message(2);
    	work->buildMessage(task_nbr, &message);
    	msg->send(&message);
    }
    
    sleep (1); //  Give 0MQ time to deliver
    
}
