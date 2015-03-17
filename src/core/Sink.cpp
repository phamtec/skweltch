
#include "Sink.hpp"

#include "ISinkWorker.hpp"
#include "Elapsed.hpp"
#include "SinkMsg.hpp"
#include "KillMsg.hpp"
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
    int timeout = 5000;
    int maxretries = 5;
 	while (!worker->shouldQuit()) {
	
 		int recved = Poller::NONE;
		try {
			recved = poller->poll(receiver, control, timeout);
		}
		catch (zmq::error_t &e) {
			if (string(e.what()) != "Interrupted system call") {
				LOG4CXX_ERROR(logger, "poll failed." << e.what())
			}
		}
		
        if (recved == Poller::MSG) {
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
                LOG4CXX_DEBUG(logger, "sink got first: " << sinkmsg.getId())
                worker->first(sinkmsg.getId());
                tracker.setFirst(sinkmsg.getId());
                elapsed.start();
                break;
                
            case 2:

                worker->process(sinkmsg.getId(), sinkmsg.getData());
                
                // mark this message off.
                tracker.track(sinkmsg.getId());
                
                LOG4CXX_TRACE(logger, "sink got msg: " << sinkmsg.getId())
                break;
                
            case 3:
                // this one usually comes from a vent.
                LOG4CXX_DEBUG(logger, "sink got last: " << sinkmsg.getId())
                worker->last(sinkmsg.getId());
                tracker.setLast(sinkmsg.getId());
                break;
                
            default:
                LOG4CXX_ERROR(logger, "Unknown message: " << sinkmsg.getCode())
                return false;
            }
            
            if (tracker.complete()) {
            
                 //  Calculate and report duration of batch
                int total_msec = elapsed.getTotal();

                // get results.
                worker->results(total_msec);
            
                // start tracking from the start.
                tracker.reset();
           	}
        }
        else if (recved == Poller::CONTROL) {
        	
        	zmq::message_t message;
			try {
	       		control->recv(&message);
	       		KillMsg kill(message);
	       		if (kill.value() == 0) {
					if (!tracker.complete()) {
						LOG4CXX_ERROR(logger, "Didn't get all the messages.")
					}
					return true;
	       		}
			}
			catch (zmq::error_t &e) {
				if (string(e.what()) != "Interrupted system call") {
					LOG4CXX_ERROR(logger, "recv failed." << e.what())
				}
			}
			
        }
        else if (recved == Poller::NONE)
        {
            LOG4CXX_INFO(logger, "No messages after " << timeout << " seconds.")
            tracker.dump();
            if (retries > maxretries) {
                
                LOG4CXX_ERROR(logger, "Waited for long enough, (" << (timeout * maxretries) << " for anything to come through")
                
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

	return true;
	
}

