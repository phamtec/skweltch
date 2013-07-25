
#include "Work.hpp"

#include "IWorkWorker.hpp"
#include "SinkMsg.hpp"

#include <zmq.hpp>
#include <msgpack.hpp>

using namespace std;

void Work::process(IWorkWorker *worker) {
	
   	//  Process tasks forever
    while (!worker->shouldQuit()) {

        zmq::message_t message;
 		try {
       		receiver->recv(&message);
		}
		catch (zmq::error_t &e) {
			if (string(e.what()) != "Interrupted system call") {
				LOG4CXX_ERROR(logger, "recv failed." << e.what())
			}
		}
		
		if (worker->shouldQuit()) {
			LOG4CXX_INFO(logger,  "interrupt received, killing server")
			break;
		}

        // do the work. Takes in the object passed in and generates a sinkmsg.
		SinkMsg smsg;
        worker->process(message, &smsg);
    
		LOG4CXX_DEBUG(logger,  "sending msg: " << smsg.getId())
 
         // Send results to sink
		smsg.set(&message);
		try {
 			sender->send(message);
 		}
		catch (zmq::error_t &e) {
			if (string(e.what()) != "Interrupted system call") {
				LOG4CXX_ERROR(logger, "send failed." << e.what())
			}
		}
		if (worker->shouldQuit()) {
			LOG4CXX_INFO(logger,  "interrupt sender, killing server")
 			break;
		}
    }
    
	LOG4CXX_INFO(logger, "finished.")

}

