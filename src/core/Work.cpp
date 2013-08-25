
#include "Work.hpp"

#include "IWorkWorker.hpp"
#include "SinkMsg.hpp"

#include <zmq.hpp>
#include <msgpack.hpp>

using namespace std;

bool Poller::poll(zmq::i_socket_t *socket, int timeout) {

	void *addr = *socket;
	LOG4CXX_DEBUG(logger, addr)
	
	zmq::pollitem_t items[] = { { addr, 0, ZMQ_POLLIN, 0 } };
 	zmq::poll (&items[0], 1, timeout);
 	
	LOG4CXX_DEBUG(logger, "events: " << items[0].events)
	LOG4CXX_DEBUG(logger, "revents: " << items[0].revents)
 	
 	return items[0].revents & ZMQ_POLLIN;
 	
}

void Work::process(IWorkWorker *worker) {
	
   	//  Process tasks forever
    while (!worker->shouldQuit()) {

		bool recved = false;
		try {
			recved = poller->poll(receiver, worker->getTimeout());
		}
		catch (zmq::error_t &e) {
			if (string(e.what()) != "Interrupted system call") {
				LOG4CXX_ERROR(logger, "poll failed." << e.what())
			}
		}
		
		if (worker->shouldQuit()) {
			LOG4CXX_INFO(logger,  "interrupt received, killing server")
			break;
		}

		if (recved) {
		
       		zmq::message_t message;
			try {
	       		receiver->recv(&message);
			}
			catch (zmq::error_t &e) {
				if (string(e.what()) != "Interrupted system call") {
					LOG4CXX_ERROR(logger, "recv failed." << e.what())
				}
			}
			
			// do the work. Takes in the object passed in and generates a sinkmsg.
			SinkMsg smsg;
			worker->process(message, &smsg);
	
			LOG4CXX_TRACE(logger,  "sending msg: " << smsg.getId())
 
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
			
		}
		else {
			worker->timeout();
			LOG4CXX_DEBUG(logger,  "timeout, asking again.")
		}
		
		if (worker->shouldQuit()) {
			LOG4CXX_INFO(logger,  "interrupt sender, killing server")
 			break;
		}
    }
    
	LOG4CXX_INFO(logger, "finished.")

}

