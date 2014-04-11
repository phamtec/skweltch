
#include "Sink.hpp"

#include "ISinkWorker.hpp"
#include "Elapsed.hpp"
#include "SinkMsg.hpp"
#include "MsgTracker.hpp"
#include "Poller.hpp"

#include <zmq.hpp>
#include <msgpack.hpp>
#include <boost/dynamic_bitset.hpp>

using namespace std;
using namespace boost;

bool Sink::process(ISinkWorker *worker) {

 	Elapsed elapsed;
	MsgTracker tracker(logger);
    int retries = 0;
 	while (!worker->shouldQuit()) {
	
 		bool recved = false;
		try {
			recved = poller->poll(receiver, 5000);
		}
		catch (zmq::error_t &e) {
			if (string(e.what()) != "Interrupted system call") {
				LOG4CXX_ERROR(logger, "poll failed." << e.what())
			}
		}
		
        if (recved) {
            retries = 0;
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
                LOG4CXX_DEBUG(logger, "got first: " << sinkmsg.getId())
                worker->first(sinkmsg.getId());
                tracker.setFirst(sinkmsg.getId());
                elapsed.start();
                break;
                
            case 2:

                worker->process(sinkmsg.getId(), sinkmsg.getData());
                
                // mark this message off.
                tracker.track(sinkmsg.getId());
                
                LOG4CXX_TRACE(logger, "msg: " << sinkmsg.getId())
                break;
                
            case 3:
                // this one usually comes from a vent.
                LOG4CXX_DEBUG(logger, "got last: " << sinkmsg.getId())
                worker->last(sinkmsg.getId());
                tracker.setLast(sinkmsg.getId());
                break;
                
            default:
                LOG4CXX_ERROR(logger, "Unknown message: " << sinkmsg.getCode())
                return false;
            }
            
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
        }
        else {
            LOG4CXX_INFO(logger, "No messages after 5s.")
            tracker.dump();
            if (retries > 5) {
                
                LOG4CXX_ERROR(logger, "Waited for long enough, (25s) for anything to come through")
                
                worker->results(elapsed.getTotal());
                LOG4CXX_INFO(logger, "Results written (probably incomplete).")
                
                // start tracking from the start.
                tracker.reset();
            }
            else {
                retries++;
            }
                
        }
	}
	
	if (!tracker.complete()) {
   		LOG4CXX_ERROR(logger, "Didn't get all the messages.")
	}
	
   	LOG4CXX_INFO(logger, "finished.")
	return true;
	
}

