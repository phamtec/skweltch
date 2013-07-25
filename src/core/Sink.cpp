
#include "Sink.hpp"

#include "ISinkWorker.hpp"
#include "Elapsed.hpp"
#include "SinkMsg.hpp"
#include "MsgTracker.hpp"

#include <zmq.hpp>
#include <msgpack.hpp>
#include <boost/dynamic_bitset.hpp>

using namespace std;
using namespace boost;

bool Sink::process(ISinkWorker *worker) {

	Elapsed elapsed;
	MsgTracker tracker(logger);
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
			LOG4CXX_INFO(logger, "interrupt received, killing server")
			break;
		}

		SinkMsg sinkmsg(message);
		switch (sinkmsg.getCode()) {
		case 1:
			worker->first(sinkmsg.getId());
			tracker.setFirst(sinkmsg.getId());
			elapsed.start();
			break;
			
		case 2:

			worker->process(sinkmsg.getId(), sinkmsg.getData());
			
			// mark this message off.
			tracker.track(sinkmsg.getId());
			
   			LOG4CXX_DEBUG(logger, "msg: " << sinkmsg.getId())
			
 			if (tracker.complete()) {
			
				LOG4CXX_INFO(logger, "Finished.")

				 //  Calculate and report duration of batch
				int total_msec = elapsed.getTotal();

				// get results.
				worker->results(total_msec);
				LOG4CXX_INFO(logger, "Results written.")
			
				// start tracking from the start.
				tracker.reset();
			}
			break;
			
		case 3:
			// this one usually comes from a vent.
			worker->last(sinkmsg.getId());
			tracker.setLast(sinkmsg.getId());
			break;
			
		default:
  			LOG4CXX_ERROR(logger, "Unknown message: " << sinkmsg.getCode())
   			return false;
		}
	}
	
	if (!tracker.complete()) {
   		LOG4CXX_INFO(logger, "Didn't get all the messages.")
	}
	
   	LOG4CXX_INFO(logger, "finished.")
	return true;
	
}

