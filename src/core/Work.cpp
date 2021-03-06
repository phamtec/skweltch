
#include "Work.hpp"

#include "IWorkWorker.hpp"
#include "SinkMsg.hpp"
#include "KillMsg.hpp"
#include "Poller.hpp"

#include <zmq.hpp>
#include <msgpack.hpp>

using namespace std;

void Work::process(IWorkWorker *worker) {
	
   	//  Process tasks forever
    while (!worker->shouldQuit()) {

		int recved = Poller::NONE;
		try {
			recved = poller->poll(receiver, control, worker->getTimeout());
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

        if (recved == Poller::MSG) {
		
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
        else if (recved == Poller::CONTROL) {
        	
        	zmq::message_t message;
			try {
	       		control->recv(&message);
	       		KillMsg kill(message);
	       		if (kill.value() == 0) {	       		
					return;
	       		}
			}
			catch (zmq::error_t &e) {
				if (string(e.what()) != "Interrupted system call") {
					LOG4CXX_ERROR(logger, "recv failed." << e.what())
				}
			}
			
        }
        else if (recved == Poller::NONE) {
            LOG4CXX_INFO(logger, "No messages after " << worker->getTimeout() << " seconds.")
			// there was a timeout, give the worker a chance to send on anyway.
			worker->timeout(this);
		}
		
		if (worker->shouldQuit()) {
			LOG4CXX_INFO(logger,  "interrupt sender, killing server")
 			break;
		}
    }
}

void SinkWorker::sendSinkMsg(SinkMsg *smsg) {
    
	zmq::message_t smessage;
    
    // Send results to sink
	smsg->set(&smessage);
    
	try {
		sender->send(smessage);
	}
	catch (zmq::error_t &e) {
		if (string(e.what()) != "Interrupted system call") {
			LOG4CXX_ERROR(logger, "send failed." << e.what())
		}
	}
    
}

