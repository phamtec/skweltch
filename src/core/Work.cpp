
#include "Work.hpp"

#include "IWorkWorker.hpp"
#include "SinkMsg.hpp"
#include "Poller.hpp"

#include <zmq.hpp>
#include <msgpack.hpp>

using namespace std;

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
			
            worker->processMsg(message);
		}
		else {
			// there was a timeout, give the worker a chance to send on anyway.
			worker->timeout(this);
		}
		
		if (worker->shouldQuit()) {
			LOG4CXX_INFO(logger,  "interrupt sender, killing server")
 			break;
		}
    }
    
	LOG4CXX_INFO(logger, "finished.")

}
